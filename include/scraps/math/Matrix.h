/**
* Copyright 2016 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <scraps/config.h>

#include <scraps/math/Vector.h>

#include <algorithm>
#include <cmath>
#include <ostream>

// Linux defines major and minor macros in sys/sysmacros.h for backward-compatibility. No one should use these.
#ifdef minor
#undef minor
#endif
#ifdef major
#undef major
#endif

namespace scraps::math {

template <typename T, size_t R, size_t C> struct Matrix;

template <typename T, size_t R, size_t C>
class MatrixBase {
public:
    constexpr MatrixBase() {}

    constexpr MatrixBase(std::initializer_list<T> l) {
        size_t r{0}, c{0};
        for (auto& v : l) {
            (*this)(r, c) = v;
            if (++c >= C) {
                c = 0;
                ++r;
            }
        }
    }

    constexpr T& operator()(size_t r, size_t c) { return at(r, c); }
    constexpr T operator()(size_t r, size_t c) const { return at(r, c); }

    constexpr T& at(size_t r, size_t c) { return _columnMajor[c * R + r]; }
    constexpr T at(size_t r, size_t c) const { return _columnMajor[c * R + r]; }

    template <size_t C2>
    constexpr Matrix<T, R, C2> operator*(const Matrix<T, C, C2>& right) const {
        Matrix<T, R, C2> ret;
        for (size_t i = 0; i < R; ++i) {
            for (size_t j = 0; j < C2; ++j) {
                T sum = 0;
                for (size_t k = 0; k < C; ++k) {
                    sum += at(i, k) * right(k, j);
                }
                ret(i, j) = sum;
            }
        }
        return ret;
    }

    template <typename U>
    constexpr auto operator*(const Vector<U, C>& right) const {
        Vector<decltype(T{} * U{}), R> ret;
        for (size_t i = 0; i < R; ++i) {
            decltype(T{} * U{}) sum = 0;
            for (size_t j = 0; j < C; ++j) {
                sum += at(i, j) * right[j];
            }
            ret[i] = sum;
        }
        return ret;
    }

    template <typename U>
    constexpr auto operator/(U divisor) const {
        Matrix<decltype(at(0, 0) / divisor), R, C> ret;
        for (size_t i = 0; i < R; ++i) {
            for (size_t j = 0; j < C; ++j) {
                ret(i, j) = at(i, j) / divisor;
            }
        }
        return ret;
    }

    constexpr bool operator==(const Matrix<T, R, C>& other) const {
        for (size_t i = 0; i < R * C; ++i) {
            if (_columnMajor[i] != other._columnMajor[i]) {
                return false;
            }
        }
        return true;
    }

    constexpr bool operator!=(const Matrix<T, R, C>& other) const {
        return !(*this == other);
    }

    const T* columnMajor() const {
        return _columnMajor;
    }

    template <typename U = T>
    constexpr std::enable_if_t<R == C && R == 1, U> determinant() const {
        return at(0, 0);
    }

    template <typename U = T>
    constexpr std::enable_if_t<R == C && (R > 1), U> determinant(std::nullptr_t _ = {}) const {
        U ret = 0;

        // Something about the combination of the parameters and the loop really don't jive with
        // cppcheck. Just suppress the syntax error linting for now.
        // cppcheck-suppress syntaxError
        for (size_t n = 0; n < R; ++n) {
            ret += at(0, n) * cofactor(0, n);
        }
        return ret;
    }

    template <typename U = T>
    constexpr std::enable_if_t<R == C && (R > 1), U> cofactor(size_t i, size_t j) const {
        return (((i + j) & 1) ? -1 : 1) * minor(i, j);
    }

    template <typename U = T>
    constexpr std::enable_if_t<R == C && (R > 1), U> minor(size_t i, size_t j) const {
        Matrix<U, R - 1, C - 1> sub;
        for (size_t si = 0; si < R - 1; ++si) {
            for (size_t sj = 0; sj < C - 1; ++sj) {
                sub(si, sj) = at(si >= i ? si + 1 : si, sj >= j ? sj + 1 : sj);
            }
        }
        return sub.determinant();
    }

    template <typename U = T>
    constexpr std::enable_if_t<R == C && (R > 1), Matrix<U, R, C>> cofactorMatrix() const {
        Matrix<U, R, C> ret;
        for (size_t i = 0; i < R; ++i) {
            for (size_t j = 0; j < C; ++j) {
                ret(i, j) = cofactor(i, j);
            }
        }
        return ret;
    }

    constexpr Matrix<T, C, R> transpose() const {
        Matrix<T, C, R> ret;
        for (size_t i = 0; i < R; ++i) {
            for (size_t j = 0; j < C; ++j) {
                ret(j, i) = at(i, j);
            }
        }
        return ret;
    }

    template <typename U = T>
    constexpr std::enable_if_t<R == C && (R > 1), Matrix<U, R, C>> adjugate() const {
        return cofactorMatrix().transpose();
    }

    template <typename DivisorType = double, typename U = T>
    constexpr auto inverse() const -> std::enable_if_t<R == C && (R > 1), Matrix<decltype(U{} / DivisorType{}), R, C>> {
        DivisorType det = determinant();
        assert(det);
        return adjugate() / det;
    }

    template <typename U = T>
    static constexpr std::enable_if_t<R == C, Matrix<U, R, C>> Scaling(const Vector<U, R>& vector) {
        Matrix<U, R, C> ret;
        for (size_t i = 0; i < R; ++i) {
            ret(i, i) = vector[i];
        }
        return ret;
    }

    friend std::ostream& operator<<(std::ostream& os, const MatrixBase& m) {
        os << '[';
        for (size_t i = 0; i < R; ++i) {
            if (i) { os << ", "; }
            os << '[';
            for (size_t j = 0; j < C; ++j) {
                if (j) { os << ", "; }
                os << m.at(i, j);
            }
            os << ']';
        }
        return os << ']';
    }

protected:
    T _columnMajor[R * C]{0};
};

#define SCRAPS_MATH_MATRIX_INHERITANCE(rows, cols) \
    constexpr Matrix() : MatrixBase<T, rows, cols>{} {} \
    template <typename... Args> \
    constexpr Matrix(Args&&... args) : MatrixBase<T, rows, cols>{std::forward<Args>(args)...} {}

template <typename T, size_t R, size_t C>
struct Matrix : MatrixBase<T, R, C> {
    SCRAPS_MATH_MATRIX_INHERITANCE(R, C)
};

template <typename T>
struct Matrix<T, 4, 4> : MatrixBase<T, 4, 4> {
    SCRAPS_MATH_MATRIX_INHERITANCE(4, 4)

    static constexpr Matrix Frustum(T left, T right, T bottom, T top, T near, T far) {
        Matrix ret;
        ret._columnMajor[ 0] = (2 * near) / (right - left);
        ret._columnMajor[ 5] = (2 * near) / (top - bottom);
        ret._columnMajor[ 8] = (right + left) / (right - left);
        ret._columnMajor[ 9] = (top + bottom) / (top - bottom);
        ret._columnMajor[10] = (far + near) / (far - near);
        ret._columnMajor[11] = -1;
        ret._columnMajor[14] = (2 * far * near) / (far - near);
        return ret;
    }

    static constexpr Matrix Perspective(T fovy, T aspect, T near, T far) {
        auto top = near * tan(fovy / 360.0 * M_PI);
        auto right = static_cast<T>(top * aspect);
        return Frustum(-right, right, static_cast<T>(-top), static_cast<T>(top), near, far);
    }

    static constexpr Matrix Translation(T x, T y, T z) {
        Matrix ret;
        ret._columnMajor[ 0] = 1;
        ret._columnMajor[ 5] = 1;
        ret._columnMajor[10] = 1;
        ret._columnMajor[12] = x;
        ret._columnMajor[13] = y;
        ret._columnMajor[14] = z;
        ret._columnMajor[15] = 1;
        return ret;
    }
};

} // namespace scraps::math
