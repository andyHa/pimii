#pragma once


#include <cstdint>
#include <limits>

namespace pimii {

    using Word = uint64_t;
    using SmallInteger = int64_t;
    using Decimal = float;

    static_assert(sizeof(Word) == sizeof(SmallInteger));
    static_assert(sizeof(Word) > sizeof(Decimal));

    class SmallIntegers {
    public:
        static constexpr SmallInteger minSmallInt() {
            return static_cast<SmallInteger>((std::numeric_limits<Word>::min() >> 3) * -1);
        }

        static constexpr SmallInteger maxSmallInt() {
            return static_cast<SmallInteger>((std::numeric_limits<Word>::max() >> 3) - 1);
        }

        static SmallInteger toSmallInteger(size_t value) {
            if (value > maxSmallInt() || value < minSmallInt()) {
                throw std::range_error("No valid small integer!");
            }

            return static_cast<SmallInteger>(value);
        }

        static SmallInteger toSafeSmallInteger(size_t value) {
            return static_cast<SmallInteger>(value & maxSmallInt());
        }
    };

}