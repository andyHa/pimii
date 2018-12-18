//
// Created by Andreas Haufler on 26.11.18.
//

#include <iostream>
#include "Primitives.h"

namespace pimii {

    bool Primitives::equality(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        interpreter.push(interpreter.pop() == interpreter.pop() ? sys.valueTrue() : sys.valueFalse());
        return true;
    }

    bool Primitives::lessThan(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();

        interpreter.push(self < arg ? sys.valueTrue() : sys.valueFalse());
        return true;
    }

    bool Primitives::lessThanOrEqual(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();

        interpreter.push(self <= arg ? sys.valueTrue() : sys.valueFalse());
        return true;
    }

    bool Primitives::greaterThan(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();

        interpreter.push(self > arg ? sys.valueTrue() : sys.valueFalse());
        return true;
    }

    bool Primitives::greaterThanOrEqual(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();

        interpreter.push(self >= arg ? sys.valueTrue() : sys.valueFalse());
        return true;
    }

    bool Primitives::add(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
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

    bool Primitives::subtract(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
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

    bool Primitives::multiply(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
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

    bool Primitives::divide(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
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

    bool Primitives::remainder(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
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

    bool Primitives::basicNew(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer type = interpreter.pop();
        //ensure type
        ObjectPointer result = sys.memoryManager().makeObject(
                type[System::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt(), type);

        interpreter.push(result);

        return true;
    }

    bool Primitives::basicNewWith(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        SmallInteger size = interpreter.pop().smallInt();
        ObjectPointer type = interpreter.pop();
        //ensure type
        ObjectPointer result = sys.memoryManager().makeObject(
                type[System::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() + size, type);

        interpreter.push(result);

        return true;
    }

    bool Primitives::clazz(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        interpreter.push(sys.type(interpreter.pop()));

        return true;
    }

    bool Primitives::id(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        interpreter.push(ObjectPointer::forSmallInt(interpreter.pop().hash()));
        return true;
    }

    bool Primitives::size(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer self = interpreter.pop();
        if (self.isSmallInt()) {
            interpreter.push(ObjectPointer::forSmallInt(0));
            return true;
        } else if (self.isObject()) {
            interpreter.push(ObjectPointer::forSmallInt(self.size() -
                                                        self.type()[System::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt()));
            return true;
        } else if (self.isBuffer()) {
            interpreter.push(ObjectPointer::forSmallInt(self.byteSize()));
            return true;
        }

        return false;
    }

    bool Primitives::value(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        ObjectPointer blockContext = interpreter.stackValue(argumentCount);
        // This primitive can only handle BlockContexts
        if (!blockContext.isObject() ||
            blockContext.type() != sys.typeBlockContext()) {
            return false;
        }

        SmallInteger blockArgumentCount = blockContext[Interpreter::CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD].smallInt();
        if (blockArgumentCount != argumentCount) {
            return false;
        }

        interpreter.currentActiveContext().transferFieldsTo(
                interpreter.basePointer() + interpreter.stackPointer() - argumentCount, blockContext,
                Interpreter::CONTEXT_FIXED_SIZE, argumentCount);

        interpreter.pop(argumentCount + 1);

        blockContext[Interpreter::CONTEXT_IP_FIELD] =
                blockContext[Interpreter::CONTEXT_INITIAL_IP_FIELD].smallInt();
        blockContext[Interpreter::CONTEXT_SP_FIELD] = argumentCount;
        blockContext[Interpreter::CONTEXT_CALLER_FIELD] = interpreter.currentActiveContext();

        interpreter.newActiveContext(blockContext);

        return true;
    }

    bool Primitives::valueWith(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        return false;
    }

    bool Primitives::perform(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        return false;
    }

    bool Primitives::performWith(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        return false;
    }

    bool Primitives::at(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        return false;
    }

    bool Primitives::atPut(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        return false;
    }

    bool Primitives::asSymbol(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer self = interpreter.pop();
        if (!sys.is(self, sys.typeString())) {
            interpreter.unPop(1);
            return false;
        }

        interpreter.push(sys.symbolTable().lookup(self.stringView()));
        return true;
    }

    bool Primitives::asString(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer self = interpreter.pop();
        if (!sys.is(self, sys.typeSymbol())) {
            interpreter.unPop(1);
            return false;
        }

        ObjectPointer result = sys.memoryManager().makeBuffer(self.byteSize(), sys.typeString());
        self.transferBytesTo(0, result, 0, self.byteSize());

        interpreter.push(result);

        return true;
    }

    bool Primitives::concat(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        ObjectPointer arg = interpreter.pop();
        ObjectPointer self = interpreter.pop();

        if (arg == Nil::NIL) {
            interpreter.push(self);
            return true;
        }

        if (!sys.is(arg, sys.typeString())) {
            interpreter.unPop(2);
            return false;
        }

        if (!sys.is(self, sys.typeString())) {
            interpreter.unPop(2);
            return false;
        }

        ObjectPointer result = sys.memoryManager().makeString(
                std::string(self.stringView()) + std::string(arg.stringView()),
                sys.typeString());

        interpreter.push(result);

        return true;
    }

    bool Primitives::sysOut(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        ObjectPointer arg = interpreter.pop();

        if (!sys.is(arg, sys.typeString())) {
            interpreter.unPop(1);
            return false;
        }

        std::cout << arg.stringView() << std::endl;
        return true;
    }

    bool Primitives::fork(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer blockContext = interpreter.stackValue(argumentCount);
        // This primitive can only handle BlockContexts
        if (!blockContext.isObject() ||
            blockContext.type() != sys.typeBlockContext()) {
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

        ObjectPointer process = sys.memoryManager().makeObject(System::PROCESS_SIZE, sys.typeProcess());
        process[System::PROCESS_FIELD_CONTEXT] = blockContext;
        process[System::PROCESS_FIELD_TIME] = 0;

        interpreter.pushBack(process, sys.processor(), System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                             System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
        interpreter.pushBack(sys.processor()[System::PROCESSOR_FIELD_ACTIVE_PROCESS],
                             sys.processor(), System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                             System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
        interpreter.requestContextSwitch();

        return true;
    }

    bool Primitives::signal(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer semaphore = interpreter.stackValue(argumentCount);
//        if (!semaphore.isObject() ||
//            semaphore.type() != sys.getTypeSystem().semaphoreType) {
//            return false;
//        }

        //    interpreter.pop();
        interpreter.signalSemaphore(semaphore);
        return true;
    }

    bool Primitives::wait(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer semaphore = interpreter.stackValue(argumentCount);
//        if (!semaphore.isObject() ||
//            semaphore.type() != sys.getTypeSystem().semaphoreType) {
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

        interpreter.pushBack(sys.processor()[System::PROCESSOR_FIELD_ACTIVE_PROCESS], semaphore,
                             System::SEMAPHORE_FIELD_FIRST_WAITING_PROCESS,
                             System::SEMAPHORE_FIELD_LAST_WAITING_PROCESS);

        interpreter.requestContextSwitch();
        return true;
    }

}