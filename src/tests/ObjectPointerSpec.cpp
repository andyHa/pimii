#include "catch.hpp"
#include "../vm/ObjectPointer.h"
#include "../vm/MemoryManager.h"
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

        SmallInteger numberOfWords = 10 / sizeof(Word);
        SmallInteger odd = sizeof(Word) - (10 % sizeof(Word));
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

    TEST_CASE("Storing GC infos works", "[objectpointer]") {
        MemoryManager mm;
        ObjectPointer test = mm.makeObject(2, Nil::NIL);

        REQUIRE(test.gcInfo() == 0);
        REQUIRE(test.size() == 2);

        test.gcInfo(5);

        REQUIRE(test.gcInfo() == 5);
        REQUIRE(test.size() == 2);


        ObjectPointer testBuffer = mm.makeBuffer(10, Nil::NIL);

        REQUIRE(testBuffer.gcInfo() == 0);
        REQUIRE(testBuffer.byteSize() == 10);
        REQUIRE(testBuffer.size() == 2);

        testBuffer.gcInfo(5);

        REQUIRE(testBuffer.gcInfo() == 5);
        REQUIRE(testBuffer.byteSize() == 10);
        REQUIRE(testBuffer.size() == 2);
    }

}