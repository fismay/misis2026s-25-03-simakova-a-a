#ifndef RATIONAL_RATIONAL_HPP_20231113
#define RATIONAL_RATIONAL_HPP_20231113

#include <iosfwd>
#include <cstdint>

class Rational {
public:
    Rational() = default;
    Rational(const Rational&) = default;
    Rational(Rational&&) = default;
    Rational(const int32_t num, const int32_t den = 1);
    ~Rational() = default;
    Rational& operator=(const Rational&) = default;
    Rational& operator=(Rational&&) = default;

    int32_t num() const noexcept { return num_; }
    int32_t den() const noexcept { return den_; }

    bool operator==(const Rational& rhs) const noexcept;
    bool operator!=(const Rational& rhs) const noexcept;
    bool operator<(const Rational& rhs) const noexcept;
    bool operator<=(const Rational& rhs) const noexcept;
    bool operator>(const Rational& rhs) const noexcept;
    bool operator>=(const Rational& rhs) const noexcept;

    Rational& operator+=(const Rational& rhs) noexcept;
    Rational& operator-=(const Rational& rhs) noexcept;
    Rational& operator*=(const Rational& rhs) noexcept;
    Rational& operator/=(const Rational& rhs);

    Rational& operator+=(const int32_t rhs) noexcept;
    Rational& operator-=(const int32_t rhs) noexcept;
    Rational& operator*=(const int32_t rhs) noexcept;
    Rational& operator/=(const int32_t rhs);

    std::ostream& WriteTo(std::ostream& ostrm) const noexcept;
    std::istream& ReadFrom(std::istream& istrm) noexcept;

private:
    int32_t num_ = 0;
    int32_t den_ = 1;
    void Normalize() noexcept;
};

Rational operator+(const Rational& lhs, const Rational& rhs) noexcept;
Rational operator-(const Rational& lhs, const Rational& rhs) noexcept;
Rational operator*(const Rational& lhs, const Rational& rhs) noexcept;
Rational operator/(const Rational& lhs, const Rational& rhs);

Rational operator+(const Rational& lhs, const int32_t rhs) noexcept;
Rational operator-(const Rational& lhs, const int32_t rhs) noexcept;
Rational operator*(const Rational& lhs, const int32_t rhs) noexcept;
Rational operator/(const Rational& lhs, const int32_t rhs);

Rational operator+(const int32_t lhs, const Rational& rhs) noexcept;
Rational operator-(const int32_t lhs, const Rational& rhs) noexcept;
Rational operator*(const int32_t lhs, const Rational& rhs) noexcept;
Rational operator/(const int32_t lhs, const Rational& rhs);

std::ostream& operator<<(std::ostream& ostrm, const Rational& rhs) noexcept;
std::istream& operator>>(std::istream& istrm, Rational& rhs) noexcept;

#endif
