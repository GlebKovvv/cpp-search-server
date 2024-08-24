#pragma once
#include <vector>
#include <iterator>
#include <algorithm>

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
        : first(begin), last(end), size_(std::distance(begin, end)) {}

    Iterator begin() const { return first; }
    Iterator end() const { return last; }
    size_t size() const { return size_; }

private:
    Iterator first, last;
    size_t size_;
};

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, size_t page_size) {
        for (Iterator it = begin; it != end; ) {
            size_t distance_to_end = std::distance(it, end);
            Iterator next_it = std::next(it, std::min(page_size, distance_to_end));
            pages.push_back({it, next_it});
            it = next_it;
        }
    }

    auto begin() const { return pages.begin(); }
    auto end() const { return pages.end(); }
    size_t size() const { return pages.size(); }

private:
    std::vector<IteratorRange<Iterator>> pages;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

template <typename Iterator>
std::ostream& operator<<(std::ostream& os, const IteratorRange<Iterator>& range) {
    for (auto it = range.begin(); it != range.end(); ) {
        os << *it;
        ++it;
        if (it != range.end()) {
            os << "";
        }
    }
    return os;
}
