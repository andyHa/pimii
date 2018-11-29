//
// Created by Andreas Haufler on 26.11.18.
//

#include "Primitives.h"

namespace pimii {

    const Offset Primitives::PRIMITIVE_EQUALITY = 0;
    const Offset Primitives::PRIMITIVE_LESS_THAN = 1;
    const Offset Primitives::PRIMITIVE_LESS_THAN_OR_EQUAL = 2;
    const Offset Primitives::PRIMITIVE_GREATER_THAN = 3;
    const Offset Primitives::PRIMITIVE_GREATER_THAN_OR_EQUAL = 4;

    const Offset Primitives::PRIMITIVE_ADD = 5;
    const Offset Primitives::PRIMITIVE_SUBTRACT = 6;
    const Offset Primitives::PRIMITIVE_MULTIPLY = 7;
    const Offset Primitives::PRIMITIVE_DIVIDE = 8;
    const Offset Primitives::PRIMITIVE_REMAINDER = 9;

    const Offset Primitives::PRIMITIVE_BASIC_NEW = 10;
    const Offset Primitives::PRIMITIVE_BASIC_NEW_WITH = 11;
    const Offset Primitives::PRIMITIVE_CLASS = 12;
    const Offset Primitives::PRIMITIVE_BLOCK_COPY = 13;
    const Offset Primitives::PRIMITIVE_VALUE_NO_ARG = 14;
    const Offset Primitives::PRIMITIVE_VALUE_ONE_ARG = 15;
    const Offset Primitives::PRIMITIVE_VALUE_TWO_ARGS = 16;
    const Offset Primitives::PRIMITIVE_VALUE_THREE_ARGS = 17;
    const Offset Primitives::PRIMITIVE_VALUE_N_ARGS = 18;
    const Offset Primitives::PRIMITIVE_PERFORM_NO_ARG = 19;
    const Offset Primitives::PRIMITIVE_PERFORM_ONE_ARG = 20;
    const Offset Primitives::PRIMITIVE_PERFORM_TWO_ARGS = 21;
    const Offset Primitives::PRIMITIVE_PERFORM_THREE_ARGS = 22;
    const Offset Primitives::PRIMITIVE_PERFORM_N_ARGS = 23;

    const Offset Primitives::PRIMITIVE_HASH = 24;
    const Offset Primitives::PRIMITIVE_SIZE = 25;
    const Offset Primitives::PRIMITIVE_AT = 26;
    const Offset Primitives::PRIMITIVE_AT_PUT = 27;
    const Offset Primitives::PRIMITIVE_AS_SYMBOL = 28;
    const Offset Primitives::PRIMITIVE_AS_STRING = 29;

    const std::array<Primitive, 30> Primitives::methods = {equality, lessThan, lessThanOrEqual, greaterThan,
                                                           greaterThanOrEqual, add, subtract, multiply, divide,
                                                           remainder, basicNew, basicNewWith, clazz, hash, size,
                                                           blockCopy, value, value, value, value, valueWith, perform,
                                                           perform, perform, perform, performWith, at, atPut, asSymbol,
                                                           asString};

    bool Primitives::executePrimitive(Offset index, Interpreter &interpreter, Offset argumentCount) {
        return methods[index](interpreter, argumentCount);
    }

    bool Primitives::equality(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        interpreter.push(interpreter.pop() == interpreter.pop() ? interpreter.getSystem().trueValue
                                                                : interpreter.getSystem().falseValue);
        return true;
    }

    bool Primitives::lessThan(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1 || interpreter.stackTop().getObjectPointerType() != SMALL_INT ||
            interpreter.stackValue(1).getObjectPointerType() != SMALL_INT) {
            return false;
        }

        SmallInteger arg = interpreter.pop().getInt();
        SmallInteger self = interpreter.pop().getInt();

