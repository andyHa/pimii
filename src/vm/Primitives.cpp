//
// Created by Andreas Haufler on 26.11.18.
//

#include "Primitives.h"

namespace pimii {

    bool Primitives::equality(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        interpreter.push(interpreter.pop() == interpreter.pop() ? interpreter.getSystem().trueValue
                                                                : interpreter.getSystem().falseValue);
        return true;
    }

    bool Primitives::lessThan(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();

        interpreter.push(self < arg ? interpreter.getSystem().trueValue
                                    : interpreter.getSystem().falseValue);
        return true;
    }

    bool Primitives::lessThanOrEqual(pimii::Interpreter &interpreter, pimii::Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();

        interpreter.push(self <= arg ? interpreter.getSystem().trueValue
                                     : interpreter.getSystem().falseValue);
        return true;
    }

    bool Primitives::greaterThan(pimii::Interpreter &interpreter, pimii::Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();

        interpreter.push(self > arg ? interpreter.getSystem().trueValue
                                    : interpreter.getSystem().falseValue);
        return true;
    }

    bool Primitives::greaterThanOrEqual(pimii::Interpreter &interpreter, pimii::Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();

        interpreter.push(self >= arg ? interpreter.getSystem().trueValue
                                     : interpreter.getSystem().falseValue);
        return true;
    }

    bool Primitives::add(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
        //TODO limits

        interpreter.push(ObjectPointer(self + arg));
        return true;
    }

    bool Primitives::subtract(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
//TODO limits
        interpreter.push(ObjectPointer(self - arg));
        return true;
    }

    bool Primitives::multiply(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
//TODO limits
        interpreter.push(ObjectPointer(self * arg));
        return true;
    }

    bool Primitives::divide(Interpreter &interpreter, Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
//TODO limits
        interpreter.push(ObjectPointer(self / arg));
        return true;
    }

    bool Primitives::remainder(pimii::Interpreter &interpreter, pimii::Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
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
        if (self.isSmallInt()) {
            interpreter.push(ObjectPointer(0));
            return true;
        } else if (self.isObject()) {
            interpreter.push(ObjectPointer(self.size() -
                                           self.type()[TypeSystem::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt()));
            return true;
        } else if (self.isBuffer()) {
            interpreter.push(ObjectPointer(self.byteSize()));
            return true;
        }

        return false;
    }

    bool Primitives::value(Interpreter &interpreter, Offset argumentCount) {
        ObjectPointer blockContext = interpreter.stackValue(argumentCount);
        // This primitive can only handle BlockContexts
        if (!blockContext.isObject() ||
            blockContext.type() != interpreter.getSystem().getTypeSystem().blockContextType) {
            return false;
        }

        SmallInteger blockArgumentCount = blockContext[Interpreter::CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD].smallInt();
        if (blockArgumentCount != argumentCount) {
            return false;
        }

        interpreter.getActiveContext().transferFieldsTo(
                interpreter.getStackBasePointer() + interpreter.getStackPointer() - argumentCount, blockContext,
                Interpreter::CONTEXT_FIXED_SIZE, argumentCount);

        interpreter.pop(argumentCount + 1);

        blockContext[Interpreter::CONTEXT_IP_FIELD] =
                blockContext[Interpreter::CONTEXT_INITIAL_IP_FIELD].smallInt();
        blockContext[Interpreter::CONTEXT_SP_FIELD] = argumentCount;
        blockContext[Interpreter::CONTEXT_CALLER_FIELD] = interpreter.getActiveContext();

        interpreter.newActiveContext(blockContext);

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