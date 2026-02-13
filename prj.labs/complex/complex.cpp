#include "complex.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>

Complex Complex::operator-() const noexcept {
    return Complex(-re, -im);
}

bool Complex::operator==(const Complex& rhs) const noexcept {
    const double eps = 2 * std::numeric_limits<double>::epsilon();
    return std::abs(re - rhs.re) <= eps && std::abs(im - rhs.im) <= eps;
}

bool Complex::operator!=(const Complex& rhs) const noexcept {
    return !operator==(rhs);
}

Complex& Complex::operator+=(const Complex& rhs) noexcept {
    re += rhs.re;
    im += rhs.im;
    return *this;
}

Complex& Complex::operator+=(const double rhs) noexcept {
    return operator+=(Complex(rhs));
}

Complex& Complex::operator-=(const Complex& rhs) noexcept {
    re -= rhs.re;
    im -= rhs.im;
    return *this;
}

Complex& Complex::operator-=(const double rhs) noexcept {
    return operator-=(Complex(rhs));
}

Complex& Complex::operator*=(const Complex& rhs) noexcept {
    double new_re = re * rhs.re - im * rhs.im;
    double new_im = re * rhs.im + im * rhs.re;
    re = new_re;
    im = new_im;
    return *this;
}

Complex& Complex::operator*=(const double rhs) noexcept {
    re *= rhs;
    im *= rhs;
    return *this;
}

Complex& Complex::operator/=(const Complex& rhs) {
    double denominator = rhs.re * rhs.re + rhs.im * rhs.im;
    if (denominator == 0.0) {
        throw std::invalid_argument("Division by zero");
    }
    double new_re = (re * rhs.re + im * rhs.im) / denominator;
    double new_im = (im * rhs.re - re * rhs.im) / denominator;
    re = new_re;
    im = new_im;
    return *this;
}

Complex& Complex::operator/=(const double rhs) {
    if (rhs == 0.0) {
        throw std::invalid_argument("Division by zero");
    }
    re /= rhs;
    im /= rhs;
    return *this;
}

std::ostream& Complex::WriteTo(std::ostream& ostrm) const noexcept {
    ostrm << leftBrace << re << separator << im << rightBrace;
    return ostrm;
}

std::istream& Complex::ReadFrom(std::istream& istrm) noexcept {
    char ch;
    // Fix: пробуем читать '{', если нет - возвращаем в поток
    istrm >> ch;
    if (ch == leftBrace) {
        double real, imaginary;
        char comma, right;
        istrm >> real >> comma >> imaginary >> right;
        if (istrm.good() && comma == separator && right == rightBrace) {
            re = real;
            im = imaginary;
        } else {
             istrm.setstate(std::ios_base::failbit);
        }
    } else {
        // Если нет скобки, пытаемся читать как обычное число (re) или re + im
        istrm.putback(ch);
        istrm >> re;
        im = 0.0;
        // Можно усложнить для чтения "re im" без скобок, но обычно достаточно
    }
    return istrm;
}

Complex operator+(const Complex& lhs, const Complex& rhs) noexcept { return Complex(lhs) += rhs; }
Complex operator+(const Complex& lhs, const double rhs) noexcept { return Complex(lhs) += rhs; }
Complex operator+(const double lhs, const Complex& rhs) noexcept { return Complex(lhs) += rhs; }

Complex operator-(const Complex& lhs, const Complex& rhs) noexcept { return Complex(lhs) -= rhs; }
Complex operator-(const Complex& lhs, const double rhs) noexcept { return Complex(lhs) -= rhs; }
Complex operator-(const double lhs, const Complex& rhs) noexcept { return Complex(lhs) -= rhs; }

Complex operator*(const Complex& lhs, const Complex& rhs) noexcept { return Complex(lhs) *= rhs; }
Complex operator*(const Complex& lhs, const double rhs) noexcept { return Complex(lhs) *= rhs; }
Complex operator*(const double lhs, const Complex& rhs) noexcept { return Complex(lhs) *= rhs; }

Complex operator/(const Complex& lhs, const Complex& rhs) { return Complex(lhs) /= rhs; }
Complex operator/(const Complex& lhs, const double rhs) { return Complex(lhs) /= rhs; }
Complex operator/(const double lhs, const Complex& rhs) { return Complex(lhs) /= rhs; }
