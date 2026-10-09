#include "TicTacToe.hpp"
#include "../cli/Input.hpp"
#include <fstream>

using ppois::TicTacToe;
namespace {
void display(const TicTacToe& game) {
    for (std::size_t row = 0; row < game.size(); ++row) {
        for (std::size_t column = 0; column < game.size(); ++column)
            std::cout << game[row * game.size() + column] << ' ';
        std::cout << '\n';
    }
    if (game.winner() != '.') std::cout << "Победитель: " << game.winner() << '\n';
    else if (game.draw()) std::cout << "Ничья\n";
    else std::cout << "Ходит: " << game.current_player() << '\n';
}
void operation(int choice, TicTacToe& game) {
    if (choice == 1) game = TicTacToe(cli::read<std::size_t>("Размер поля (1..1000): "));
    else if (choice == 2) {
        const auto row = cli::read<std::size_t>("Строка (начиная с 1): ");
        const auto column = cli::read<std::size_t>("Столбец (начиная с 1): ");
        if (row == 0 || column == 0) throw std::out_of_range("Нумерация начинается с 1");
        game.place(row - 1, column - 1, game.current_player());
    } else if (choice == 3) game.reset();
    else if (choice == 4) {
        std::ofstream file(cli::read_line("Путь для сохранения: "));
        if (!(file << game)) throw std::runtime_error("Не удалось сохранить игру");
    } else if (choice == 5) {
        std::ifstream file(cli::read_line("Путь к файлу: "));
        if (!(file >> game)) throw std::runtime_error("Не удалось загрузить игру");
    } else throw std::invalid_argument("Нет такого пункта меню");
}
}
int main() {
    TicTacToe game;
    while (std::cin) {
        display(game);
        std::cout << "1 — новая игра, 2 — ход, 3 — очистить поле, 4 — сохранить, 5 — загрузить, 0 — выход\n";
        try {
            const auto choice = cli::read<int>("> ");
            if (choice == 0) break;
            operation(choice, game);
        } catch (const std::exception& error) { std::cerr << error.what() << '\n'; }
    }
}
