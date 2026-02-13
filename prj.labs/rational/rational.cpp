#include "rational.hpp"
#include <iostream>   
#include <numeric>    
#include <stdexcept>  
#include <limits>     
#include <cstdint>
#include <cstdlib>   

Rational::Rational(const int32_t num, const int32_t den) : num_(num), den_(den) {
    if (den_ == 0) throw std::invalid_argument("Zero denominator in Rational ctor");
    Normalize();
}

void Rational::Normalize() noexcept {
    if (num_ == 0) { den_ = 1; return; }
    if (den_ < 0) { num_ = -num_; den_ = -den_; }
    const int32_t g = static_cast<int32_t>(std::gcd(std::abs(num_), den_));
    if (g > 1) { num_ /= g; den_ /= g; }
}

bool Rational::operator==(const Rational& rhs) const noexcept { return num_ == rhs.num_ && den_ == rhs.den_; }
bool Rational::operator!=(const Rational& rhs) const noexcept { return !(*this == rhs); }
bool Rational::operator<(const Rational& rhs) const noexcept {
    return static_cast<int64_t>(num_) * rhs.den_ < static_cast<int64_t>(rhs.num_) * den_;
}
bool Rational::operator<=(const Rational& rhs) const noexcept { return !(rhs < *this); }
bool Rational::operator>(const Rational& rhs) const noexcept { return rhs < *this; }
bool Rational::operator>=(const Rational& rhs) const noexcept { return !(*this < rhs); }

static inline bool fits_int32(int64_t v) {
    return v >= std::numeric_limits<int32_t>::min() && v <= std::numeric_limits<int32_t>::max();
}

Rational& Rational::operator+=(const Rational& rhs) noexcept {
    const int64_t a = num_, b = den_, c = rhs.num_, d = rhs.den_;
    const int64_t n = a * d + c * b;
    const int64_t m = b * d;
    const int64_t g = std::gcd(std::llabs(n), std::llabs(m));
    const int64_t nn = n / (g == 0 ? 1 : g);
    const int64_t mm = m / (g == 0 ? 1 : g);
    if (!fits_int32(nn) || !fits_int32(mm)) { num_ = 0; den_ = 1; return *this; }
    num_ = static_cast<int32_t>(nn);
    den_ = static_cast<int32_t>(mm);
    return *this;
}

Rational& Rational::operator-=(const Rational& rhs) noexcept { return operator+=(Rational(-rhs.num_, rhs.den_)); }

Rational& Rational::operator*=(const Rational& rhs) noexcept {
    int64_t a = num_, b = den_, c = rhs.num_, d = rhs.den_;
    int64_t g1 = std::gcd(std::llabs(a), std::llabs(d)); a /= g1; d /= g1;
    int64_t g2 = std::gcd(std::llabs(c), std::llabs(b)); c /= g2; b /= g2;
    int64_t nn = a * c;
    int64_t dd = b * d;
    if (!fits_int32(nn) || !fits_int32(dd)) { num_ = 0; den_ = 1; return *this; }
    num_ = static_cast<int32_t>(nn);
    den_ = static_cast<int32_t>(dd);
    return *this;
}

Rational& Rational::operator/=(const Rational& rhs) {
    if (rhs.num_ == 0) throw std::invalid_argument("Division by zero Rational");
    int64_t a = num_, b = den_, c = rhs.num_, d = rhs.den_;
    int64_t g1 = std::gcd(std::llabs(a), std::llabs(c)); a /= g1; c /= g1;
    int64_t g2 = std::gcd(std::llabs(d), std::llabs(b)); d /= g2; b /= g2;
    int64_t nn = a * d;
    int64_t dd = b * c;
    if (dd < 0) { nn = -nn; dd = -dd; }
    if (!fits_int32(nn) || !fits_int32(dd)) throw std::overflow_error("Rational overflow");
    num_ = static_cast<int32_t>(nn);
    den_ = static_cast<int32_t>(dd);
    return *this;
}

Rational& Rational::operator+=(const int32_t rhs) noexcept { return operator+=(Rational(rhs)); }
Rational& Rational::operator-=(const int32_t rhs) noexcept { return operator-=(Rational(rhs)); }
Rational& Rational::operator*=(const int32_t rhs) noexcept { return operator*=(Rational(rhs)); }
Rational& Rational::operator/=(const int32_t rhs) {
    if (rhs == 0) throw std::invalid_argument("Division by zero integer");
    return operator/=(Rational(rhs));
}

std::ostream& Rational::WriteTo(std::ostream& ostrm) const noexcept {
    ostrm << num_ << '/' << den_;
    return ostrm;
}

std::istream& Rational::ReadFrom(std::istream& istrm) noexcept {
    int32_t n = 0, d = 1; char sep = 0;
    if (!(istrm >> n)) { istrm.setstate(std::ios::failbit); return istrm; }
    
    // Fix: читаем разделитель, но если его нет или это пробел - возвращаем как целое число
    if (istrm.peek() == '/') {
        istrm >> sep;
        istrm >> d;
    } else {
        d = 1;
    }
    
    if (d == 0) { istrm.setstate(std::ios::failbit); return istrm; }
    num_ = n; den_ = d; Normalize();
    return istrm;
}

Rational operator+(const Rational& lhs, const Rational& rhs) noexcept { Rational tmp(lhs); tmp += rhs; return tmp; }
Rational operator-(const Rational& lhs, const Rational& rhs) noexcept { Rational tmp(lhs); tmp -= rhs; return tmp; }
Rational operator*(const Rational& lhs, const Rational& rhs) noexcept { Rational tmp(lhs); tmp *= rhs; return tmp; }
Rational operator/(const Rational& lhs, const Rational& rhs) { Rational tmp(lhs); tmp /= rhs; return tmp; }

Rational operator+(const Rational& lhs, const int32_t rhs) noexcept { return lhs + Rational(rhs); }
Rational operator-(const Rational& lhs, const int32_t rhs) noexcept { return lhs - Rational(rhs); }
Rational operator*(const Rational& lhs, const int32_t rhs) noexcept { return lhs * Rational(rhs); }
Rational operator/(const Rational& lhs, const int32_t rhs) { return lhs / Rational(rhs); }

Rational operator+(const int32_t lhs, const Rational& rhs) noexcept { return Rational(lhs) + rhs; }
Rational operator-(const int32_t lhs, const Rational& rhs) noexcept { return Rational(lhs) - rhs; }
Rational operator*(const int32_t lhs, const Rational& rhs) noexcept { return Rational(lhs) * rhs; }
Rational operator/(const int32_t lhs, const Rational& rhs) { return Rational(lhs) / rhs; }

std::ostream& operator<<(std::ostream& ostrm, const Rational& rhs) noexcept { return rhs.WriteTo(ostrm); }
std::istream& operator>>(std::istream& istrm, Rational& rhs) noexcept { return rhs.ReadFrom(istrm); }
