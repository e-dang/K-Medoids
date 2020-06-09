#include <array>
#include <hpkmedoids/types/selected_set.hpp>
#define BOOST_TEST_MODULE test_selected_set
#include <boost/test/unit_test.hpp>

using namespace hpkmedoids;

struct SelectedSetFixture
{
    SelectedSetFixture() : numData(100), numClusters(5), selectedSet(numData, numClusters) {}

    ~SelectedSetFixture() {}

    int32_t numData;
    int numClusters;
    SelectedSet selectedSet;
};

std::array<int32_t, 5> selectIndices(SelectedSet& selectedSet)
{
    std::array<int32_t, 5> indices = { 5, 25, 30, 76, 91 };
    for (auto& idx : indices)
    {
        selectedSet.select(idx);
    }

    return indices;
}

BOOST_FIXTURE_TEST_CASE(test_selected_set_constructor, SelectedSetFixture)
{
    BOOST_TEST(selectedSet.selectedSize() == 0);
    BOOST_TEST(selectedSet.unselectedSize() == numData);
    BOOST_TEST(selectedSet.size() == numData);
    BOOST_TEST(selectedSet.maxSelectedSize() == numClusters);
}

BOOST_FIXTURE_TEST_CASE(test_selected_set_select_1_param, SelectedSetFixture)
{
    auto indices = selectIndices(selectedSet);
    for (auto& idx : indices)
    {
        BOOST_TEST(selectedSet.seleContains(idx));
        BOOST_TEST(!selectedSet.unseleContains(idx));
    }
}

BOOST_FIXTURE_TEST_CASE(test_selected_set_select_1_param_fail, SelectedSetFixture)
{
    selectIndices(selectedSet);
    BOOST_CHECK_THROW(selectedSet.select(92), std::length_error);
}

BOOST_FIXTURE_TEST_CASE(test_selected_set_replaceSelected, SelectedSetFixture)
{
    int dataIdx     = 67;
    int centroidIdx = 3;

    selectIndices(selectedSet);
    auto prevSelection = *(selectedSet.seleBegin() + centroidIdx);
    selectedSet.replaceSelected(dataIdx, centroidIdx);

    BOOST_TEST(std::distance(selectedSet.seleBegin(),
                             std::find(selectedSet.seleBegin(), selectedSet.seleEnd(), dataIdx)) == centroidIdx);
    BOOST_TEST(selectedSet.unseleContains(prevSelection));
}