        interpreter.push(self < arg ? interpreter.getSystem().trueValue
                                    : interpreter.getSystem().falseValue);
        return true;
    }

    bool Primitives::lessThanOrEqual(pimii::Interpreter &interpreter, pimii::Offset argumentCount) {
        if (argumentCount != 1 || interpreter.stackTop().getObjectPointerType() != SMALL_INT ||
            interpreter.stackValue(1).getObjectPointerType() != SMALL_INT) {
            return false;
        }

        SmallInteger arg = interpreter.pop().getInt();
        SmallInteger self = interpreter.pop().getInt();

        interpreter.push(self <= arg ? interpreter.getSystem().trueValue
                                     : interpreter.getSystem().falseValue);
        return true;
    }

    bool Primitives::greaterThan(pimii::Interpreter &interpreter, pimii::Offset argumentCount) {
        if (argumentCount != 1 || interpreter.stackTop().getObjectPointerType() != SMALL_INT ||
            interpreter.stackValue(1).getObjectPointerType() != SMALL_INT) {
            return false;
        }

        SmallInteger arg = interpreter.pop().getInt();
        SmallInteger self = interpreter.pop().getInt();

        interpreter.push(self > arg ? interpreter.getSystem().trueValue
                                    : interpreter.getSystem().falseValue);
        return true;
    }

    bool Primitives::greaterThanOrEqual(pimii::Interpreter &interpreter, pimii::Offset argumentCount) {
        if (argumentCount != 1 || interpreter.stackTop().getObjectPointerType() != SMALL_INT ||
            interpreter.stackValue(1).getObjectPointerType() != SMALL_INT) {
            return false;
        }

        SmallInteger arg = interpreter.pop().getInt();
        SmallInteger self = interpreter.pop().getInt();

        interpreter.push(self >= arg ? interpreter.getSystem().trueValue
                                     : interpreter.getSystem().falseValue);
        return true;
    }

    bool Primitives::add(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1 || interpreter.stackTop().getObjectPointerType() != SMALL_INT ||
            interpreter.stackValue(1).getObjectPointerType() != SMALL_INT) {
            return false;
        }

        SmallInteger arg = interpreter.pop().getInt();
        SmallInteger self = interpreter.pop().getInt();
        //TODO limits

        interpreter.push(ObjectPointer(self + arg));
        return true;
    }

    bool Primitives::subtract(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1 || interpreter.stackTop().getObjectPointerType() != SMALL_INT ||
            interpreter.stackValue(1).getObjectPointerType() != SMALL_INT) {
            return false;
        }

        SmallInteger arg = interpreter.pop().getInt();
        SmallInteger self = interpreter.pop().getInt();
//TODO limits
        interpreter.push(ObjectPointer(self - arg));
        return true;
    }

    bool Primitives::multiply(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1 || interpreter.stackTop().getObjectPointerType() != SMALL_INT ||
            interpreter.stackValue(1).getObjectPointerType() != SMALL_INT) {
            return false;
        }

        SmallInteger arg = interpreter.pop().getInt();
        SmallInteger self = interpreter.pop().getInt();
//TODO limits
        interpreter.push(ObjectPointer(self * arg));
        return true;
    }

    bool Primitives::divide(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1 || interpreter.stackTop().getObjectPointerType() != SMALL_INT ||
            interpreter.stackValue(1).getObjectPointerType() != SMALL_INT) {
            return false;
        }

        SmallInteger arg = interpreter.pop().getInt();
        SmallInteger self = interpreter.pop().getInt();
