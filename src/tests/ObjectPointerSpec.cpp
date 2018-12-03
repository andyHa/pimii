#include "catch.hpp"
#include "../vm/ObjectPointer.h"
#include <memory>

namespace pimii {

    TEST_CASE("SmallIntegers can be embedded in ObjectPointers", "[objectpointer]") {

        REQUIRE(ObjectPointer(5).smallInt() == 5);
        REQUIRE(ObjectPointer(-1).smallInt() == -1);
        REQUIRE(ObjectPointer(0).smallInt() == 0);

        REQUIRE(ObjectPointer(minSmallInt()).smallInt() == minSmallInt());
        REQUIRE(ObjectPointer(maxSmallInt()).smallInt() == maxSmallInt());

    }

    TEST_CASE("Wrapping a byte array as ObjectPointer", "[objectpointer]") {

        struct {
            Word size;
            Word type;
            char test[10];
        } test;

        Offset numberOfWords = 10 / sizeof(Word);
        Offset odd = sizeof(Word) - (10 % sizeof(Word));
        if (odd != 0) {
            numberOfWords++;
        }

        ObjectPointer pointer(&test, Nil::NIL, numberOfWords, odd);

        REQUIRE(pointer.byteSize() == 10);
        REQUIRE(pointer.size() == numberOfWords);

        test.test[0] = 'A';
        test.test[9] = 'Z';

        REQUIRE(pointer.fetchByte(0) == 'A');
        REQUIRE(pointer.fetchByte(9) == 'Z');
    }

}