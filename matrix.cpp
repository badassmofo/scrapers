#include <iostream>
#include <string>
#include <array>

#define _MATRIX matrix_t<T, R, C>

template<typename T, int R, int C> class matrix_t {
  std::array<std::array<T, C>, R> _matrix;
  T _rows  = R,
  _columns = C;
  
public:
  matrix_t(const int i = 0) {
    for (std::array<T, C>& a: _matrix)
      a.fill(i);
  }
  
  template <typename A, typename... B> matrix_t(A _a, B... _b) {
    int _r = 0,
    _c = 0;
    for (const A& i: { _a, _b... }) {
      _matrix[_r][_c] = i;
      _c += 1;
      if (_c == _columns) {
        _c = 0;
        _r += 1;
      }
    }
  }
  
  auto rows() const {
    return _rows;
  }
  
  auto columns() const {
    return _columns;
  }
  
  auto size() const {
    return _rows * _columns;
  }
  
  auto get(const int& x, const int& y) const {
    return _matrix[x][y];
  }
  
  auto set(const int& x, const int& y, const T& n) {
    _matrix[x][y] = n;
  }
  
  auto to_string() {
    std::string out = "[";
    for (const auto& a: _matrix)
      for (const auto& v: a)
        out += std::to_string(v) + ", ";
    return out.substr(0, out.length() - 2) + "]";
  }
  
  friend std::ostream& operator <<(std::ostream& os, const _MATRIX& m) {
    std::array<int, R> lengths;
    lengths.fill(0);
    
    for (int i = 0; i < R; ++i) {
      for (int j = 0; j < C; ++j) {
        int t = std::to_string(m._matrix[j][i]).length();
        if (t > lengths[i])
          lengths[i] = t;
      }
    }
    
    for (int i = 0; i < R; ++i) {
      os << "[";
      for (int j = 0; j < C; ++j) {
        int buf_len = lengths[j] - std::to_string(m._matrix[i][j]).length() + 1;
        os << m._matrix[i][j];
        if (j + 1 != C)
          os << "," << std::string(buf_len, ' ');
        else
          if (buf_len - 1 > 0)
            os << std::string(buf_len - 1, ' ');
      }
      os << "]";
      if (i + 1 != C)
        os << std::endl;
    }
    return os;
  }
  
  friend _MATRIX operator +(const _MATRIX& m1, const _MATRIX& m2) {
    _MATRIX ret(0);
    for (int i = 0; i < R; ++i)
      for (int j = 0; j < C; ++j)
        ret._matrix[i][j] = m1._matrix[i][j] + m2._matrix[i][j];
    return ret;
  }
  
  friend _MATRIX operator -(const _MATRIX& m1, const _MATRIX& m2) {
    _MATRIX ret(0);
    for (int i = 0; i < R; ++i)
      for (int j = 0; j < C; ++j)
        ret._matrix[i][j] = m1._matrix[i][j] - m2._matrix[i][j];
    return ret;
  }
  
  _MATRIX operator +=(const _MATRIX& m) {
    return *this + m;
  }
  
  _MATRIX operator -=(const _MATRIX& m) {
    return *this - m;
  }
  
  std::array<T, C>& operator [](const int& i) {
    return this->_matrix[i];
  }
};

template<typename T> class matrix_2x2_t: public matrix_t<T, 2, 2> {
public:
};

typedef matrix_2x2_t<signed int>		matrix_2x2i;
typedef matrix_2x2_t<unsigned int>	matrix_2x2ui;
typedef matrix_2x2_t<float>					matrix_2x2f;
typedef matrix_2x2_t<double>				matrix_2x2d;


template<typename T> class matrix_3x3_t: public matrix_t<T, 3, 3> {
public:
};

typedef matrix_3x3_t<signed int>    matrix_3x3i;
typedef matrix_3x3_t<unsigned int>  matrix_3x3ui;
typedef matrix_3x3_t<float>         matrix_3x3f;
typedef matrix_3x3_t<double>        matrix_3x3d;


template<typename T> class matrix_4x4_t: public matrix_t<T, 4, 4> {
public:
};

typedef matrix_4x4_t<signed int>    matrix_4x4i;
typedef matrix_4x4_t<unsigned int>  matrix_4x4ui;
typedef matrix_4x4_t<float>         matrix_4x4f;
typedef matrix_4x4_t<double>        matrix_4x4d;

typedef matrix_t<int, 2, 2> matrix2x2i;
typedef matrix_t<int, 3, 3> matrix3x3i;

auto main() -> int {
  matrix3x3i m1 = {0,  12,  2,
                   3,  444, 52,
                   63, 7,   8};
  std::cout << "m1" << std::endl;
  std::cout << m1.rows() << "x" << m1.columns() << " (" << m1.size() << ")" << std::endl;
  std::cout << m1.to_string() << std::endl;
  std::cout << m1 << std::endl << std::endl;
  
  matrix3x3i m2 = {-6, 4,  8,
                    3, 0,  1,
                    2, 2, -4};
  std::cout << "m2" << std::endl;
  std::cout << m2.rows() << "x" << m1.columns() << " (" << m2.size() << ")" << std::endl;
  std::cout << m2.to_string() << std::endl;
  std::cout << m2 << std::endl << std::endl;
  
  std::cout << m1.to_string() << " - " << m2.to_string() << " = " << std::endl;
  matrix3x3i m3 = m1 - m2;
  std::cout << m3 << std::endl << std::endl;
  
  m1 += m2;
  std::cout << "m1 (m1 += m2)" << std::endl;
  std::cout << m1 << std::endl << std::endl;
  
  m1 -= m2;
  std::cout << "m1 (m1 -= m2)" << std::endl;
  std::cout << m1 << std::endl << std::endl;
  
  // matrix3x3i m4 = m1 * m2;
  // std::cout << "m4 (m1 * m2)" << std::endl;
  // std::cout << m1 << std::endl;
}
