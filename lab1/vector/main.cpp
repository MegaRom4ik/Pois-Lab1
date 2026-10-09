#include "Vector3.hpp"
#include "../cli/Input.hpp"

using ppois::Vector3;
namespace {
void display(const Vector3& vector) {
    std::cout << "Начало и конец: " << vector << "\nДлина: " << vector.length() << '\n';
}
void operation(int choice, Vector3& vector) {
    if (choice == 1) vector = cli::read<Vector3>("Введите x1 y1 z1 x2 y2 z2: ");
    else if (choice == 2) display(vector);
    else if (choice == 6) vector *= cli::read<double>("Множитель: ");
    else if (choice == 7) vector /= cli::read<double>("Делитель: ");
    else if (choice >= 3 && choice <= 9) {
        const auto other = cli::read<Vector3>("Второй вектор: x1 y1 z1 x2 y2 z2: ");
        if (choice == 3) vector += other;
        if (choice == 4) vector -= other;
        if (choice == 5) vector *= other;
        if (choice == 8) std::cout << "Косинус: " << vector.cosine(other) << '\n';
        if (choice == 9) std::cout << "Равны: " << (vector == other)
            << "; длина меньше: " << (vector < other)
            << "; длина больше: " << (vector > other) << '\n';
    } else throw std::invalid_argument("Нет такого пункта меню");
}
}
int main() {
    Vector3 vector;
    while (std::cin) {
        std::cout << "\nВектор: 1 — ввод, 2 — просмотр, 3 — сложение, 4 — вычитание,\n"
            "5 — векторное произведение, 6 — умножение на число, 7 — деление,\n"
            "8 — косинус угла, 9 — сравнение, 0 — выход\n";
        try {
            const auto choice = cli::read<int>("> ");
            if (choice == 0) break;
            operation(choice, vector);
        } catch (const std::exception& error) { std::cerr << error.what() << '\n'; }
    }
}