//TODO limits
        interpreter.push(ObjectPointer(self / arg));
        return true;
    }

    bool Primitives::remainder(pimii::Interpreter &interpreter, pimii::Offset argumentCount) {
        if (argumentCount != 1 || interpreter.stackTop().getObjectPointerType() != SMALL_INT ||
            interpreter.stackValue(1).getObjectPointerType() != SMALL_INT) {
            return false;
        }

        SmallInteger arg = interpreter.pop().getInt();
        SmallInteger self = interpreter.pop().getInt();
//TODO limits
        interpreter.push(ObjectPointer(self % arg));
        return true;
    }

    bool Primitives::basicNew(Interpreter &interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::basicNewWith(Interpreter &interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::clazz(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        interpreter.push(interpreter.getSystem().getType(interpreter.pop()));

        return true;
    }

    bool Primitives::hash(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        //TODO make consistent with native code
        interpreter.push(interpreter.getSystem().getType(interpreter.pop()));
        return false;
    }

    bool Primitives::size(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer self = interpreter.pop();
        switch (self.getObjectPointerType()) {
            case SMALL_INT:
                interpreter.push(ObjectPointer(0));
                return true;
            case OBJECT:
                interpreter.push(ObjectPointer(self.getObject()->size -
                                               self.getObject()->type.getObject()->fields[TypeSystem::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].getInt()));
                return true;
            case WORDS:
                interpreter.push(ObjectPointer(self.getWords()->size));
                return true;
            case BYTES:
                interpreter.push(ObjectPointer(self.getBytes()->size * sizeof(Word) - self.getBytes()->odd));
                return true;
        }
    }


    bool Primitives::blockCopy(Interpreter &interpreter, Offset argumentCount) {
        SmallInteger blockArgumentCount = interpreter.pop().getInt();
        Object *context = interpreter.getActiveContext();
        Object *methodContext;
        if (interpreter.isBlockContext(context)) {
            methodContext = context->fields[Interpreter::CONTEXT_HOME_FIELD].getObject();
        } else {
            methodContext = context;
        };

        Object *newContext = interpreter.getSystem().getMemoryManager().allocObject(
                context->size, interpreter.getSystem().getTypeSystem().blockContextType);

        newContext->fields[Interpreter::CONTEXT_INITIAL_IP_FIELD] = ObjectPointer(
                interpreter.getInstructionPointer() + 2);
        newContext->fields[Interpreter::CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD] = ObjectPointer(blockArgumentCount);
        newContext->fields[Interpreter::CONTEXT_HOME_FIELD] = ObjectPointer(methodContext);
        interpreter.push(ObjectPointer(newContext));

        return true;
    }

    bool Primitives::value(Interpreter &interpreter, Offset argumentCount) {
        ObjectPointer blockContext = interpreter.stackValue(argumentCount);
        // This primitive can only handle BlockContexts
        if (blockContext.getObjectPointerType() != OBJECT ||
            blockContext.getObject()->type != interpreter.getSystem().getTypeSystem().blockContextType) {
            return false;
        }

        Object *block = blockContext.getObject();
        SmallInteger blockArgumentCount = block->fields[Interpreter::CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD].getInt();
        if (blockArgumentCount != argumentCount) {
            return false;
        }

        interpreter.transfer(argumentCount,
                             interpreter.getActiveContext(),
                             interpreter.getStackBasePointer() + interpreter.getStackPointer() - argumentCount + 1,
                             block, Interpreter::CONTEXT_FIXED_SIZE);

        interpreter.pop(argumentCount + 1);

        block->fields[Interpreter::CONTEXT_IP_FIELD] = ObjectPointer(
                block->fields[Interpreter::CONTEXT_INITIAL_IP_FIELD].getInt());
        block->fields[Interpreter::CONTEXT_SP_FIELD] = ObjectPointer(argumentCount);
        block->fields[Interpreter::CONTEXT_CALLER_FIELD] = ObjectPointer(interpreter.getActiveContext());
        interpreter.newActiveContext(block);
        return true;
    }

    bool Primitives::valueWith(Interpreter &interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::perform(Interpreter &interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::performWith(Interpreter &interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::at(Interpreter &interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::atPut(Interpreter &interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::asSymbol(Interpreter &interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::asString(Interpreter &interpreter, Offset argumentCount) {
        return false;
    }

}