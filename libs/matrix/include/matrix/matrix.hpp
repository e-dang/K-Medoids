#pragma once

#include <iterator>
#include <type_traits>
#include <vector>

template <typename T>
class Matrix
{
public:
    template <bool Const = false>
    class ForwardIterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = std::remove_cv_t<T>;
        using difference_type   = std::ptrdiff_t;
        using pointer           = typename std::conditional_t<Const, const T*, T*>;
        using reference         = typename std::conditional_t<Const, const T&, T&>;

        ForwardIterator() : p_iter(nullptr) {}

        ForwardIterator(pointer ptr) : p_iter(ptr) {}

        ForwardIterator operator++()
        {
            ForwardIterator temp = *this;
            p_iter++;
            return temp;
        }

        ForwardIterator& operator++(int)
        {
            ++p_iter;
            return *this;
        }

        bool operator==(const ForwardIterator& rhs) const { return p_iter == rhs.p_iter; }

        bool operator!=(const ForwardIterator& rhs) const { return p_iter != rhs.p_iter; }

        template <bool _Const = Const>
        std::enable_if_t<_Const, reference> operator*() const
        {
            assert(p_iter != nullptr);
            return *p_iter;
        }

        template <bool _Const = Const>
        std::enable_if_t<!_Const, reference> operator*()
        {
            assert(p_iter != nullptr);
            return *p_iter;
        }

    private:
        pointer p_iter;
    };

    typedef T value_type;
    typedef ForwardIterator<false> iterator;
    typedef ForwardIterator<true> const_iterator;

    Matrix();

    Matrix(const int64_t rows, const int64_t cols, const bool autoReserve = false, const T fillVal = 0.0);

    Matrix(const Matrix& other);

    Matrix(Matrix&& other);

    virtual ~Matrix();

    Matrix& operator=(const Matrix& rhs);

    Matrix& operator=(Matrix&& rhs);

    bool operator==(const Matrix& rhs) const;

    bool operator!=(const Matrix& rhs) const;

    template <typename Container>
    void append(const Container& datapoint)
    {
        checkAtCapacity();

        std::copy(datapoint.cbegin(), datapoint.cend(), p_data + m_size);
        ++m_numRows;
        m_size += m_cols;
    }

    template <typename Container>
    void append(const Container&& datapoint)
    {
        checkAtCapacity();

        std::copy(datapoint.cbegin(), datapoint.cend(), p_data + m_size);
        ++m_numRows;
        m_size += m_cols;
    }

    template <typename Iter>
    void append(Iter begin, Iter end)
    {
        checkAtCapacity();

        std::copy(begin, end, p_data + m_size);
        ++m_numRows;
        m_size += m_cols;
    }

    template <typename Container>
    void add(const int32_t row, const Container& datapoint)
    {
        std::copy(datapoint.cbegin(), datapoint.cend(), rowBegin(row));
    }

    template <typename Container>
    void add(const int32_t row, const Container&& datapoint)
    {
        std::copy(datapoint.cbegin(), datapoint.cend(), rowBegin(row));
    }

    template <typename Iter>
    void add(const int32_t row, Iter begin, Iter end)
    {
        std::copy(begin, end, rowBegin(row));
    }

    void resize(const int64_t elements);

    void fill(const T val);

    void clear();

    bool empty() const noexcept;

    T* at(const int64_t row);

    T& at(const int64_t row, const int64_t col);

    const T* at(const int64_t row) const;

    const T& at(const int64_t row, const int64_t col) const;

    iterator begin();

    const_iterator begin() const;

    const_iterator cbegin() const;

    iterator rowBegin(const int64_t row);

    const_iterator crowBegin(const int64_t row) const;

    iterator end();

    const_iterator end() const;

    const_iterator cend() const;

    iterator rowEnd(const int64_t row);

    const_iterator crowEnd(const int64_t row) const;

    std::vector<T> extractRow(const int64_t row) const;

    T* data();

    const T* data() const;

    int64_t numRows() const noexcept;

    int64_t rows() const noexcept;

    int64_t cols() const noexcept;

    int64_t size() const noexcept;

    int64_t capacity() const noexcept;

    int64_t bytes() const noexcept;

    char* serialize() const noexcept;

protected:
    void checkAtCapacity();

private:
    void validateDimensions() const;

    void validateRows() const;

    void validateCols() const;

    void allocate(const bool autoSize, const T fillVal);

protected:
    int64_t m_rows;
    int64_t m_cols;
    int64_t m_capacity;
    int64_t m_numRows;
    int64_t m_size;
    T* p_data;
};
