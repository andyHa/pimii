//
// Created by Andreas Haufler on 26.11.18.
//

#ifndef MEM_PRIMITIVES_H
#define MEM_PRIMITIVES_H

#include <array>
#include "Interpreter.h"

namespace pimii {

    typedef bool(* Primitive)(Interpreter& interpreter, Offset argumentCount);

    class Primitives {

        static bool equality(Interpreter& interpreter, Offset argumentCount);

        static bool lessThan(Interpreter& interpreter, Offset argumentCount);

        static bool lessThanOrEqual(Interpreter& interpreter, Offset argumentCount);

        static bool greaterThan(Interpreter& interpreter, Offset argumentCount);

        static bool greaterThanOrEqual(Interpreter& interpreter, Offset argumentCount);

        static bool add(Interpreter& interpreter, Offset argumentCount);

        static bool subtract(Interpreter& interpreter, Offset argumentCount);

        static bool multiply(Interpreter& interpreter, Offset argumentCount);

        static bool divide(Interpreter& interpreter, Offset argumentCount);

        static bool remainder(Interpreter& interpreter, Offset argumentCount);

        static bool basicNew(Interpreter& interpreter, Offset argumentCount);

        static bool basicNewWith(Interpreter& interpreter, Offset argumentCount);

        static bool clazz(Interpreter& interpreter, Offset argumentCount);

        static bool id(Interpreter& interpreter, Offset argumentCount);

        static bool size(Interpreter& interpreter, Offset argumentCount);

        static bool fork(Interpreter& interpreter, Offset argumentCount);

        static bool signal(Interpreter& interpreter, Offset argumentCount);

        static bool wait(Interpreter& interpreter, Offset argumentCount);

        static bool value(Interpreter& interpreter, Offset argumentCount);

        static bool valueWith(Interpreter& interpreter, Offset argumentCount);

        static bool perform(Interpreter& interpreter, Offset argumentCount);

        static bool performWith(Interpreter& interpreter, Offset argumentCount);

        static bool at(Interpreter& interpreter, Offset argumentCount);

        static bool atPut(Interpreter& interpreter, Offset argumentCount);

        static bool asSymbol(Interpreter& interpreter, Offset argumentCount);

        static bool asString(Interpreter& interpreter, Offset argumentCount);

        static bool concat(Interpreter& interpreter, Offset argumentCount);

        static bool sysOut(Interpreter& interpreter, Offset argumentCount);

        static inline const std::array<Primitive, 34> methods = {equality, lessThan, lessThanOrEqual, greaterThan,
                                                                 greaterThanOrEqual, add, subtract, multiply, divide,
                                                                 remainder, basicNew, basicNewWith, clazz,
                                                                 value, value, value, value, valueWith,
                                                                 perform, perform, perform, perform, performWith, id,
                                                                 size, fork, wait, signal, at, atPut, asSymbol,
                                                                 asString, concat, sysOut};


    public:
        // These are preferred primitives. This list matches the special selectors in System.cpp
        // and these primitives will be invoked in place of an actual method call.
        static inline const Offset PRIMITIVE_EQUALITY = 0;
        static inline const Offset PRIMITIVE_LESS_THAN = 1;
        static inline const Offset PRIMITIVE_LESS_THAN_OR_EQUAL = 2;
        static inline const Offset PRIMITIVE_GREATER_THAN = 3;
        static inline const Offset PRIMITIVE_GREATER_THAN_OR_EQUAL = 4;

        static inline const Offset PRIMITIVE_ADD = 5;
        static inline const Offset PRIMITIVE_SUBTRACT = 6;
        static inline const Offset PRIMITIVE_MULTIPLY = 7;
        static inline const Offset PRIMITIVE_DIVIDE = 8;
        static inline const Offset PRIMITIVE_REMAINDER = 9;

        static inline const Offset PRIMITIVE_BASIC_NEW = 10;
        static inline const Offset PRIMITIVE_BASIC_NEW_WITH = 11;
        static inline const Offset PRIMITIVE_CLASS = 12;
        static inline const Offset PRIMITIVE_VALUE_NO_ARG = 13;
        static inline const Offset PRIMITIVE_VALUE_ONE_ARG = 14;
        static inline const Offset PRIMITIVE_VALUE_TWO_ARGS = 15;
        static inline const Offset PRIMITIVE_VALUE_THREE_ARGS = 16;
        static inline const Offset PRIMITIVE_VALUE_N_ARGS = 17;
        static inline const Offset PRIMITIVE_PERFORM_NO_ARG = 18;
        static inline const Offset PRIMITIVE_PERFORM_ONE_ARG = 19;
        static inline const Offset PRIMITIVE_PERFORM_TWO_ARGS = 20;
        static inline const Offset PRIMITIVE_PERFORM_THREE_ARGS = 21;
        static inline const Offset PRIMITIVE_PERFORM_N_ARGS = 22;

        // These primitives can be invoked via methods but also be overwritten by classes.
        static inline const Offset PRIMITIVE_ID = 23;
        static inline const Offset PRIMITIVE_SIZE = 24;
        static inline const Offset PRIMITIVE_FORK = 25;
        static inline const Offset PRIMITIVE_WAIT = 26;
        static inline const Offset PRIMITIVE_SIGNAL = 27;
        static inline const Offset PRIMITIVE_AT = 28;
        static inline const Offset PRIMITIVE_AT_PUT = 29;
        static inline const Offset PRIMITIVE_AS_SYMBOL = 30;
        static inline const Offset PRIMITIVE_AS_STRING = 31;
        static inline const Offset PRIMITIVE_CONCAT = 32;
        static inline const Offset PRIMITIVE_SYSOUT = 33;

        static inline bool executePrimitive(Offset index, Interpreter& interpreter, Offset argumentCount) {
            return methods[index](interpreter, argumentCount);
        }
    };

}


#endif //MEM_PRIMITIVES_H
