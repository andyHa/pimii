//
// Created by Andreas Haufler on 26.11.18.
//

#ifndef MEM_PRIMITIVES_H
#define MEM_PRIMITIVES_H

#include <array>
#include "Interpreter.h"

namespace pimii {

    typedef bool(* Primitive)(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

    class Primitives {

        static bool equality(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool lessThan(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool lessThanOrEqual(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool greaterThan(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool greaterThanOrEqual(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool add(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool subtract(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool multiply(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool divide(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool remainder(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool bitAnd(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool bitOr(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool bitInvert(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool shiftLeft(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool shiftRight(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool basicNew(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool basicNewWith(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool basicAllocWith(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool byteAt(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool byteAtPut(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool transferBytes(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool compareBytes(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool hashBytes(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool clazz(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool id(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool size(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool objectSize(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool fork(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool signal(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool wait(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool value(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool valueWith(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool perform(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool performWith(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool objectAt(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool objectAtPut(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool objectTransfer(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool at(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool atPut(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool transfer(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool terminalNextEvent(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool terminalSize(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool terminalShowString(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool terminalShowBox(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool terminalShowCursor(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool terminalHideCursor(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static bool terminalDraw(Interpreter& interpreter, System& sys, SmallInteger argumentCount);

        static constexpr std::array<Primitive, 53> methods = {equality, lessThan, lessThanOrEqual, greaterThan,
                                                              greaterThanOrEqual, add, subtract, multiply, divide,
                                                              remainder, bitAnd, bitOr, bitInvert, shiftLeft,
                                                              shiftRight, basicNew, basicNewWith, basicAllocWith,
                                                              byteAt, byteAtPut, transferBytes, compareBytes, hashBytes,
                                                              clazz, value, value, value, value, valueWith, perform,
                                                              perform, perform, perform, performWith, objectAt,
                                                              objectAtPut, objectTransfer, id, size, objectSize, fork,
                                                              wait, signal, at, atPut, transfer, terminalNextEvent,
                                                              terminalSize, terminalShowString, terminalShowBox,
                                                              terminalShowCursor, terminalHideCursor, terminalDraw};


    public:
        static inline bool
        executePrimitive(SmallInteger index, Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
            return methods[index](interpreter, sys, argumentCount);
        }
    };

}


#endif //MEM_PRIMITIVES_H
