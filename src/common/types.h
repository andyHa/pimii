#pragma once


#include <cstdint>
#include <limits>

namespace pimii {

    using Word = uint64_t;
    using SmallInteger = int64_t;
    using Decimal = float;
    using Offset = uint64_t;

    constexpr SmallInteger minSmallInt() {
        return (SmallInteger) ((std::numeric_limits<Word>::min() >> 3) * -1);
    }

    constexpr SmallInteger maxSmallInt() {
        return (SmallInteger) ((std::numeric_limits<Word>::max() >> 3) - 1);
    }

    constexpr Offset maxOffset() {
        return (Offset) (std::numeric_limits<Word>::max() >> 8);
    }

    constexpr Offset wordSize() {
        return sizeof(Word);
    }

    constexpr Offset usableSizeBytes() {
        return sizeof(Word) - 1;
    }

}