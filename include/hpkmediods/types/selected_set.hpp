#pragma once

#include <vector>

namespace hpkmediods
{
class SelectedSet
{
public:
    typedef std::vector<int32_t> selected_t;
    typedef std::vector<int32_t> unselected_t;
    typedef std::vector<int32_t>::iterator iterator;
    typedef std::vector<int32_t>::const_iterator const_iterator;

    SelectedSet();

    SelectedSet(const int32_t numData, const int32_t numClusters);

    void select(const int32_t idx);

    void replaceSelected(const int32_t idx, const int32_t centroidIdx);

    bool seleContains(const int32_t idx);

    bool unseleContains(const int32_t idx);

    iterator seleBegin();

    iterator seleEnd();

    iterator unseleBegin();

    iterator unseleEnd();

    const_iterator cseleBegin() const;

    const_iterator cseleEnd() const;

    const_iterator cunseleBegin() const;

    const_iterator cunseleEnd() const;

    const selected_t& selected() const;

    const unselected_t& unselected() const;

    int32_t size() const;

    int32_t selectedSize() const;

    int32_t unselectedSize() const;

    int32_t maxSelectedSize() const;

private:
    int32_t m_numClusters;
    std::vector<int32_t> m_unselected;
    std::vector<int32_t> m_selected;
};
}  // namespace hpkmediods
