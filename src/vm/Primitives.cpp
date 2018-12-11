//
// Created by Andreas Haufler on 26.11.18.
//

#include <iostream>
#include "Primitives.h"

namespace pimii {

    bool Primitives::equality(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        interpreter.push(interpreter.pop() == interpreter.pop() ? interpreter.getSystem().trueValue
                                                                : interpreter.getSystem().falseValue);
        return true;
    }

    bool Primitives::lessThan(Interpreter& interpreter, Offset argumentCount) {
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

    bool Primitives::lessThanOrEqual(pimii::Interpreter& interpreter, pimii::Offset argumentCount) {
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

    bool Primitives::greaterThan(pimii::Interpreter& interpreter, pimii::Offset argumentCount) {
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

    bool Primitives::greaterThanOrEqual(pimii::Interpreter& interpreter, pimii::Offset argumentCount) {
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

    bool Primitives::add(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
        //TODO limits

        interpreter.push(ObjectPointer::forSmallInt(self + arg));
        return true;
    }

    bool Primitives::subtract(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
//TODO limits
        interpreter.push(ObjectPointer::forSmallInt(self - arg));
        return true;
    }

    bool Primitives::multiply(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
//TODO limits
        interpreter.push(ObjectPointer::forSmallInt(self * arg));
        return true;
    }

    bool Primitives::divide(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
//TODO limits
        interpreter.push(ObjectPointer::forSmallInt(self / arg));
        return true;
    }

    bool Primitives::remainder(pimii::Interpreter& interpreter, pimii::Offset argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
//TODO limits
        interpreter.push(ObjectPointer::forSmallInt(self % arg));
        return true;
    }

    bool Primitives::basicNew(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer type = interpreter.pop();
        //ensure type
        ObjectPointer result = interpreter.getSystem().getMemoryManager().makeObject(
                type[TypeSystem::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt(), type);

        interpreter.push(result);

        return true;
    }

    bool Primitives::basicNewWith(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        SmallInteger size = interpreter.pop().smallInt();
        ObjectPointer type = interpreter.pop();
        //ensure type
        ObjectPointer result = interpreter.getSystem().getMemoryManager().makeObject(
                type[TypeSystem::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() + size, type);

        interpreter.push(result);

        return true;
    }

    bool Primitives::clazz(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        interpreter.push(interpreter.getSystem().getType(interpreter.pop()));

        return true;
    }

    bool Primitives::id(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        interpreter.push(ObjectPointer::forSmallInt(interpreter.pop().hash()));
        return true;
    }

    bool Primitives::size(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer self = interpreter.pop();
        if (self.isSmallInt()) {
            interpreter.push(ObjectPointer::forSmallInt(0));
            return true;
        } else if (self.isObject()) {
            interpreter.push(ObjectPointer::forSmallInt(self.size() -
                                                        self.type()[TypeSystem::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt()));
            return true;
        } else if (self.isBuffer()) {
            interpreter.push(ObjectPointer::forSmallInt(self.byteSize()));
            return true;
        }

        return false;
    }

    bool Primitives::value(Interpreter& interpreter, Offset argumentCount) {
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

    bool Primitives::valueWith(Interpreter& interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::perform(Interpreter& interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::performWith(Interpreter& interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::at(Interpreter& interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::atPut(Interpreter& interpreter, Offset argumentCount) {
        return false;
    }

    bool Primitives::asSymbol(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer self = interpreter.pop();
        if (!interpreter.getSystem().is(self, interpreter.getSystem().getTypeSystem().stringType)) {
            interpreter.unPop(1);
            return false;
        }

        interpreter.push(interpreter.getSystem().getSymbolTable().lookup(self.stringView()));
        return true;
    }

    bool Primitives::asString(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer self = interpreter.pop();
        if (!interpreter.getSystem().is(self, interpreter.getSystem().getTypeSystem().symbolType)) {
            interpreter.unPop(1);
            return false;
        }

        ObjectPointer result = interpreter.getSystem().getMemoryManager().makeBuffer(self.byteSize(),
                                                                                     interpreter.getSystem().getTypeSystem().stringType);
        self.transferBytesTo(0, result, 0, self.byteSize());

        interpreter.push(result);

        return true;
    }

    bool Primitives::concat(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        ObjectPointer arg = interpreter.pop();
        ObjectPointer self = interpreter.pop();

        if (arg == Nil::NIL) {
            interpreter.push(self);
            return true;
        }

        if (!interpreter.getSystem().is(arg, interpreter.getSystem().getTypeSystem().stringType)) {
            interpreter.unPop(2);
            return false;
        }

        if (!interpreter.getSystem().is(self, interpreter.getSystem().getTypeSystem().stringType)) {
            interpreter.unPop(2);
            return false;
        }

        ObjectPointer result = interpreter.getSystem().getMemoryManager().makeString(
                std::string(self.stringView()) + std::string(arg.stringView()),
                interpreter.getSystem().getTypeSystem().stringType);

        interpreter.push(result);

        return true;
    }

    bool Primitives::sysOut(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        ObjectPointer arg = interpreter.pop();
        interpreter.pop();

        if (!interpreter.getSystem().is(arg, interpreter.getSystem().getTypeSystem().stringType)) {
            interpreter.unPop(2);
            return false;
        }

        std::cout << arg.stringView() << std::endl;
        return true;
    }

    bool Primitives::fork(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer blockContext = interpreter.stackValue(argumentCount);
        // This primitive can only handle BlockContexts
        if (!blockContext.isObject() ||
            blockContext.type() != interpreter.getSystem().getTypeSystem().blockContextType) {
            return false;
        }

        SmallInteger blockArgumentCount = blockContext[Interpreter::CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD].smallInt();
        if (blockArgumentCount != 0) {
            return false;
        }

        interpreter.pop(argumentCount);

        blockContext[Interpreter::CONTEXT_IP_FIELD] =
                blockContext[Interpreter::CONTEXT_INITIAL_IP_FIELD].smallInt();
        blockContext[Interpreter::CONTEXT_SP_FIELD] = argumentCount;
        blockContext[Interpreter::CONTEXT_CALLER_FIELD] = Nil::NIL;
        // TODO maybe clone HOME_CONTEXT and maybe even the block-context itself(?)

        ObjectPointer process = interpreter.getSystem().getMemoryManager().makeObject(System::PROCESS_SIZE,
                                                                                      interpreter.getSystem().getTypeSystem().processType);
        process[System::PROCESS_FIELD_CONTEXT] = blockContext;

        interpreter.pushBack(process, interpreter.getSystem().processor, System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                             System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
        interpreter.pushBack(interpreter.getSystem().processor[System::PROCESSOR_FIELD_ACTIVE_PROCESS],
                             interpreter.getSystem().processor, System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                             System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
        interpreter.contextSwitchExpected = true;

        return true;
    }

    bool Primitives::signal(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer semaphore = interpreter.stackValue(argumentCount);
//        if (!semaphore.isObject() ||
//            semaphore.type() != interpreter.getSystem().getTypeSystem().semaphoreType) {
//            return false;
//        }

        //    interpreter.pop();
        interpreter.signalSemaphore(semaphore);
        return true;
    }

    bool Primitives::wait(Interpreter& interpreter, Offset argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer semaphore = interpreter.stackValue(argumentCount);
//        if (!semaphore.isObject() ||
//            semaphore.type() != interpreter.getSystem().getTypeSystem().semaphoreType) {
//            return false;
//        }

        //  interpreter.pop();

        if (semaphore[System::SEMAPHORE_FIELD_EXCESS_SIGNALS].isSmallInt() &&
            semaphore[System::SEMAPHORE_FIELD_EXCESS_SIGNALS].smallInt() > 0) {
            semaphore[System::SEMAPHORE_FIELD_EXCESS_SIGNALS] =
                    semaphore[System::SEMAPHORE_FIELD_EXCESS_SIGNALS].smallInt() - 1;
            std::cout << semaphore[System::SEMAPHORE_FIELD_EXCESS_SIGNALS].smallInt() << std::endl;
            return true;
        }

        interpreter.pushBack(interpreter.getSystem().processor[System::PROCESSOR_FIELD_ACTIVE_PROCESS], semaphore,
                             System::SEMAPHORE_FIELD_FIRST_WAITING_PROCESS,
                             System::SEMAPHORE_FIELD_LAST_WAITING_PROCESS);

        interpreter.contextSwitchExpected = true;
        return true;
    }

}