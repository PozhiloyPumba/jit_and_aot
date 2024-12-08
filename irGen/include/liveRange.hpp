#ifndef __LIVE_RANGE_HPP__
#define __LIVE_RANGE_HPP__

#include <algorithm>
#include <cstdint>

namespace IRGen {

struct LiveRange {
    uint64_t start = 0;
    uint64_t end = 0;

    inline LiveRange &operator+=(const LiveRange &other) {
        if (start == end) {
            start = other.start;
            end = other.end;
        } else {
            start = std::min(start, other.start);
            end = std::max(end, other.end);
        }
        return *this;
    }

    inline bool operator==(const LiveRange &other) const {
        return start == other.start && end == other.end;
    }

    inline bool isValid() const {
        return end >= start + 2;
    }
};

} // namespace IRGen

#endif