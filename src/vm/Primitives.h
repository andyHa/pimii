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
        static const std::array<Primitive, 30> methods;

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

        static bool blockCopy(Interpreter &interpreter, Offset argumentCount);

        static bool value(Interpreter &interpreter, Offset argumentCount);

        static bool valueWith(Interpreter &interpreter, Offset argumentCount);

        static bool perform(Interpreter &interpreter, Offset argumentCount);

        static bool performWith(Interpreter &interpreter, Offset argumentCount);

        static bool at(Interpreter &interpreter, Offset argumentCount);

        static bool atPut(Interpreter &interpreter, Offset argumentCount);

        static bool asSymbol(Interpreter &interpreter, Offset argumentCount);

        static bool asString(Interpreter &interpreter, Offset argumentCount);

    public:
        // These are preferred primitives. This list matches the special selectors in Interpreter.cpp
        // and these primitives will be invoked in place of an actual method call.
        static const Offset PRIMITIVE_EQUALITY;
        static const Offset PRIMITIVE_LESS_THAN;
        static const Offset PRIMITIVE_LESS_THAN_OR_EQUAL;
        static const Offset PRIMITIVE_GREATER_THAN;
        static const Offset PRIMITIVE_GREATER_THAN_OR_EQUAL;

        static const Offset PRIMITIVE_ADD;
        static const Offset PRIMITIVE_SUBTRACT;
        static const Offset PRIMITIVE_MULTIPLY;
        static const Offset PRIMITIVE_DIVIDE;
        static const Offset PRIMITIVE_REMAINDER;

        static const Offset PRIMITIVE_BASIC_NEW;
        static const Offset PRIMITIVE_BASIC_NEW_WITH;
        static const Offset PRIMITIVE_CLASS;
        static const Offset PRIMITIVE_BLOCK_COPY;
        static const Offset PRIMITIVE_VALUE_NO_ARG;
        static const Offset PRIMITIVE_VALUE_ONE_ARG;
        static const Offset PRIMITIVE_VALUE_TWO_ARGS;
        static const Offset PRIMITIVE_VALUE_THREE_ARGS;
        static const Offset PRIMITIVE_VALUE_N_ARGS;
        static const Offset PRIMITIVE_PERFORM_NO_ARG;
        static const Offset PRIMITIVE_PERFORM_ONE_ARG;
        static const Offset PRIMITIVE_PERFORM_TWO_ARGS;
        static const Offset PRIMITIVE_PERFORM_THREE_ARGS;
        static const Offset PRIMITIVE_PERFORM_N_ARGS;

        // These primitives can be invoked via methods but also be overwritten by classes.
        static const Offset PRIMITIVE_HASH;
        static const Offset PRIMITIVE_SIZE;
        static const Offset PRIMITIVE_AT;
        static const Offset PRIMITIVE_AT_PUT;
        static const Offset PRIMITIVE_AS_SYMBOL;
        static const Offset PRIMITIVE_AS_STRING;

        static bool executePrimitive(Offset index, Interpreter &interpreter, Offset argumentCount);
    };

}


#endif //MEM_PRIMITIVES_H
