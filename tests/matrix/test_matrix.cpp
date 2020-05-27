#include <matrix/matrix.hpp>
#define BOOST_TEST_MODULE test_matrix
#include <algorithm>
#include <boost/mpl/list.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/unit_test.hpp>
#include <numeric>

namespace utf = boost::unit_test;
namespace tt  = boost::test_tools;

struct SmallMatrix
{
    SmallMatrix() : rows(10), cols(50), autoResize(true), fillVal(7) {}
    ~SmallMatrix() {}

    int64_t rows;
    int64_t cols;
    bool autoResize;
    double fillVal;
};

struct LargeMatrix  // will cause overflow for int32_t
{
    LargeMatrix() : rows(7500), cols(1286792) {}
    ~LargeMatrix() {}

    int64_t rows;
    int64_t cols;
};

typedef boost::mpl::list<double, float> test_types;

BOOST_AUTO_TEST_SUITE(matrix_constructors)

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_2_param_constructor, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols);
    BOOST_TEST(matrix.size() == 0);
    BOOST_TEST(matrix.capacity() == rows * cols);
    BOOST_TEST(matrix.bytes() == rows * cols * sizeof(T));
    BOOST_TEST(matrix.rows() == rows);
    BOOST_TEST(matrix.cols() == cols);
    BOOST_TEST(matrix.numRows() == 0);
    BOOST_TEST(matrix.data() != nullptr);
    BOOST_TEST(matrix.serialize() != nullptr);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_3_param_constructor, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols, autoResize);
    BOOST_TEST(matrix.size() == rows * cols);
    BOOST_TEST(matrix.capacity() == rows * cols);
    BOOST_TEST(matrix.bytes() == rows * cols * sizeof(T));
    BOOST_TEST(matrix.rows() == rows);
    BOOST_TEST(matrix.cols() == cols);
    BOOST_TEST(matrix.numRows() == rows);
    BOOST_TEST(*matrix.data() == static_cast<T>(0));
    BOOST_TEST(*matrix.serialize() == static_cast<char>(0));
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_4_param_constructor, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols, autoResize, fillVal);
    BOOST_TEST(matrix.size() == rows * cols);
    BOOST_TEST(matrix.capacity() == rows * cols);
    BOOST_TEST(matrix.bytes() == rows * cols * sizeof(T));
    BOOST_TEST(matrix.rows() == rows);
    BOOST_TEST(matrix.cols() == cols);
    BOOST_TEST(matrix.numRows() == rows);
    BOOST_TEST(
      std::all_of(matrix.begin(), matrix.end(), [this](const T val) { return val == static_cast<T>(fillVal); }));
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_invalid_constructor, T, test_types, SmallMatrix)
{
    BOOST_CHECK_THROW(Matrix<T> matrix(-1, cols, autoResize, fillVal), std::length_error);
    BOOST_CHECK_THROW(Matrix<T> matrix(rows, -1, autoResize, fillVal), std::length_error);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_copy_constructor, T, test_types, SmallMatrix)
{
    Matrix<T> matrix1(rows, cols, autoResize, fillVal);
    Matrix<T> matrix2(matrix1);
    BOOST_TEST(matrix1.size() == matrix2.size());
    BOOST_TEST(matrix1.capacity() == matrix2.capacity());
    BOOST_TEST(matrix1.bytes() == matrix2.bytes());
    BOOST_TEST(matrix1.rows() == matrix2.rows());
    BOOST_TEST(matrix1.cols() == matrix2.cols());
    BOOST_TEST(matrix1.numRows() == matrix2.numRows());
    BOOST_CHECK_EQUAL_COLLECTIONS(matrix1.begin(), matrix1.end(), matrix2.begin(), matrix2.end());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_move_constructor, T, test_types, SmallMatrix)
{
    Matrix<T> matrix1(rows, cols, autoResize, fillVal);
    Matrix<T> matrix2(std::move(matrix1));
    BOOST_TEST(matrix1.size() == 0);
    BOOST_TEST(matrix1.capacity() == 0);
    BOOST_TEST(matrix1.bytes() == 0);
    BOOST_TEST(matrix1.rows() == 0);
    BOOST_TEST(matrix1.cols() == 0);
    BOOST_TEST(matrix1.numRows() == 0);
    BOOST_TEST(matrix1.data() == nullptr);
    BOOST_TEST(matrix1.serialize() == nullptr);

    BOOST_TEST(matrix2.size() == rows * cols);
    BOOST_TEST(matrix2.capacity() == rows * cols);
    BOOST_TEST(matrix2.bytes() == rows * cols * sizeof(T));
    BOOST_TEST(matrix2.rows() == rows);
    BOOST_TEST(matrix2.cols() == cols);
    BOOST_TEST(matrix2.numRows() == rows);
    BOOST_TEST(
      std::all_of(matrix2.begin(), matrix2.end(), [this](const T val) { return val == static_cast<T>(fillVal); }));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(matrix_operators)

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_copy_assignment, T, test_types, SmallMatrix)
{
    Matrix<T> matrix1(rows, cols, autoResize, fillVal);
    Matrix<T> matrix2(0, 0);
    matrix2 = matrix1;
    BOOST_TEST(matrix1.size() == matrix2.size());
    BOOST_TEST(matrix1.capacity() == matrix2.capacity());
    BOOST_TEST(matrix1.bytes() == matrix2.bytes());
    BOOST_TEST(matrix1.rows() == matrix2.rows());
    BOOST_TEST(matrix1.cols() == matrix2.cols());
    BOOST_TEST(matrix1.numRows() == matrix2.numRows());
    BOOST_CHECK_EQUAL_COLLECTIONS(matrix1.begin(), matrix1.end(), matrix2.begin(), matrix2.end());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_move_assignment, T, test_types, SmallMatrix)
{
    Matrix<T> matrix2(0, 0);
    matrix2 = Matrix<T>(rows, cols, autoResize, fillVal);
    BOOST_TEST(matrix2.size() == rows * cols);
    BOOST_TEST(matrix2.capacity() == rows * cols);
    BOOST_TEST(matrix2.bytes() == rows * cols * sizeof(T));
    BOOST_TEST(matrix2.rows() == rows);
    BOOST_TEST(matrix2.cols() == cols);
    BOOST_TEST(matrix2.numRows() == rows);
    BOOST_TEST(
      std::all_of(matrix2.begin(), matrix2.end(), [this](const T val) { return val == static_cast<T>(fillVal); }));
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_equality, T, test_types, SmallMatrix)
{
    Matrix<T> matrix1(rows, cols, autoResize, fillVal);
    Matrix<T> matrix2(rows, cols, autoResize, fillVal);
    BOOST_CHECK(matrix1 == matrix2);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_inequality, T, test_types, SmallMatrix)
{
    Matrix<T> matrix1(rows, cols, autoResize, fillVal);
    Matrix<T> matrix2(rows, cols, autoResize);
    BOOST_CHECK(matrix1 != matrix2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(matrix_functions)

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_container_append, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols);
    std::vector<T> vec(cols);
    std::iota(vec.begin(), vec.end(), 0);
    matrix.append(vec);
    BOOST_TEST(matrix.numRows() == 1);
    BOOST_TEST(matrix.size() == cols);
    for (int i = 0; i < cols; ++i)
    {
        BOOST_TEST(vec[i] == matrix.at(0, i));
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_container_r_val_append, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols);
    std::vector<T> vec(cols);
    std::iota(vec.begin(), vec.end(), 0);
    matrix.append(std::vector<T>(vec.begin(), vec.end()));
    BOOST_TEST(matrix.numRows() == 1);
    BOOST_TEST(matrix.size() == cols);
    for (int i = 0; i < cols; ++i)
    {
        BOOST_TEST(vec[i] == matrix.at(0, i));
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_container_append_fail, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(1, cols);
    std::vector<T> vec(cols);
    std::iota(vec.begin(), vec.end(), 0);
    matrix.append(vec);
    BOOST_CHECK_THROW(matrix.append(vec), std::length_error);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_iter_append, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols);
    std::vector<T> vec(cols);
    std::iota(vec.begin(), vec.end(), 0);
    matrix.append(vec.begin(), vec.end());
    BOOST_TEST(matrix.numRows() == 1);
    BOOST_TEST(matrix.size() == cols);
    for (int i = 0; i < cols; ++i)
    {
        BOOST_TEST(vec[i] == matrix.at(0, i));
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_iter_append_fail, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(1, cols);
    std::vector<T> vec(cols);
    std::iota(vec.begin(), vec.end(), 0);
    matrix.append(vec.begin(), vec.end());
    BOOST_CHECK_THROW(matrix.append(vec.begin(), vec.end()), std::length_error);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_iter_set, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols, autoResize, fillVal);
    std::vector<T> vec(cols);
    std::iota(vec.begin(), vec.end(), 0);
    matrix.set(rows - 1, vec.begin(), vec.end());
    BOOST_TEST(matrix.numRows() == rows);
    BOOST_TEST(matrix.size() == rows * cols);
    for (int i = 0; i < cols; ++i)
    {
        BOOST_TEST(vec[i] == matrix.at(rows - 1, i));
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_container_set, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols, autoResize, fillVal);
    std::vector<T> vec(cols);
    std::iota(vec.begin(), vec.end(), 0);
    matrix.set(rows - 1, vec);
    BOOST_TEST(matrix.numRows() == rows);
    BOOST_TEST(matrix.size() == rows * cols);
    for (int i = 0; i < cols; ++i)
    {
        BOOST_TEST(vec[i] == matrix.at(rows - 1, i));
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_container_r_val_set, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols, autoResize, fillVal);
    std::vector<T> vec(cols);
    std::iota(vec.begin(), vec.end(), 0);
    matrix.set(rows - 1, std::vector<T>(vec.begin(), vec.end()));
    BOOST_TEST(matrix.numRows() == rows);
    BOOST_TEST(matrix.size() == rows * cols);
    for (int i = 0; i < cols; ++i)
    {
        BOOST_TEST(vec[i] == matrix.at(rows - 1, i));
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_resize, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols);
    matrix.resize(1);
    BOOST_TEST(matrix.numRows() == 1);
    BOOST_TEST(matrix.size() == cols);
    matrix.resize(2);
    BOOST_TEST(matrix.numRows() == 2);
    BOOST_TEST(matrix.size() == 2 * cols);
    matrix.resize(rows);
    BOOST_TEST(matrix.numRows() == rows);
    BOOST_TEST(matrix.size() == rows * cols);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_resize_fail, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols);
    BOOST_CHECK_THROW(matrix.resize(rows * cols), std::length_error);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_clear_empty, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols, autoResize, fillVal);
    BOOST_TEST(!matrix.empty());
    matrix.clear();
    BOOST_TEST(matrix.empty());
    BOOST_TEST(matrix.numRows() == 0);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_extract_vector, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols);
    std::vector<T> vec(cols);
    std::iota(vec.begin(), vec.end(), 0);
    matrix.append(vec.begin(), vec.end());
    auto extractedVec = matrix.extractRow(0);
    BOOST_TEST(vec == extractedVec, tt::per_element());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_find, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols);
    std::vector<T> vec(cols);
    std::iota(vec.begin(), vec.end(), 0);
    for (int i = 0; i < matrix.rows(); ++i)
    {
        matrix.append(vec);
    }
    auto coords = matrix.find(25);
    BOOST_TEST(coords.first == 0);
    BOOST_TEST(coords.second == 25);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(matrix_accessors)

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_at_row_col, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols, autoResize);
    matrix.at(0, 0) = 1;
    BOOST_TEST(matrix.at(0, 0) == 1);
    matrix.at(1, 0) = 2;
    BOOST_TEST(matrix.at(1, 0) == 2);
    matrix.at(rows - 1, 0) = 3;
    BOOST_TEST(matrix.at(rows - 1, 0) == 3);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_at_row_col_const, T, test_types, SmallMatrix)
{
    const Matrix<T> matrix(rows, cols, autoResize, fillVal);
    auto val = matrix.at(0, 0);
    BOOST_TEST(val == fillVal);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_at_row_col_fail, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols);
    BOOST_CHECK_THROW(matrix.at(rows, 0) = 1, std::out_of_range);
    BOOST_CHECK_THROW(matrix.at(-1, 0) = 1, std::out_of_range);
    BOOST_CHECK_THROW(matrix.at(0, cols) = 1, std::out_of_range);
    BOOST_CHECK_THROW(matrix.at(0, -1) = 1, std::out_of_range);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_at_row_col_const_fail, T, test_types, SmallMatrix)
{
    const Matrix<T> matrix(rows, cols);
    T val;
    BOOST_CHECK_THROW(val = matrix.at(rows, 0), std::out_of_range);
    BOOST_CHECK_THROW(val = matrix.at(-1, 0), std::out_of_range);
    BOOST_CHECK_THROW(val = matrix.at(0, cols), std::out_of_range);
    BOOST_CHECK_THROW(val = matrix.at(0, -1), std::out_of_range);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_iterator, T, test_types, SmallMatrix)
{
    Matrix<T> matrix(rows, cols, autoResize, fillVal);
    BOOST_CHECK_EQUAL(std::distance(matrix.begin(), matrix.end()), rows * cols);
    for (auto& val : matrix)
    {
        BOOST_TEST(val == fillVal);
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_const_iterator, T, test_types, SmallMatrix)
{
    const Matrix<T> matrix(rows, cols, autoResize, fillVal);
    BOOST_CHECK_EQUAL(std::distance(matrix.cbegin(), matrix.cend()), rows * cols);
    for (const auto& val : matrix)
    {
        BOOST_TEST(val == fillVal);
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_row_iterator, T, test_types, SmallMatrix)
{
    std::array<Matrix<T>, 2> matrices = { Matrix<T>(1, cols), Matrix<T>(2, cols) };
    for (auto& matrix : matrices)
    {
        std::vector<T> vec(cols);
        std::iota(vec.begin(), vec.end(), 0);
        matrix.append(vec.begin(), vec.end());
        BOOST_REQUIRE_EQUAL(std::distance(matrix.rowBegin(0), matrix.rowEnd(0)), cols);
        BOOST_CHECK_EQUAL_COLLECTIONS(vec.begin(), vec.end(), matrix.rowBegin(0), matrix.rowEnd(0));
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_const_row_iterator, T, test_types, SmallMatrix)
{
    std::array<Matrix<T>, 2> matrices = { Matrix<T>(1, cols), Matrix<T>(2, cols) };
    for (auto& matrix : matrices)
    {
        std::vector<T> vec(cols);
        std::iota(vec.begin(), vec.end(), 0);
        matrix.append(vec.begin(), vec.end());
        BOOST_REQUIRE_EQUAL(std::distance(matrix.crowBegin(0), matrix.crowEnd(0)), cols);
        BOOST_CHECK_EQUAL_COLLECTIONS(vec.begin(), vec.end(), matrix.crowBegin(0), matrix.crowEnd(0));
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_col_iterator, T, test_types, SmallMatrix)
{
    std::array<Matrix<T>, 2> matrices = { Matrix<T>(1, cols), Matrix<T>(6, cols) };
    for (auto& matrix : matrices)
    {
        std::vector<T> vec(cols);
        std::iota(vec.begin(), vec.end(), 0);
        for (int i = 0; i < matrix.rows(); ++i)
        {
            matrix.append(vec.begin(), vec.end());
        }

        BOOST_REQUIRE_EQUAL(std::distance(matrix.colBegin(0), matrix.colEnd(0)), matrix.rows());
        for (int i = 0; i < cols; ++i)
        {
            BOOST_TEST(std::all_of(matrix.colBegin(i), matrix.colEnd(i), [&i](T val) { return val == i; }));
        }
    }
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(test_const_col_iterator, T, test_types, SmallMatrix)
{
    std::array<Matrix<T>, 2> matrices = { Matrix<T>(1, cols), Matrix<T>(6, cols) };
    for (auto& matrix : matrices)
    {
        std::vector<T> vec(cols);
        std::iota(vec.begin(), vec.end(), 0);
        for (int i = 0; i < matrix.rows(); ++i)
        {
            matrix.append(vec.begin(), vec.end());
        }

        BOOST_REQUIRE_EQUAL(std::distance(matrix.ccolBegin(0), matrix.ccolEnd(0)), matrix.rows());
        for (int i = 0; i < cols; ++i)
        {
            BOOST_TEST(std::all_of(matrix.ccolBegin(i), matrix.ccolEnd(i), [&i](T val) { return val == i; }));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()