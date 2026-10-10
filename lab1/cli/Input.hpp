#pragma once
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace cli {
inline std::string read_line(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) throw std::runtime_error("Ввод завершён");
    return line;
}
template <typename Value>
Value read(const std::string& prompt) {
    const auto line = read_line(prompt);
    std::istringstream input(line);
    Value value{};
    if (!(input >> value)) throw std::invalid_argument("Некорректный ввод");
    input >> std::ws;
    if (!input.eof()) throw std::invalid_argument("Лишние данные в строке");
    return value;
}
}
