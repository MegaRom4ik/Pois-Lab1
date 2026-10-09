#include "TicTacToe.hpp"
#include "../cli/Input.hpp"
#include <fstream>
#include <iomanip>
#include <string>

using ppois::TicTacToe;
namespace {
enum class Action { exit, new_game, move, reset, save, load };
void display_board(const TicTacToe& game) {
    const auto label_width = static_cast<int>(std::to_string(game.size()).size());
    const auto cell_width = label_width + 1;
    std::cout << std::setw(label_width + 2) << "";
    for (std::size_t column = 0; column < game.size(); ++column)
        std::cout << std::setw(cell_width) << column + 1;
    std::cout << '\n';
    for (std::size_t row = 0; row < game.size(); ++row) {
        std::cout << std::setw(label_width) << row + 1 << " |";
        for (std::size_t column = 0; column < game.size(); ++column)
            std::cout << std::setw(cell_width) << game[row * game.size() + column];
        std::cout << '\n';
    }
}
void display(const TicTacToe& game) {
    display_board(game);
    if (game.winner() != '.') std::cout << "Победитель: " << game.winner() << '\n';
    else if (game.draw()) std::cout << "Ничья\n";
    else std::cout << "Ходит: " << game.current_player() << '\n';
}
void move(TicTacToe& game) {
    const auto row = cli::read<std::size_t>("Строка (начиная с 1): ");
    const auto column = cli::read<std::size_t>("Столбец (начиная с 1): ");
    if (row == 0 || column == 0) throw std::out_of_range("Нумерация начинается с 1");
    game.place(row - 1, column - 1, game.current_player());
}
void save(const TicTacToe& game) {
    std::ofstream file(cli::read_line("Путь для сохранения: "));
    if (!(file << game)) throw std::runtime_error("Не удалось сохранить игру");
}
void load(TicTacToe& game) {
    std::ifstream file(cli::read_line("Путь к файлу: "));
    if (!(file >> game)) throw std::runtime_error("Не удалось загрузить игру");
}
void operation(Action action, TicTacToe& game) {
    switch (action) {
    case Action::new_game:
        game = TicTacToe(cli::read<std::size_t>("Размер поля (1..1000): ")); break;
    case Action::move: move(game); break;
    case Action::reset: game.reset(); break;
    case Action::save: save(game); break;
    case Action::load: load(game); break;
    default: throw std::invalid_argument("Нет такого пункта меню");
    }
}
}
int main() {
    TicTacToe game;
    while (std::cin) {
        display(game);
        std::cout << "1 — новая игра, 2 — ход, 3 — очистить поле, 4 — сохранить, 5 — загрузить, 0 — выход\n";
        try {
            const auto action = static_cast<Action>(cli::read<int>("> "));
            if (action == Action::exit) break;
            operation(action, game);
        } catch (const std::exception& error) { std::cerr << error.what() << '\n'; }
    }
}
