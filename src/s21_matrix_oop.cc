#include "s21_matrix_oop.h"

// Конструкторы и деструктор
S21Matrix::S21Matrix() : rows_(1), cols_(1), matrix_(new double[1]()) {
  // std::cout << "Дефолтный конструктор\n";
}

S21Matrix::S21Matrix(int rows, int cols) : rows_(rows), cols_(cols) {
  // std::cout << "2 конструктор ";
  if (rows < 1 || cols < 1) {
    throw std::invalid_argument(
        "Invalid matrix: Rows and columns must be non-negative.");
  }
  matrix_ = new double[rows_ * cols_]();
  // std::cout << "rows = " << rows_ << ", cols = " << cols_
  //           << ", matrix = " << matrix_ << "\n";
}

S21Matrix::S21Matrix(std::initializer_list<double> init) {
  // std::cout << "initializer_list конструктор\n";
  if (init.size() < 2) {
    throw std::invalid_argument(
        "Invalid matrix: Initialization list must contain at least two "
        "elements.");
  }

  auto it = init.begin();
  rows_ = *it++;
  cols_ = *it++;
  if (rows_ < 1 || cols_ < 1) {
    throw std::invalid_argument(
        "Invalid matrix: Rows and columns must be non-negative.");
  }

  matrix_ = new double[rows_ * cols_]();

  for (size_t i = 0; i < rows_; i++) {
    for (size_t j = 0; j < cols_; j++) {
      if (it != init.end()) {
        (*this)(i, j) = *it++;
      }
    }
  }
}

S21Matrix::S21Matrix(const S21Matrix& other)
    : rows_(other.rows_),
      cols_(other.cols_),
      matrix_(new double[rows_ * cols_]()) {
  // std::cout << "Конструктор копирования\n";
  std::copy(other.matrix_, other.matrix_ + rows_ * cols_, matrix_);
}

S21Matrix::S21Matrix(S21Matrix&& other) noexcept
    : rows_(other.rows_), cols_(other.cols_), matrix_(other.matrix_) {
  // std::cout << "Конструктор перемещения\n";
  other.rows_ = other.cols_ = 0;
  other.matrix_ = nullptr;
}

S21Matrix::~S21Matrix() {
  // std::cout << "Деструктор\n";
  rows_ = cols_ = 0;
  Clear();
}

//  Геттеры и сеттеры

double S21Matrix::GetRows() const { return rows_; }

void S21Matrix::SetRows(int row) {
  if (row < 1) {
    throw std::invalid_argument(
        "Invalid Matrix: the number of rows should be a natural number");
  }

  double* tmp = new double[row * cols_]();

  for (size_t i = 0; i < row; i++) {
    for (size_t j = 0; j < cols_; j++) {
      if (i < rows_) {
        tmp[i * cols_ + j] = (*this)(i, j);
      }
    }
  }
  Clear();
  rows_ = row;
  matrix_ = tmp;
  // tmp = nullptr;
}

double S21Matrix::GetCols() const { return cols_; }

void S21Matrix::SetCols(int column) {
  if (column < 1) {
    throw std::invalid_argument(
        "Invalid Matrix: the number of columns should be a natural number");
  }

  double* tmp = new double[rows_ * column]();

  for (size_t i = 0; i < rows_; i++) {
    for (size_t j = 0; j < column; j++) {
      if (j < cols_) {
        tmp[i * column + j] = (*this)(i, j);
      }
    }
  }
  Clear();
  cols_ = column;
  matrix_ = tmp;
}

// Операции
void S21Matrix::PrintMatrix() const {
  for (size_t i = 0; i < rows_; i++) {
    for (size_t j = 0; j < cols_; j++) {
      std::cout << std::fixed << (*this)(i, j) << " ";
    }
    std::cout << "\n";
  }
  std::cout << "____________________________\n";
}

bool S21Matrix::EqMatrix(const S21Matrix& other) const noexcept {
  if (rows_ != other.rows_ || cols_ != other.cols_) {
    return false;
  }
  bool res = true;
  for (size_t i = 0; i < rows_; i++) {
    for (size_t j = 0; j < cols_; j++) {
      if (fabs((*this)(i, j) - other(i, j)) >= EPS) {
        res = false;
      };
    }
  }
  return res;
}

void S21Matrix::SumMatrix(const S21Matrix& other) {
  if (rows_ != other.rows_ || cols_ != other.cols_) {
    throw std::invalid_argument("Not equal sizes");
  }

  for (size_t i = 0; i < rows_; i++) {
    for (size_t j = 0; j < cols_; j++) {
      (*this)(i, j) += other(i, j);
    }
  }
}

void S21Matrix::SubMatrix(const S21Matrix& other) {
  if (rows_ != other.rows_ || cols_ != other.cols_) {
    throw std::invalid_argument("Not equal sizes");
  }

  for (size_t i = 0; i < rows_; i++) {
    for (size_t j = 0; j < cols_; j++) {
      (*this)(i, j) -= other(i, j);
    }
  }
}

void S21Matrix::MulNumber(const double num) {
  for (size_t i = 0; i < rows_; i++) {
    for (size_t j = 0; j < cols_; j++) {
      (*this)(i, j) *= num;
    }
  }
}

void S21Matrix::MulMatrix(const S21Matrix& other) {
  if (cols_ != other.rows_) {
    throw std::invalid_argument("Invalid Matrix: not valid sizes");
  }

  S21Matrix tmp(rows_, other.cols_);
  for (size_t i = 0; i < tmp.rows_; i++) {
    for (size_t j = 0; j < tmp.cols_; j++) {
      for (size_t k = 0; k < cols_; k++) {
        tmp(i, j) += (*this)(i, k) * other(k, j);
      }
    }
  }
  *this = std::move(tmp);
}

