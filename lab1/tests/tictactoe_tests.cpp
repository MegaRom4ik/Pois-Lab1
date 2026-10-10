#include "doctest.h"
#include "../tictactoe/TicTacToe.hpp"
#include <algorithm>
#include <sstream>
#include <stdexcept>

using ppois::TicTacToe;
namespace {
void play(TicTacToe& game, std::initializer_list<std::size_t> indices) {
    for (const auto index : indices) game[index] = game.current_player();
}
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
