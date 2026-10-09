#include "Vector3.hpp"
#include "../cli/Input.hpp"

using ppois::Vector3;
namespace {
enum class Action {
    exit, input, view, add, subtract, cross_product,
    multiply, divide, cosine, compare
};
void display(const Vector3& vector) {
    std::cout << "Начало и конец: " << vector << "\nДлина: " << vector.length() << '\n';
}
void compare(const Vector3& vector, const Vector3& other) {
    std::cout << "Равны: " << (vector == other)
        << "; длина меньше: " << (vector < other)
        << "; длина больше: " << (vector > other) << '\n';
}
void binary_operation(Action action, Vector3& vector) {
    const auto other = cli::read<Vector3>("Второй вектор: x1 y1 z1 x2 y2 z2: ");
    switch (action) {
    case Action::add: vector += other; break;
    case Action::subtract: vector -= other; break;
    case Action::cross_product: vector *= other; break;
    case Action::cosine: std::cout << "Косинус: " << vector.cosine(other) << '\n'; break;
    case Action::compare: compare(vector, other); break;
    default: throw std::invalid_argument("Нет такого пункта меню");
    }
}
void operation(Action action, Vector3& vector) {
    switch (action) {
    case Action::input: vector = cli::read<Vector3>("Введите x1 y1 z1 x2 y2 z2: "); break;
    case Action::view: display(vector); break;
    case Action::multiply: vector *= cli::read<double>("Множитель: "); break;
    case Action::divide: vector /= cli::read<double>("Делитель: "); break;
    case Action::add: case Action::subtract: case Action::cross_product:
    case Action::cosine: case Action::compare: binary_operation(action, vector); break;
    default: throw std::invalid_argument("Нет такого пункта меню");
    }
}
void menu() {
    std::cout << "\nВектор: 1 — ввод, 2 — просмотр, 3 — сложение, 4 — вычитание,\n"
        "5 — векторное произведение, 6 — умножение на число, 7 — деление,\n"
        "8 — косинус угла, 9 — сравнение, 0 — выход\n";
}
}
int main() {
    Vector3 vector;
    while (std::cin) {
        menu();
        try {
            const auto action = static_cast<Action>(cli::read<int>("> "));
            if (action == Action::exit) break;
            operation(action, vector);
        } catch (const std::exception& error) { std::cerr << error.what() << '\n'; }
    }
}
