#include <exception>
#include <matrix.hpp>
#include <string>
#ifndef __clang__
    #include <algorithm>
#endif

template <typename T>
Matrix<T>::Matrix() : m_rows(0), m_cols(0), m_capacity(0), m_numRows(0), m_size(0), p_data(nullptr)
{
}

template <typename T>
Matrix<T>::Matrix(const int64_t rows, const int64_t cols, const bool autoResize, const T fillVal) :
    m_rows(rows), m_cols(cols), m_capacity(rows * cols), m_numRows(0), m_size(0), p_data(nullptr)
{
    validateDimensions();
    allocate(autoResize, fillVal);
}

template <typename T>
Matrix<T>::Matrix(const Matrix<T>& other) :
    m_rows(other.m_rows),
    m_cols(other.m_cols),
    m_capacity(other.m_capacity),
    m_numRows(other.m_numRows),
    m_size(other.m_size),
    p_data(new T[m_capacity])
{
    std::copy(other.cbegin(), other.cend(), begin());
}

template <typename T>
Matrix<T>::Matrix(Matrix<T>&& other) : Matrix(0, 0, false)
{
    *this = std::move(other);
}

template <typename T>
Matrix<T>::~Matrix()
{
    if (p_data != nullptr)
    {
        delete[] p_data;
        p_data = nullptr;
    }
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& rhs)
{
    if (this != &rhs)
    {
        if (p_data != nullptr)
            delete[] p_data;

        m_rows     = rhs.m_rows;
        m_cols     = rhs.m_cols;
        m_capacity = rhs.m_capacity;
        m_numRows  = rhs.m_numRows;
        m_size     = rhs.m_size;
        p_data     = new T[m_capacity];
        std::copy(rhs.cbegin(), rhs.cend(), begin());
    }

    return *this;
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(Matrix<T>&& rhs)
{
    if (this != &rhs)
    {
        if (p_data != nullptr)
            delete[] p_data;

        m_rows     = rhs.m_rows;
        m_cols     = rhs.m_cols;
        m_capacity = rhs.m_capacity;
        m_numRows  = rhs.m_numRows;
        m_size     = rhs.m_size;
        p_data     = rhs.p_data;

        rhs.m_rows     = 0;
        rhs.m_cols     = 0;
        rhs.m_capacity = 0;
        rhs.m_numRows  = 0;
        rhs.m_size     = 0;
        rhs.p_data     = nullptr;
    }

    return *this;
}

template <typename T>
bool Matrix<T>::operator==(const Matrix<T>& rhs) const
{
    if (m_rows != rhs.m_rows)
        return false;

    if (m_cols != rhs.m_cols)
        return false;

    if (m_capacity != rhs.m_capacity)
        return false;

    if (m_numRows != rhs.m_numRows)
        return false;

    if (m_size != rhs.m_size)
        return false;

    for (int64_t i = 0; i < size(); ++i)
    {
        if (p_data[i] != rhs.p_data[i])
            return false;
    }

    return true;
}

template <typename T>
bool Matrix<T>::operator!=(const Matrix<T>& rhs) const
{
    return !(*this == rhs);
}

template <typename T>
void Matrix<T>::resize(const int64_t rows)
{
    if (rows < 0 || rows > m_rows)
        throw std::length_error("Cannot resize to size less than zero or greater than the capacity.");

    m_numRows = rows;
    m_size    = rows * m_cols;
}

template <typename T>
void Matrix<T>::fill(const T val)
{
    std::fill(begin(), end(), val);
}

template <typename T>
void Matrix<T>::clear()
{
    m_size    = 0;
    m_numRows = 0;
}

template <typename T>
bool Matrix<T>::empty() const noexcept
{
    return !static_cast<bool>(m_size);
}

template <typename T>
T* Matrix<T>::at(const int64_t row)
{
    return const_cast<T*>(static_cast<const Matrix<T>&>(*this).at(row));
}

template <typename T>
T& Matrix<T>::at(const int64_t row, const int64_t col)
{
    return const_cast<T&>(static_cast<const Matrix<T>&>(*this).at(row, col));
}

#ifdef DEBUG

template <typename T>
const T* Matrix<T>::at(const int64_t row) const
{
    if (row >= m_rows || row < 0)
        throw std::out_of_range("Row index " + std::to_string(row) + " out of range [0, " + std::to_string(m_rows - 1) +
                                "].");

    return p_data + (row * m_cols);
}

template <typename T>
const T& Matrix<T>::at(const int64_t row, const int64_t col) const
{
    if (col >= m_cols || col < 0)
        throw std::out_of_range("Column index " + std::to_string(col) + " out of range [0, " +
                                std::to_string(m_cols - 1) + "].");

    return *(at(row) + col);
}

#else

template <typename T>
const T* Matrix<T>::at(const int64_t row) const
{
    return p_data + (row * m_cols);
}

template <typename T>
const T& Matrix<T>::at(const int64_t row, const int64_t col) const
{
    return *(at(row) + col);
}

#endif

template <typename T>
std::pair<int32_t, int32_t> Matrix<T>::find(const T element) const
{
    auto iter     = std::find(cbegin(), cend(), element);
    auto distance = std::distance(cbegin(), iter);
    return std::make_pair<int32_t, int32_t>(distance / m_cols, distance % m_cols);  // row, col
}

template <typename T>
typename Matrix<T>::row_iterator Matrix<T>::begin()
{
    return Matrix<T>::row_iterator(p_data);
}

template <typename T>
typename Matrix<T>::const_row_iterator Matrix<T>::begin() const
{
    return Matrix<T>::const_row_iterator(p_data);
}

template <typename T>
typename Matrix<T>::const_row_iterator Matrix<T>::cbegin() const
{
    return Matrix<T>::const_row_iterator(p_data);
}

template <typename T>
typename Matrix<T>::col_iterator Matrix<T>::colBegin(const int64_t col)
{
    return Matrix<T>::col_iterator(p_data + col, m_cols);
}

template <typename T>
typename Matrix<T>::row_iterator Matrix<T>::rowBegin(const int64_t row)
{
    return Matrix<T>::row_iterator(at(row));
}

template <typename T>
typename Matrix<T>::const_row_iterator Matrix<T>::crowBegin(const int64_t row) const
{
    return Matrix<T>::const_row_iterator(at(row));
}

template <typename T>
typename Matrix<T>::const_col_iterator Matrix<T>::ccolBegin(const int64_t col) const
{
    return Matrix<T>::const_col_iterator(p_data + col, m_cols);
}

template <typename T>
typename Matrix<T>::row_iterator Matrix<T>::end()
{
    return Matrix<T>::row_iterator(p_data + capacity());
}

template <typename T>
typename Matrix<T>::const_row_iterator Matrix<T>::end() const
{
    return Matrix<T>::const_row_iterator(p_data + capacity());
}

template <typename T>
typename Matrix<T>::const_row_iterator Matrix<T>::cend() const
{
    return Matrix<T>::const_row_iterator(p_data + capacity());
}

template <typename T>
typename Matrix<T>::row_iterator Matrix<T>::rowEnd(const int64_t row)
{
    return Matrix<T>::row_iterator(at(row) + m_cols);
}

template <typename T>
typename Matrix<T>::col_iterator Matrix<T>::colEnd(const int64_t col)
{
    return Matrix<T>::col_iterator(p_data + col + (m_numRows * m_cols), m_cols);
}

template <typename T>
typename Matrix<T>::const_row_iterator Matrix<T>::crowEnd(const int64_t row) const
{
    return Matrix<T>::const_row_iterator(at(row) + m_cols);
}

template <typename T>
typename Matrix<T>::const_col_iterator Matrix<T>::ccolEnd(const int64_t col) const
{
    return Matrix<T>::const_col_iterator(p_data + col + (m_numRows * m_cols), m_cols);
}

template <typename T>
std::vector<T> Matrix<T>::extractRow(const int64_t row) const
{
    return std::vector<T>(crowBegin(row), crowEnd(row));
}

template <typename T>
T* Matrix<T>::data()
{
    return p_data;
}

template <typename T>
const T* Matrix<T>::data() const
{
    return p_data;
}

template <typename T>
int64_t Matrix<T>::numRows() const noexcept
{
    return m_numRows;
}

template <typename T>
int64_t Matrix<T>::rows() const noexcept
{
    return m_rows;
}

template <typename T>
int64_t Matrix<T>::cols() const noexcept
{
    return m_cols;
}

template <typename T>
int64_t Matrix<T>::size() const noexcept
{
    return m_size;
}

template <typename T>
int64_t Matrix<T>::capacity() const noexcept
{
    return m_capacity;
}

template <typename T>
int64_t Matrix<T>::bytes() const noexcept
{
    return m_capacity * static_cast<int64_t>(sizeof(T));
}

template <typename T>
char* Matrix<T>::serialize() const noexcept
{
    return reinterpret_cast<char*>(p_data);
}

template <typename T>
void Matrix<T>::validateDimensions() const
{
    validateRows();
    validateCols();
}

template <typename T>
void Matrix<T>::validateRows() const
{
    if (m_rows < 0)
        throw std::length_error("The number of rows in a matrix must be positive. Provided number of rows: " +
                                std::to_string(m_rows));
}

template <typename T>
void Matrix<T>::validateCols() const
{
    if (m_cols < 0)
        throw std::length_error("The number of cols in a matrix must be positive. Provided number of cols: " +
                                std::to_string(m_cols));
}

template <typename T>
void Matrix<T>::allocate(const bool autoresize, const T fillVal)
{
    p_data = new T[m_capacity];
    if (autoresize)
    {
        resize(m_rows);
        fill(fillVal);
    }
}

template <typename T>
void Matrix<T>::checkAtCapacity()
{
    if (m_size >= m_capacity)
        throw std::length_error("Cannot add data to full Matrix.");
}

template class Matrix<float>;
template class Matrix<double>;