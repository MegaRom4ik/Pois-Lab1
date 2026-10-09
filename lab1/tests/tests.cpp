#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../vector/Vector3.hpp"
#include "../tictactoe/TicTacToe.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

using ppois::Vector3;
using ppois::TicTacToe;
namespace {
Vector3 vector(double x, double y, double z) { return {{}, {x, y, z}}; }
void play(TicTacToe& game, std::initializer_list<std::size_t> indices) {
    for (const auto index : indices) game[index] = game.current_player();
}
}
TEST_CASE("Vector endpoints, displacement, copying and free-vector equality") {
    const Vector3 original({1, 2, 3}, {4, 6, 15});
    CHECK(original.start().x == 1);
    CHECK(original.start().y == 2);
    CHECK(original.start().z == 3);
    CHECK(original.end().x == 4);
    CHECK(original.end().y == 6);
    CHECK(original.end().z == 15);
    CHECK(original.length() == doctest::Approx(13));
    Vector3 copy(original), assigned;
    assigned = original;
    CHECK(copy == assigned);
    copy *= 2;
    CHECK(copy != original);
    CHECK(original == vector(3, 4, 12));
    CHECK(vector(1, 2, 3) != vector(1, 4, 3));
    CHECK(vector(1, 2, 3) != vector(1, 2, 4));
}
TEST_CASE("Vector arithmetic and compound operators") {
    const Vector3 left({1, 2, 3}, {2, 4, 6});
    const auto right = vector(4, 5, 6);
    CHECK(left + right == vector(5, 7, 9));
    CHECK(left - right == vector(-3, -3, -3));
    CHECK(left * right == vector(-3, 6, -3));
    CHECK((left + right).start().x == 1);
    CHECK(left * 2 == vector(2, 4, 6));
    CHECK(2 * left == left * 2);
    CHECK(left / 2 == vector(0.5, 1, 1.5));
    auto changed = left;
    changed += right;
    CHECK(changed == left + right);
    changed -= right;
    CHECK(changed == left);
    changed *= right;
    CHECK(changed == left * right);
    changed *= -2;
    changed /= -2;
    CHECK(changed == left * right);
    auto self = vector(1, 2, 3);
    self *= self;
    CHECK(self == Vector3{});
    CHECK(vector(1, 0, 0) * vector(0, 1, 0) == vector(0, 0, 1));
}
TEST_CASE("Length ordering and cosine") {
    const auto short_vector = vector(1, 0, 0), long_vector = vector(0, 2, 0);
    CHECK(short_vector < long_vector);
    CHECK(long_vector > short_vector);
    CHECK(short_vector <= short_vector);
    CHECK(long_vector >= long_vector);
    CHECK_FALSE(long_vector <= short_vector);
    CHECK_FALSE(short_vector >= long_vector);
    CHECK(short_vector.cosine(long_vector) == doctest::Approx(0));
    CHECK(short_vector.cosine(short_vector) == doctest::Approx(1));
    CHECK(short_vector.cosine(vector(-1, 0, 0)) == doctest::Approx(-1));
    CHECK(vector(1, 1, 0).cosine(short_vector) == doctest::Approx(1 / std::sqrt(2.0)));
    CHECK_THROWS_AS(short_vector.cosine(Vector3{}), std::domain_error);
    CHECK_THROWS_AS(Vector3{}.cosine(short_vector), std::domain_error);
    CHECK_THROWS_AS(short_vector / 0, std::domain_error);
}
TEST_CASE("Nonfinite values, overflow and transactional input") {
    const double infinity = std::numeric_limits<double>::infinity();
    const double largest = std::numeric_limits<double>::max();
    CHECK_THROWS_AS(vector(infinity, 0, 0), std::invalid_argument);
    CHECK_THROWS_AS(vector(0, infinity, 0), std::invalid_argument);
    CHECK_THROWS_AS(vector(0, 0, infinity), std::invalid_argument);
    CHECK_THROWS_AS(Vector3({infinity, 0, 0}, {}), std::invalid_argument);
    CHECK_THROWS_AS(Vector3({-largest, 0, 0}, {largest, 0, 0}), std::invalid_argument);
    CHECK_THROWS_AS(vector(1, 0, 0) * infinity, std::invalid_argument);
    CHECK_THROWS_AS(vector(1, 0, 0) / infinity, std::invalid_argument);
    CHECK_THROWS_AS(vector(largest, largest, 0).cosine(vector(1, 0, 0)), std::overflow_error);
    const auto original = vector(1, 2, 3);
    auto restored = original;
    std::stringstream stream;
    stream << original;
    CHECK(stream.precision() == 6);
    stream >> restored;
    CHECK(restored == original);
    for (const auto* text : {"bad", "0 0 0 1", "-1.7e308 0 0 1.7e308 0 0"}) {
        std::istringstream invalid(text);
        invalid >> restored;
        CHECK(invalid.fail());
        CHECK(restored == original);
    }
    const auto fractional = vector(0.123456789123456, -0.987654321987654, 0.25);
    std::stringstream precise;
    precise << fractional;
    precise >> restored;
    CHECK(restored == fractional);
}
TEST_CASE("Board size, cell proxies, turns and invalid moves") {
    CHECK_THROWS_AS(TicTacToe(0), std::invalid_argument);
    CHECK_THROWS_AS(TicTacToe(TicTacToe::max_size + 1), std::invalid_argument);
    TicTacToe game;
    CHECK(game.size() == 3);
    CHECK(game.current_player() == 'X');
    CHECK(static_cast<char>(game[0]) == '.');
    game[0] = 'X';
    CHECK(game.current_player() == 'O');
    CHECK_FALSE(game.can_place(0, 0));
    CHECK_FALSE(game.can_place(3, 0));
    CHECK_FALSE(game.can_place(0, 3));
    CHECK_THROWS_AS(game.place(0, 0, 'O'), std::logic_error);
    CHECK_THROWS_AS(game.place(0, 1, 'X'), std::invalid_argument);
    CHECK_THROWS_AS(game.place(0, 1, '?'), std::invalid_argument);
    CHECK_THROWS_AS(game.place(3, 0, 'O'), std::out_of_range);
    CHECK_THROWS_AS(game[9], std::out_of_range);
    const TicTacToe& constant = game;
    CHECK(constant[0] == 'X');
    CHECK_THROWS_AS(constant[9], std::out_of_range);
    TicTacToe other;
    other[0] = game[0];
    CHECK(other == game);
    CHECK_FALSE(other != game);
    other[1] = 'O';
    CHECK(other != game);
    auto retained = other[8];
    other = TicTacToe(1);
    CHECK_THROWS_AS((void)static_cast<char>(retained), std::out_of_range);
    CHECK_THROWS_AS(retained = 'X', std::out_of_range);
}
TEST_CASE("Every winning line for both players on multiple board sizes") {
    for (const std::size_t size : {2, 3, 4}) {
        for (const char winner : {'X', 'O'}) {
            for (int line = 0; line < 4; ++line) {
                TicTacToe game(size);
                std::vector<std::size_t> targets;
                for (std::size_t offset = 0; offset < size; ++offset) {
                    if (line == 0) targets.push_back(offset);
                    if (line == 1) targets.push_back(offset * size);
                    if (line == 2) targets.push_back(offset * (size + 1));
                    if (line == 3) targets.push_back((offset + 1) * (size - 1));
                }
                std::vector<std::size_t> fillers;
                for (std::size_t index = 0; index < size * size; ++index)
                    if (std::find(targets.begin(), targets.end(), index) == targets.end()) fillers.push_back(index);
                // Spread filler moves so they do not form a winning line first.
                if (size == 2 && winner == 'O') continue;
                if (size == 3 && line == 0) fillers = {3, 4, 8, 5, 6, 7};
                if (size == 4 && line == 0) fillers = {4, 5, 6, 15, 7, 8, 9, 10, 11, 12, 13, 14};
                std::size_t filler = 0;
                if (winner == 'O') game[fillers[filler++]] = 'X';
                for (std::size_t offset = 0; offset < size; ++offset) {
                    game[targets[offset]] = winner;
                    if (offset + 1 < size) game[fillers[filler++]] = game.current_player();
                }
                CHECK(game.winner() == winner);
                CHECK(game.finished());
                CHECK_FALSE(game.draw());
                CHECK_FALSE(game.can_place(size - 1, size - 1));
            }
        }
    }
}
TEST_CASE("Draw, reset, single-cell victory, and independent copies") {
    TicTacToe game;
    play(game, {0, 1, 2, 4, 3, 5, 7, 6, 8});
    CHECK(game.draw());
    CHECK(game.finished());
    CHECK(game.winner() == '.');
    CHECK_THROWS_AS(game.place(0, 0, game.current_player()), std::logic_error);
    TicTacToe copy(game), assigned;
    assigned = game;
    CHECK(copy == assigned);
    copy.reset();
    CHECK(copy.current_player() == 'X');
    CHECK_FALSE(copy.finished());
    CHECK_FALSE(copy.draw());
    CHECK(copy.winner() == '.');
    CHECK(copy != game);
    CHECK(copy == TicTacToe{});
    TicTacToe single(1);
    single[0] = 'X';
    CHECK(single.winner() == 'X');
}
TEST_CASE("Game serialization and rejected histories") {
    TicTacToe original(4), restored;
    play(original, {0, 4, 1, 5, 2, 6, 3});
    std::stringstream saved;
    saved << original;
    saved >> restored;
    CHECK_FALSE(saved.fail());
    CHECK(restored == original);
    CHECK(restored.winner() == 'X');
    for (const auto* text : {"bad", "0 0", "1001 0", "3 10", "3 1", "3 1 9", "3 2 0 0", "1 2 0 0", "2 4 0 2 1 3"}) {
        std::istringstream invalid(text);
        invalid >> restored;
        CHECK(invalid.fail());
        CHECK(restored == original);
    }
    std::istringstream empty("2 0");
    empty >> restored;
    CHECK(restored == TicTacToe(2));
}
