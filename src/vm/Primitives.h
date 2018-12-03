//
// Created by Andreas Haufler on 26.11.18.
//

#ifndef MEM_PRIMITIVES_H
#define MEM_PRIMITIVES_H

#include <array>
#include "Interpreter.h"

namespace pimii {

    typedef bool(*Primitive)(Interpreter &interpreter, Offset argumentCount);

    class Primitives {

        static bool equality(Interpreter &interpreter, Offset argumentCount);

        static bool lessThan(Interpreter &interpreter, Offset argumentCount);

        static bool lessThanOrEqual(Interpreter &interpreter, Offset argumentCount);

        static bool greaterThan(Interpreter &interpreter, Offset argumentCount);

        static bool greaterThanOrEqual(Interpreter &interpreter, Offset argumentCount);

        static bool add(Interpreter &interpreter, Offset argumentCount);

        static bool subtract(Interpreter &interpreter, Offset argumentCount);

        static bool multiply(Interpreter &interpreter, Offset argumentCount);

        static bool divide(Interpreter &interpreter, Offset argumentCount);

        static bool remainder(Interpreter &interpreter, Offset argumentCount);

        static bool basicNew(Interpreter &interpreter, Offset argumentCount);

        static bool basicNewWith(Interpreter &interpreter, Offset argumentCount);

        static bool clazz(Interpreter &interpreter, Offset argumentCount);

        static bool hash(Interpreter &interpreter, Offset argumentCount);

        static bool size(Interpreter &interpreter, Offset argumentCount);

        static bool value(Interpreter &interpreter, Offset argumentCount);

        static bool valueWith(Interpreter &interpreter, Offset argumentCount);

        static bool perform(Interpreter &interpreter, Offset argumentCount);

        static bool performWith(Interpreter &interpreter, Offset argumentCount);

        static bool at(Interpreter &interpreter, Offset argumentCount);

        static bool atPut(Interpreter &interpreter, Offset argumentCount);

        static bool asSymbol(Interpreter &interpreter, Offset argumentCount);

        static bool asString(Interpreter &interpreter, Offset argumentCount);

        static inline const std::array<Primitive, 30> methods = {equality, lessThan, lessThanOrEqual, greaterThan,
                                                                 greaterThanOrEqual, add, subtract, multiply, divide,
                                                                 remainder, basicNew, basicNewWith, clazz, hash, size,
                                                                 value, value, value, value, valueWith,
                                                                 perform,
                                                                 perform, perform, perform, performWith, at, atPut,
                                                                 asSymbol,
                                                                 asString};


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
        static inline const Offset PRIMITIVE_HASH = 23;
        static inline const Offset PRIMITIVE_SIZE = 24;
        static inline const Offset PRIMITIVE_AT = 25;
        static inline const Offset PRIMITIVE_AT_PUT = 26;
        static inline const Offset PRIMITIVE_AS_SYMBOL = 27;
        static inline const Offset PRIMITIVE_AS_STRING = 28;

        static inline bool executePrimitive(Offset index, Interpreter &interpreter, Offset argumentCount) {
            return methods[index](interpreter, argumentCount);
        }
    };

}


#endif //MEM_PRIMITIVES_H
