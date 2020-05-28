#include <hpkmediods/types/selected_set.hpp>
#include <numeric>

namespace hpkmediods
{
SelectedSet::SelectedSet() : m_numClusters(0), m_unselected(), m_selected() {}

SelectedSet::SelectedSet(const int32_t numData, const int32_t numClusters) :
    m_numClusters(numClusters), m_unselected(numData), m_selected(0)
{
    std::iota(unseleBegin(), unseleEnd(), 0);
}

void SelectedSet::select(const int32_t idx)
{
    if (static_cast<int32_t>(m_selected.size()) >= m_numClusters)
        throw std::length_error("The maximum number of centroids has already been selected!");

    m_unselected.erase(std::find(cunseleBegin(), cunseleEnd(), idx));
    m_selected.push_back(idx);
}

void SelectedSet::replaceSelected(const int32_t dataIdx, const int32_t centroidIdx)
{
    m_unselected.push_back(m_selected[centroidIdx]);
    m_unselected.erase(std::find(cunseleBegin(), cunseleEnd(), dataIdx));
    m_selected[centroidIdx] = dataIdx;
}

bool SelectedSet::seleContains(const int32_t idx)
{
    return std::find(m_selected.cbegin(), m_selected.cend(), idx) != m_selected.cend();
}

bool SelectedSet::unseleContains(const int32_t idx)
{
    return std::find(m_unselected.cbegin(), m_unselected.cend(), idx) != m_unselected.cend();
}

typename SelectedSet::iterator SelectedSet::seleBegin() { return m_selected.begin(); }

typename SelectedSet::iterator SelectedSet::seleEnd() { return m_selected.end(); }

typename SelectedSet::iterator SelectedSet::unseleBegin() { return m_unselected.begin(); }

typename SelectedSet::iterator SelectedSet::unseleEnd() { return m_unselected.end(); }

typename SelectedSet::const_iterator SelectedSet::cseleBegin() const { return m_selected.cbegin(); }

typename SelectedSet::const_iterator SelectedSet::cseleEnd() const { return m_selected.cend(); }

typename SelectedSet::const_iterator SelectedSet::cunseleBegin() const { return m_unselected.cbegin(); }

typename SelectedSet::const_iterator SelectedSet::cunseleEnd() const { return m_unselected.cend(); }

const SelectedSet::selected_t& SelectedSet::selected() const { return m_selected; }

const SelectedSet::unselected_t& SelectedSet::unselected() const { return m_unselected; }

int32_t SelectedSet::size() const { return selectedSize() + unselectedSize(); }

int32_t SelectedSet::selectedSize() const { return static_cast<int32_t>(m_selected.size()); }

int32_t SelectedSet::unselectedSize() const { return static_cast<int32_t>(m_unselected.size()); }

int32_t SelectedSet::maxSelectedSize() const { return m_numClusters; }
}  // namespace hpkmediods