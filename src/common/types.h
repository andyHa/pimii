#pragma once


#include <cstdint>
#include <limits>

namespace pimii {

    using Word = uint64_t;
    using SmallInteger = int32_t;
    using Decimal = float;

    class SmallIntegers {
    public:
        static constexpr SmallInteger minSmallInt() {
            return std::numeric_limits<int32_t>::min();
        }

        static constexpr SmallInteger maxSmallInt() {
            return std::numeric_limits<int32_t>::max();
        }

        static SmallInteger toSmallInteger(size_t value) {
            if (value > maxSmallInt()) {
                throw std::range_error("No valid small integer!");
            }

            return static_cast<SmallInteger>(value);
        }

        static SmallInteger toSafeSmallInteger(size_t value) {
            return static_cast<SmallInteger>(value & maxSmallInt());
        }
    };

}