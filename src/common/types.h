#pragma once


#include <cstdint>
#include <limits>

namespace pimii {

    using Word = uint64_t;
    using SmallInteger = int64_t;
    using Decimal = float;

    constexpr SmallInteger minSmallInt() {
        return (SmallInteger) ((std::numeric_limits<Word>::min() >> 3) * -1);
    }

    constexpr SmallInteger maxSmallInt() {
        return (SmallInteger) ((std::numeric_limits<Word>::max() >> 3) - 1);
    }

    constexpr SmallInteger usableSizeBytes() {
        return sizeof(Word) - 1;
    }

}