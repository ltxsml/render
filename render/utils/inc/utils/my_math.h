#ifndef TINY_RENDER_MY_MATH_H
#define TINY_RENDER_MY_MATH_H

#include <cassert>
#include <cmath>

/**
 * n维向量
 * @tparam DIM
 * @tparam T
 */
template<size_t DIM, typename T>
struct Vec {
  Vec() {
    for (auto i = 0; i < DIM; ++i) data_[i] = T{};
  }

  T &operator[](const size_t i) {
    assert(i < DIM);
    return data_[i];
  }

  const T &operator[](const size_t i) const {
    assert(i < DIM);
    return data_[i];
  }

private:
  T data_[DIM];
};

/**
 * 2维向量
 * @tparam T
 */
template<typename T>
struct Vec<2, T> {
  T x;
  T y;

  Vec() : x(T()), y(T()) {}

  Vec(T x, T y) : x(x), y(y) {}

  T &operator[](const size_t i) {
    assert(i < 2);
    if (i == 0) return x;
    if (i == 1) return y;
    return x;
  }

  const T &operator[](const size_t i) const {
    assert(i < 2);
    if (i == 0) return x;
    if (i == 1) return y;
    return x;
  }
};

/**
 * 3维向量
 * @tparam T
 */
template<typename T>
struct Vec<3, T> {
  T x;
  T y;
  T z;

  Vec() : x(T()), y(T()), z(T()) {}

  Vec(T x, T y, T z) : x(x), y(y), z(z) {}

  T &operator[](const size_t i) {
    assert(i < 3);
    if (i == 0) return x;
    if (i == 1) return y;
    if (i == 2) return z;
    return x;
  }

  const T &operator[](const size_t i) const {
    assert(i < 3);
    if (i == 0) return x;
    if (i == 1) return y;
    if (i == 2) return z;
    return x;
  }
};

/**
 * 4维向量
 * @tparam T
 */
template<typename T>
struct Vec<4, T> {
  T x;
  T y;
  T z;
  T w;

  Vec() : x(T()), y(T()), z(T()), w(T()) {}

  Vec(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

  T &operator[](const size_t i) {
    assert(i < 4);
    if (i == 0) return x;
    if (i == 1) return y;
    if (i == 2) return z;
    if (i == 3) return w;
    return x;
  }

  const T &operator[](const size_t i) const {
    assert(i < 4);
    if (i == 0) return x;
    if (i == 1) return y;
    if (i == 2) return z;
    if (i == 3) return w;
    return x;
  }

  Vec<4, T> &normalize(T l = 1) {
    *this = (*this) * (l / norm());
    return *this;
  }

private:
  float norm() { return std::sqrt(x * x + y * y + z * z); }
};

/////////////////////////////////////////////////////////////////////////////////

// 点乘
template<size_t DIM, typename T>
T operator*(const Vec<DIM, T> &lv, const Vec<DIM, T> &rv) {
  T ret = T();
  for (int i = 0; i < DIM; ++i) ret += lv[i] * rv[i];
  return ret;
}

// 叉乘，针对4维（齐次）向量，不考虑w分量
template<typename T>
Vec<4, T> Cross(const Vec<4, T> &lv, const Vec<4, T> &rv) {
  return Vec<4, T>{
          lv.y * rv.z - lv.z * rv.y, lv.z * rv.x - lv.x * rv.z, lv.x * rv.y - lv.y * rv.x, lv.w
  };
}

// 加法
template<size_t DIM, typename T>
Vec<DIM, T> operator+(const Vec<DIM, T> &lv, const Vec<DIM, T> &rv) {
  Vec<DIM, T> ret;
  for (int i = 0; i < DIM; ++i) ret[i] = lv[i] + rv[i];
  return ret;
}

// 减法
template<size_t DIM, typename T>
Vec<DIM, T> operator-(const Vec<DIM, T> &lv, const Vec<DIM, T> &rv) {
  Vec<DIM, T> ret;
  for (int i = 0; i < DIM; ++i) ret[i] = lv[i] - rv[i];
  return ret;
}

// 乘法
template<size_t DIM, typename T, typename U>
Vec<DIM, T> operator*(const Vec<DIM, T> &v, U k) {
  Vec<DIM, T> ret;
  for (int i = 0; i < DIM; ++i) ret[i] = v[i] * k;
  return ret;
}

// 除法，不考虑w分量
template<size_t DIM, typename T, typename U>
Vec<DIM, T> operator/(const Vec<DIM, T> &v, U k) {
  Vec<DIM, T> ret;
  ret.w = v.w;
  for (int i = 0; i < DIM; ++i) ret[i] = v[i] / k;
  return ret;
}

/////////////////////////////////////////////////////////////////////////////////

/**
 * 方阵
 * @tparam T
 */
template<size_t DIM, typename T>
struct Matrix {
  Vec<DIM, T> &operator[](const size_t i) {
    assert(i < DIM);
    return rows_[i];
  }

