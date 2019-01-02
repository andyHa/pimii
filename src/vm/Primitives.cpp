//
// Created by Andreas Haufler on 26.11.18.
//

#include <iostream>
#include <ncurses.h>
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


    bool Primitives::bitAnd(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
        interpreter.push(ObjectPointer::forSmallInt(self & arg));
        return true;
    }

    bool Primitives::bitOr(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
        interpreter.push(ObjectPointer::forSmallInt(self | arg));
        return true;
    }

    bool Primitives::bitInvert(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0 || !interpreter.stackTop().isSmallInt()) {
            return false;
        }

        SmallInteger self = interpreter.pop().smallInt();
        interpreter.push(ObjectPointer::forSmallInt(~self));
        return true;
    }

    bool Primitives::shiftLeft(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
        interpreter.push(ObjectPointer::forSmallInt(self << arg));
        return true;
    }

    bool Primitives::shiftRight(pimii::Interpreter& interpreter, pimii::System& sys,
                                pimii::SmallInteger argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt() ||
            !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger arg = interpreter.pop().smallInt();
        SmallInteger self = interpreter.pop().smallInt();
        interpreter.push(ObjectPointer::forSmallInt(self >> arg));
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
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt()) {
            return false;
        }

        SmallInteger size = interpreter.pop().smallInt();
        ObjectPointer type = interpreter.pop();
        //TODO ensure type
        ObjectPointer result = sys.memoryManager().makeObject(
                type[System::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() + size, type);

        interpreter.push(result);

        return true;
    }


    bool Primitives::basicAllocWith(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt()) {
            return false;
        }

        SmallInteger size = interpreter.pop().smallInt();
        ObjectPointer type = interpreter.pop();
        //TODO ensure type
        //TODO type[System::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] == 0
        ObjectPointer result = sys.memoryManager().makeBuffer(size, type);

        interpreter.push(result);

        return true;
    }

    bool Primitives::byteAt(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1 || !interpreter.stackTop().isSmallInt()) {
            return false;
        }

        SmallInteger index = interpreter.pop().smallInt() - 1;
        ObjectPointer value = interpreter.pop();

        interpreter.push(ObjectPointer::forSmallInt(value.fetchByte(index)));
        return true;
    }

    bool Primitives::byteAtPut(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 2 || !interpreter.stackTop().isSmallInt() || !interpreter.stackValue(1).isSmallInt()) {
            return false;
        }

        SmallInteger byte = interpreter.pop().smallInt();
        SmallInteger index = interpreter.pop().smallInt() - 1;
        if (byte < 0 || byte > 255) {
            throw std::range_error("byte value out of range!");
        }
        ObjectPointer value = interpreter.stackTop();
        value.storeByte(index, (char) byte);

        return true;
    }

    bool Primitives::transferBytes(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 4) {
            return false;
        }

        SmallInteger length = interpreter.pop().smallInt();
        SmallInteger destIndex = interpreter.pop().smallInt() -1;
        SmallInteger index = interpreter.pop().smallInt() - 1;
        ObjectPointer dest = interpreter.pop();
        ObjectPointer self = interpreter.stackTop();

        self.transferBytesTo(index, dest, destIndex, length);

        return true;
    }

    bool Primitives::compareBytes(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        ObjectPointer other = interpreter.pop();
        ObjectPointer self = interpreter.pop();
        interpreter.push(ObjectPointer::forSmallInt(self.compareTo(other)));

        return true;
    }

    bool Primitives::hashBytes(pimii::Interpreter& interpreter, pimii::System& sys,
                               pimii::SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer self = interpreter.pop();
        interpreter.push(ObjectPointer::forSmallInt(self.hash()));

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

        interpreter.push(ObjectPointer::forSmallInt(interpreter.pop().id()));
        return true;
    }

    bool Primitives::size(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer self = interpreter.pop();
        if (self.isSmallInt() || self == Nil::NIL) {
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

    bool Primitives::objectSize(pimii::Interpreter& interpreter, pimii::System& sys,
                                pimii::SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer self = interpreter.pop();
        if (self.isSmallInt() || self == Nil::NIL) {
            interpreter.push(ObjectPointer::forSmallInt(0));
            return true;
        } else if (self.isObject()) {
            interpreter.push(ObjectPointer::forSmallInt(self.size()));
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
        if (blockContext == Nil::NIL || !blockContext.isObject() ||
            blockContext.type() != sys.typeBlockContext()) {
            return false;
        }

        SmallInteger blockArgumentCount = blockContext[System::CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD].smallInt();
        if (blockArgumentCount != argumentCount) {
            return false;
        }

        interpreter.currentActiveContext().transferFieldsTo(
                interpreter.basePointer() + interpreter.stackPointer() - argumentCount, blockContext,
                System::CONTEXT_FIXED_SIZE, argumentCount);

        interpreter.pop(argumentCount + 1);

        blockContext[System::CONTEXT_IP_FIELD] =
                blockContext[System::CONTEXT_INITIAL_IP_FIELD].smallInt();
        blockContext[System::CONTEXT_SP_FIELD] = argumentCount;
        blockContext[System::CONTEXT_CALLER_FIELD] = interpreter.currentActiveContext();

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
        if (argumentCount != 1) {
            return false;
        }

        SmallInteger index = interpreter.pop().smallInt() - 1;
        ObjectPointer self = interpreter.pop();
        interpreter.push(self[self.type()[System::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() + index]);

        return true;
    }

    bool Primitives::atPut(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 2) {
            return false;
        }

        ObjectPointer value = interpreter.pop();
        SmallInteger index = interpreter.pop().smallInt() - 1;
        ObjectPointer self = interpreter.stackTop();
        self[self.type()[System::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() + index] = value;

        return true;
    }

    bool Primitives::transfer(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 4) {
            return false;
        }

        SmallInteger length = interpreter.pop().smallInt();
        SmallInteger destIndex = interpreter.pop().smallInt() - 1;
        SmallInteger index = interpreter.pop().smallInt() - 1;
        ObjectPointer dest = interpreter.pop();
        //TODO ensure value range
        ObjectPointer self = interpreter.stackTop();
        self.transferFieldsTo(self.type()[System::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() + index, dest,
                              dest.type()[System::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() + destIndex, length);

        return true;
    }

    bool Primitives::objectAt(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        SmallInteger index = interpreter.pop().smallInt() - 1;
        ObjectPointer self = interpreter.pop();
        interpreter.push(self[index]);

        return true;
    }

    bool Primitives::objectAtPut(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 2) {
            return false;
        }

        ObjectPointer value = interpreter.pop();
        SmallInteger index = interpreter.pop().smallInt() - 1;
        ObjectPointer self = interpreter.stackTop();
        self[index] = value;

        return true;
    }

    bool Primitives::objectTransfer(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 4) {
            return false;
        }

        SmallInteger length = interpreter.pop().smallInt();
        SmallInteger destIndex = interpreter.pop().smallInt() - 1;
        SmallInteger index = interpreter.pop().smallInt() - 1;
        ObjectPointer dest = interpreter.pop();
        //TODO ensure value range
        ObjectPointer self = interpreter.stackTop();
        self.transferFieldsTo(index, dest, destIndex, length);

        return true;
    }

    bool Primitives::fork(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 1) {
            return false;
        }

        ObjectPointer name = interpreter.pop();
        ObjectPointer blockContext = interpreter.pop();
        // This primitive can only handle BlockContexts
        if (!blockContext.isObject() ||
            blockContext.type() != sys.typeBlockContext()) {
            interpreter.unPop(2);
            return false;
        }

        SmallInteger blockArgumentCount = blockContext[System::CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD].smallInt();
        if (blockArgumentCount != 0) {
            interpreter.unPop(2);
            return false;
        }

        interpreter.pop(argumentCount);

        blockContext[System::CONTEXT_IP_FIELD] =
                blockContext[System::CONTEXT_INITIAL_IP_FIELD].smallInt();
        blockContext[System::CONTEXT_SP_FIELD] = argumentCount;
        blockContext[System::CONTEXT_CALLER_FIELD] = Nil::NIL;
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

    bool Primitives::terminalNextEvent(pimii::Interpreter& interpreter, pimii::System& sys,
                                       pimii::SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer event = sys.popInputEvent();
        interpreter.pop();
        interpreter.push(event);
        return true;
    }

    bool Primitives::terminalSize(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        ObjectPointer point = sys.memoryManager().makeObject(2, sys.typePoint());
        SmallInteger x;
        SmallInteger y;
        getmaxyx(stdscr, y, x);

        point[0] = x;
        point[1] = y;
        interpreter.pop();
        interpreter.push(point);

        return true;
    }

    bool Primitives::terminalShowString(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 3) {
            return false;
        }

        ObjectPointer string = interpreter.pop();
        ObjectPointer colorIndex = interpreter.pop();
        ObjectPointer point = interpreter.pop();

        if (!sys.is(point, sys.typePoint()) || !colorIndex.isSmallInt() || !sys.is(string, sys.typeString())) {
            interpreter.unPop(3);
            return false;
        }

        std::cout <<  string.stringView() << std::endl;
        //mvaddstr(point[1].smallInt(), point[0].smallInt(), string.stringView().data());
        return true;
    }

    bool Primitives::terminalShowCursor(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        return false;
    }

    bool Primitives::terminalHideCursor(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        return false;
    }

    bool Primitives::terminalDraw(Interpreter& interpreter, System& sys, SmallInteger argumentCount) {
        if (argumentCount != 0) {
            return false;
        }

        refresh();
        return true;
    }


}