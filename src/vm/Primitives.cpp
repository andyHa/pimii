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
        interpreter.getSystem().debug(ObjectPointer(interpreter.getActiveContext()));
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

        interpreter.getSystem().debug(ObjectPointer(interpreter.getActiveContext()));

        interpreter.transfer(argumentCount,
                             interpreter.getActiveContext(),
                             interpreter.getStackBasePointer() + interpreter.getStackPointer() - argumentCount,
                             block, Interpreter::CONTEXT_FIXED_SIZE);

        interpreter.pop(argumentCount + 1);

        interpreter.getSystem().debug(ObjectPointer(interpreter.getActiveContext()));

        block->fields[Interpreter::CONTEXT_IP_FIELD] = ObjectPointer(
                block->fields[Interpreter::CONTEXT_INITIAL_IP_FIELD].getInt());
        block->fields[Interpreter::CONTEXT_SP_FIELD] = ObjectPointer(argumentCount);
        block->fields[Interpreter::CONTEXT_CALLER_FIELD] = ObjectPointer(interpreter.getActiveContext());
//        interpreter.getSystem().debug(ObjectPointer(block));
        interpreter.newActiveContext(block);
        interpreter.getSystem().debug(ObjectPointer(interpreter.getActiveContext()));
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