S21Matrix S21Matrix::Transpose() {
  S21Matrix tmp(cols_, rows_);
  for (size_t i = 0; i < cols_; i++) {
    for (size_t j = 0; j < rows_; j++) {
      tmp(i, j) = (*this)(j, i);
    }
  }
  std::swap(rows_, cols_);
  *this = std::move(tmp);
  return *this;
}

S21Matrix S21Matrix::CalcComplements() {
  double res = 0;
  if (rows_ != cols_) {
    throw std::invalid_argument("Invalid Matrix: The matrix should be square");
  }

  S21Matrix tmp(rows_, cols_);
  for (size_t i = 0; i < tmp.rows_; i++) {
    for (size_t j = 0; j < tmp.cols_; j++) {
      tmp(i, j) = Complement(i, j);
    }
  }
  *this = std::move(tmp);
  return *this;
}

double S21Matrix::Determinant() const {
  double res = 0;
  if (rows_ != cols_) {
    throw std::invalid_argument("Invalid Matrix: The matrix should be square");
  }

  if (rows_ == 1) {
    res = (*this)(0, 0);
  } else if (rows_ == 2) {
    res = SimpleDet();
  } else {
    for (int i = 0; i < rows_; i++) {
      res += (*this)(i, 0) * Complement(i, 0);
    }
  }

  return res;
}

S21Matrix S21Matrix::InverseMatrix() {
  double res = 0;
  if (rows_ != cols_) {
    throw std::invalid_argument("Invalid Matrix: The matrix should be square");
  }
  double det = Determinant();
  if (det == 0) {
    throw std::invalid_argument("Invalid Matrix: Determinant of matrix is 0");
  }

  if (rows_ == 1) {
    (*this)(0, 0) = 1 / (*this)(0, 0);
  } else {
    CalcComplements();
    Transpose();
    MulNumber(1 / det);
  }

  return *this;
}

// Перегрузки операторов
double& S21Matrix::operator()(int i, int j) {
  if (i < 0 || j < 0 || i > GetRows() || j > GetCols()) {
    throw std::out_of_range("Out of range");
  }
  return matrix_[i * cols_ + j];
}

double S21Matrix::operator()(int i, int j) const {
  if (i < 0 || j < 0 || i > GetRows() || j > GetCols()) {
    throw std::out_of_range("Out of range");
  }
  return matrix_[i * cols_ + j];
}

S21Matrix S21Matrix::operator+(const S21Matrix& other) const {
  S21Matrix res(*this);
  res.SumMatrix(other);
  return res;
}

S21Matrix S21Matrix::operator-(const S21Matrix& other) const {
  S21Matrix res(*this);
  res.SubMatrix(other);
  return res;
}

S21Matrix S21Matrix::operator*(const S21Matrix& other) const {
  S21Matrix res(*this);
  res.MulMatrix(other);
  return res;
}

S21Matrix S21Matrix::operator*(const double num) const {
  S21Matrix res(*this);
  res.MulNumber(num);
  return res;
}

S21Matrix operator*(double num, const S21Matrix& other) {
  S21Matrix res(other);
  res.MulNumber(num);
  return res;
}

bool S21Matrix::operator==(const S21Matrix& other) const noexcept {
  return (*this).EqMatrix(other);
}

S21Matrix& S21Matrix::operator=(const S21Matrix& other) {
  if (this != &other) {
    S21Matrix res(other);
    rows_ = other.GetRows();
    cols_ = other.GetCols();
    Clear();
    matrix_ = new double[rows_ * cols_]();
    for (size_t i = 0; i < rows_; i++) {
      for (size_t j = 0; j < cols_; j++) {
        (*this)(i, j) = other(i, j);
      }
    }
  }
  return *this;
}

S21Matrix& S21Matrix::operator=(S21Matrix&& other) {
  if (this != &other) {
    rows_ = cols_ = 0;
    Clear();
    std::swap(this->rows_, other.rows_);
    std::swap(this->cols_, other.cols_);
    std::swap(this->matrix_, other.matrix_);
  }
  return *this;
}

S21Matrix& S21Matrix::operator+=(const S21Matrix& other) {
  SumMatrix(other);
  return *this;
}

S21Matrix& S21Matrix::operator-=(const S21Matrix& other) {
  SubMatrix(other);
  return *this;
}

S21Matrix& S21Matrix::operator*=(const S21Matrix& other) {
  MulMatrix(other);
  return *this;
}

S21Matrix& S21Matrix::operator*=(const double num) {
  MulNumber(num);
  return *this;
}

// Вспомогательные приватные методы

void S21Matrix::Clear() {
  if (matrix_) {
    delete[] matrix_;
  }
  matrix_ = nullptr;
}

double S21Matrix::SimpleDet() const {
  return (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
}

double S21Matrix::Minor(int n, int m) const {
  double res = 0;

  S21Matrix tmp(rows_ - 1, cols_ - 1);
  for (int i = 0; i < tmp.rows_; i++) {
    for (int j = 0; j < tmp.cols_; j++) {
      int x = 0, y = 0;
      if (i >= n) x = 1;
      if (j >= m) y = 1;
      tmp(i, j) = (*this)(i + x, j + y);
    }
  }

  res = tmp.Determinant();
  return res;
}

double S21Matrix::Complement(int n, int m) const {
  return pow(-1, (n + m)) * Minor(n, m);
}