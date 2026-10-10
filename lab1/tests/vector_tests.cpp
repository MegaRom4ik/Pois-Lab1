#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../vector/Vector3.hpp"
#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

using ppois::Vector3;
namespace {
Vector3 vector(double x, double y, double z) { return {{}, {x, y, z}}; }
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