  const Vec<DIM, T> &operator[](const size_t i) const {
    assert(i < DIM);
    return rows_[i];
  }

  Vec<DIM, T> Col(const size_t j) const {
    assert(j < DIM);
    Vec<DIM, T> ret;
    for (size_t i = 0; i < DIM; ++i) ret[i] = rows_[i][j];
    return ret;
  }

private:
  Vec<DIM, T> rows_[DIM];
};

/**
  * 方阵乘法
  */
template<size_t DIM, typename T>
Matrix<DIM, T> operator*(const Matrix<DIM, T> &lm, const Matrix<DIM, T> &rm) {
  Matrix<DIM, T> ret;
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      ret[i][j] = lm[i] * rm.Col(j);
    }
  }
  return ret;
}

/**
 * 方阵 乘 列向量
 */
template<size_t DIM, typename T>
Vec<DIM, T> operator*(const Matrix<DIM, T> &m, const Vec<DIM, T> &v) {
  Vec<DIM, T> ret;
  for (int i = 0; i < DIM; ++i) {
    ret[i] = m[i] * v;
  }
  return ret;
}

/////////////////////////////////////////////////////////////////////////////////

typedef Vec<2, float> Vec2f;
typedef Vec<2, int> Vec2i;
typedef Vec<3, float> Vec3f;
typedef Vec<3, int> Vec3i;
typedef Vec<4, float> Vec4f;
typedef Vec<4, int> Vec4i;
typedef Matrix<4, float> M4f;
typedef Matrix<4, int> M4i;

/////////////////////////////////////////////////////////////////////////////////

inline M4f GetScaleMatrix(const Vec3f &s) {
  M4f m;
  for (int i = 0; i < 3; ++i) m[i][i] = s[i];
  m[3][3] = 1.f;
  return m;
}

inline M4f GetXRotateMatrix(float rx) {
  const auto c = static_cast<float>(cos(rx * M_PI / 180.));
  const auto s = static_cast<float>(sin(rx * M_PI / 180.));
  M4f m;
  m[0][0] = 1.f;
  m[1][1] = c;
  m[1][2] = -s;
  m[2][1] = s;
  m[2][2] = c;
  m[3][3] = 1.f;
  return m;
}

inline M4f GetYRotateMatrix(float rx) {
  const auto c = static_cast<float>(cos(rx * M_PI / 180.));
  const auto s = static_cast<float>(sin(rx * M_PI / 180.));
  M4f m;
  m[0][0] = c;
  m[0][2] = s;
  m[1][1] = 1.f;
  m[2][0] = -s;
  m[2][2] = c;
  m[3][3] = 1.f;
  return m;
}

inline M4f GetZRotateMatrix(float rx) {
  const auto c = static_cast<float>(cos(rx * M_PI / 180.));
  const auto s = static_cast<float>(sin(rx * M_PI / 180.));
  M4f m;
  m[0][0] = c;
  m[0][1] = -s;
  m[1][0] = s;
  m[1][1] = c;
  m[2][2] = 1.f;
  m[3][3] = 1.f;
  return m;
}

inline M4f GetTranslationMatrix(const Vec3f &t) {
  M4f m;
  for (int i = 0; i < 4; ++i) m[i][i] = 1.f;
  for (int i = 0; i < 3; ++i) m[i][3] = t[i];
  return m;
}

#endif //TINY_RENDER_MY_MATH_H
