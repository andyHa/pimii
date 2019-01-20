//
// Created by Andreas Haufler on 23.11.18.
//

#include <array>
#include <iostream>
#include <sstream>
#include <thread>

#include "Interpreter.h"
#include "Primitives.h"
#include "../common/Looping.h"
#include "../compiler/Methods.h"

namespace pimii {


    Interpreter::Interpreter(System& system) : system(system), contextSwitchExpected(false) {
        startup = std::chrono::steady_clock::now();
        lastMetrics = std::chrono::steady_clock::now();
    }

    void Interpreter::run(ObjectPointer rootContext) {
        lastTimer = std::chrono::steady_clock::now();
        rootProcess = system.memoryManager().makeObject(System::PROCESS_SIZE, system.typeProcess());
        rootProcess[System::PROCESS_FIELD_CONTEXT] = rootContext;
        rootProcess[System::PROCESS_FIELD_TIME] = 0;

        pushBack(rootProcess, system.processor(), System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                 System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
        contextSwitchExpected = true;

        while (true) { //TODO for rootProcess done
            if (!contextSwitchExpected) {
                notifySemaphores();
            }

            if (system.memoryManager().shouldRunRecommendedGC()) {
                ObjectPointer currentProcess = system.processor()[System::PROCESSOR_FIELD_ACTIVE_PROCESS];
                if (currentProcess != Nil::NIL) {
                    storeContextRegisters();
                    currentProcess[System::PROCESS_FIELD_CONTEXT] = activeContext;
                    system.memoryManager().runRecommendedGC();
                    currentProcess = system.processor()[System::PROCESSOR_FIELD_ACTIVE_PROCESS];
                    activeContext = currentProcess[System::PROCESS_FIELD_CONTEXT];
                    fetchContextRegisters();
                }
            }

            if (contextSwitchExpected) {
                handleContextSwitch();
            }

            uint8_t opcode = fetchInstruction();
            dispatchInstruction(opcode);
            instuctionsExecuted++;
        }
    }

    void Interpreter::notifySemaphores() {
        std::chrono::milliseconds delta = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastTimer);

        if (delta.count() > 200) {
            ObjectPointer semaphore = system.processor()[System::PROCESSOR_FIELD_TIMER_SEMAPHORE];
            signalSemaphore(semaphore);
            lastTimer = std::chrono::steady_clock::now();
        } else {
            if (inputAvailable) {
                std::lock_guard lock(inputQueueMutex);
                inputAvailable = false;
                ObjectPointer semaphore = system.processor()[System::PROCESSOR_FIELD_INPUT_SEMAPHORE];
                signalSemaphore(semaphore);
            }
        }
    }

    void Interpreter::handleContextSwitch() {
        SmallInteger elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - lastContextSwitch).count();

        ObjectPointer activeProcess = system.processor()[System::PROCESSOR_FIELD_ACTIVE_PROCESS];
        if (activeProcess != Nil::NIL) {
            activeProcess[System::PROCESS_FIELD_TIME] =
                    activeProcess[System::PROCESS_FIELD_TIME].smallInt() + elapsedTime;
            activeProcess[System::PROCESS_FIELD_CONTEXT] = activeContext;
        }
        system.processor()[System::PROCESSOR_FIELD_ACTIVE_PROCESS] = Nil::NIL;

        ObjectPointer nextProcess = popFront(system.processor(), System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                                             System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
        while (nextProcess == Nil::NIL) {
            /*
            if (activeContext != Nil::NIL && system.memoryManager().shouldIdleGC()) {
                storeContextRegisters();
                activeProcess[System::PROCESS_FIELD_CONTEXT] = activeContext;
                system.memoryManager().idleGC();
                activeProcess = system.processor()[System::PROCESSOR_FIELD_ACTIVE_PROCESS];
                activeContext = activeProcess[System::PROCESS_FIELD_CONTEXT];
                fetchContextRegisters();
            }
             */

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            notifySemaphores();
            nextProcess = popFront(system.processor(), System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                                   System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
        }

        contextSwitchExpected = false;
        lastContextSwitch = std::chrono::steady_clock::now();


        system.processor()[System::PROCESSOR_FIELD_ACTIVE_PROCESS] = nextProcess;

        newActiveContext(nextProcess[System::PROCESS_FIELD_CONTEXT]);
        activeMicros += elapsedTime;
    }


    void Interpreter::dispatchInstruction(uint8_t opCode) {
        uint8_t code = opCode & (uint8_t) 0b11111;
        uint8_t index = opCode >> 5;

        switch (code) {
            case OP_RETURN:
                dispatchReturn(index);
                return;
            case OP_PUSH:
                dispatchPush(index);
                return;
            case OP_JUMP_BACK:
            case OP_JUMP_ALWAYS:
            case OP_JUMP_ON_TRUE:
            case OP_JUMP_ON_FALSE:
                dispatchJump(opCode, index);
                return;
            case OP_BLOCK_COPY:
                performBlockCopy(index);
                return;
        }

        if (index == 0b111) {
            index = fetchInstruction();
        }

        switch (code) {
            case OP_PUSH_LITERAL_CONSTANT:
                push(literal(index));
                return;
            case OP_PUSH_LITERAL_VARIABLE:
                push(literal(index)[System::ASSOCIATION_FIELD_VALUE]);
                return;
            case OP_PUSH_TEMPORARY:
                push(temporary(index));
                return;
            case OP_PUSH_RECEIVER_FIELD:
                push(receiver[index]);
                return;
            case OP_POP_AND_STORE_RECEIVER_FIELD:
                receiver[index] = pop();
                return;
            case OP_POP_AND_STORE_IN_TEMPORARY:
                temporary(index, pop());
                return;
            case OP_POP_AND_STORE_IN_LITERAL_VARIABLE:
                literal(index)[System::ASSOCIATION_FIELD_VALUE] = pop();
                return;
            case OP_POP:
                pop();
                return;
            case OP_DUPLICAE_STACK_TOP:
                push(stackTop());
                return;
            case OP_SEND_LITERAL_SELECTOR_WITH_NO_ARGS:
                send(literal(index), 0);
                return;
            case OP_SEND_LITERAL_SELECTOR_WITH_ONE_ARG:
                send(literal(index), 1);
                return;
            case OP_SEND_LITERAL_SELECTOR_WITH_TWO_ARGS:
                send(literal(index), 2);
                return;
            case OP_SEND_LITERAL_SELECTOR_WITH_N_ARGS:
                send(literal(fetchInstruction()), index);
                return;
            case OP_SEND_SPECIAL_SELECTOR_WITH_NO_ARGS:
                if (index > System::LAST_PREFERRED_PRIMITIVE_INDEX || !executePrimitive(index, 0)) {
                    send(system.specialSelector(index), 0);
                }
                return;
            case OP_SEND_SPECIAL_SELECTOR_WITH_ONE_ARG:
                if (index > System::LAST_PREFERRED_PRIMITIVE_INDEX || !executePrimitive(index, 1)) {
                    send(system.specialSelector(index), 1);
                }
                return;
            case OP_SEND_SPECIAL_SELECTOR_WITH_TWO_ARGS:
                if (index > System::LAST_PREFERRED_PRIMITIVE_INDEX || !executePrimitive(index, 2)) {
                    send(system.specialSelector(index), 2);
                }
                return;
            case OP_SEND_SPECIAL_SELECTOR_WITH_N_ARGS:
                SmallInteger primitiveIndex = fetchInstruction();
                if (primitiveIndex > System::LAST_PREFERRED_PRIMITIVE_INDEX ||
                    !executePrimitive(primitiveIndex, index)) {
                    send(system.specialSelector(primitiveIndex), index);
                }
                return;
        }
    }

    void Interpreter::newActiveContext(ObjectPointer context) {
        if (activeContext != Nil::NIL) {
            storeContextRegisters();
        }

        activeContext = context;

        if (activeContext != Nil::NIL) {
            fetchContextRegisters();
        }
    }

    void Interpreter::storeContextRegisters() {
        activeContext[System::CONTEXT_IP_FIELD] = ip;
        activeContext[System::CONTEXT_SP_FIELD] = sp;
    }

    void Interpreter::fetchContextRegisters() {
        if (isBlockContext(activeContext)) {
            homeContext = activeContext[System::CONTEXT_HOME_FIELD];
            temporaryCount = 0;
        } else {
            homeContext = activeContext;
            temporaryCount = MethodHeader(
                    homeContext[System::CONTEXT_METHOD_FIELD][System::COMPILED_METHOD_FIELD_HEADER].smallInt()).temporaries();
        }

        receiver = homeContext[System::CONTEXT_RECEIVER_FIELD];
        method = homeContext[System::CONTEXT_METHOD_FIELD];
        opCodes = method[System::COMPILED_METHOD_FIELD_OPCODES];
        maxIP = opCodes.byteSize();
        ip = (SmallInteger) activeContext[System::CONTEXT_IP_FIELD].smallInt();
        sp = (SmallInteger) activeContext[System::CONTEXT_SP_FIELD].smallInt();
    }

    uint8_t Interpreter::fetchInstruction() {
        if (ip >= maxIP) {
            return OP_RETURN;
        }
        return (uint8_t) opCodes.fetchByte(ip++);
    }

    ObjectPointer Interpreter::sender() {
        return homeContext[System::CONTEXT_SENDER_FIELD];
    }

    ObjectPointer Interpreter::caller() {
        return activeContext[System::CONTEXT_SENDER_FIELD];
    }

    ObjectPointer Interpreter::temporary(SmallInteger index) {
        return homeContext[System::CONTEXT_FIXED_SIZE + index];
    }

    void Interpreter::temporary(SmallInteger index, ObjectPointer value) {
        homeContext[System::CONTEXT_FIXED_SIZE + index] = value;
    }

    ObjectPointer Interpreter::literal(SmallInteger index) {
        return method[System::COMPILED_METHOD_FIELD_LITERALS_START + index];
    }

    void Interpreter::returnValueTo(ObjectPointer returnValue, ObjectPointer targetContext) {
        newActiveContext(targetContext);
        if (activeContext != Nil::NIL) {
            push(returnValue);
        } else {
            contextSwitchExpected = true;
        }
    }

    ObjectPointer Interpreter::findMethod(ObjectPointer type, ObjectPointer selector) {
        while (type != Nil::NIL) {
            if (type[System::TYPE_FIELD_SELECTORS] != Nil::NIL) {
                ObjectPointer method = findMethodInType(type, selector);
                if (method != Nil::NIL) {
                    return method;
                }
            }
            type = type[System::TYPE_FIELD_SUPERTYPE];
        }


        return Nil::NIL;
    }

    ObjectPointer Interpreter::findMethodInType(ObjectPointer type, ObjectPointer selector) {
        ObjectPointer selectors = type[System::TYPE_FIELD_SELECTORS];
        for (Looping loop = Looping(selectors.size(), selector.id()); loop.hasNext(); loop.next()) {
            if (selectors[loop()] == selector) {
                return type[System::TYPE_FIELD_METHODS][loop()];
            } else if (selectors[loop()] == Nil::NIL) {
                return Nil::NIL;
            }
        }

        return Nil::NIL;
    }

    void Interpreter::send(ObjectPointer selector, SmallInteger numArguments) {
        ObjectPointer newReceiver = stackValue(numArguments);
        ObjectPointer type = system.type(newReceiver);
        ObjectPointer newMethod = findMethod(type, selector);

        if (newMethod == Nil::NIL) {
            std::stringstream errorMessage;
            errorMessage << "Unknown method " << selector.stringView() << " in "
                         << system.type(newReceiver)[System::TYPE_FIELD_NAME].stringView();
            throw std::runtime_error(errorMessage.str());
        }

        MethodHeader header(newMethod[System::COMPILED_METHOD_FIELD_HEADER].smallInt());

        if (header.methodType() == CompiledMethodType::MT_PRIMITIVE) {
            if (executePrimitive(header.primitiveIndex(), numArguments)) {
                return;
            }
        } else if (header.methodType() == CompiledMethodType::MT_RETURN_FIELD) {
            //TODO range check
            pop(); //assert numArguments == 0
            push(newReceiver[header.fieldIndex()]);
            return;
        } else if (header.methodType() == CompiledMethodType::MT_POP_AND_STORE_FIELD) {
            //TODO range check
            //pop(); //assert numArguments == 1
            newReceiver[header.fieldIndex()] = pop();
            //push(newReceiver);
            return;
        }

        //std::cout << "Sending " << selector.stringView() << std::endl;

        // TODO check if method is non-nil and has bytecodes
        ObjectPointer newContext = system.memoryManager().makeObject(System::CONTEXT_SIZE, system.typeMethodContext());
        newContext[System::CONTEXT_SENDER_FIELD] = activeContext;
        newContext[System::CONTEXT_IP_FIELD] = 0;
        newContext[System::CONTEXT_SP_FIELD] = header.temporaries();
        newContext[System::CONTEXT_METHOD_FIELD] = newMethod;
        newContext[System::CONTEXT_RECEIVER_FIELD] = newReceiver;

        //TODO ensure proper stack limits
        //TODO ensure numArguments <= numTeporaries
        if (numArguments > 0) {
            activeContext.transferFieldsTo(basePointer() + (sp - numArguments), newContext,
                                           System::CONTEXT_FIXED_SIZE, numArguments);
        }

        pop(numArguments + 1);

        newActiveContext(newContext);
    }

    bool Interpreter::executePrimitive(SmallInteger index, SmallInteger numberOfArguments) {
        SmallInteger backupSP = sp;
        try {
            bool result = Primitives::executePrimitive(index, *this, system, numberOfArguments);
            if (result) {
                return true;
            }
        } catch (std::exception& e) {
#ifdef DEBUG
            std::cout << "Primitive: " << index << " failed: " << e.what() << std::endl;
#endif
        }

        // Restore original stack pointer...
        sp = backupSP;
        return false;
    }

    bool Interpreter::isBlockContext(ObjectPointer context) {
        return context[System::CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD].isSmallInt();
    }

    void Interpreter::dispatchJump(uint8_t code, uint8_t index) {
        int delta = index * 255 + fetchInstruction();

        switch (code) {
            case OP_JUMP_BACK:
                pop();
                if (delta > ip) {
                    ip = 0;
                } else {
                    ip = ip - delta;
                }
                return;
            case OP_JUMP_ALWAYS:
                ip = ip + delta;
                return;
            case OP_JUMP_ON_TRUE:
                if (stackTop() == system.valueTrue()) {
                    ip = ip + delta;
                } else {
                    pop();
                }
                return;
            case OP_JUMP_ON_FALSE:
                if (stackTop() == system.valueFalse()) {
                    ip = ip + delta;
                } else {
                    pop();
                }
                return;
            default:
                throw std::runtime_error("Invalid jump instruction");
        }
    }

    void Interpreter::performBlockCopy(uint8_t blockArgumentCount) {
        ObjectPointer newContext = system.memoryManager().makeObject(
                activeContext.size(), system.typeBlockContext());

        newContext[System::CONTEXT_INITIAL_IP_FIELD] = ip + 2;
        newContext[System::CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD] = blockArgumentCount;
        newContext[System::CONTEXT_HOME_FIELD] = homeContext;
        push(ObjectPointer(newContext));
    }

    void Interpreter::signalSemaphore(ObjectPointer semaphore) {
        if (semaphore == Nil::NIL) {
            return;
        }

        ObjectPointer firstWaitingProcess = popFront(semaphore, System::SEMAPHORE_FIELD_FIRST_WAITING_PROCESS,
                                                     System::SEMAPHORE_FIELD_LAST_WAITING_PROCESS);

        if (firstWaitingProcess == Nil::NIL) {
            semaphore[System::SEMAPHORE_FIELD_EXCESS_SIGNALS] =
                    semaphore[System::SEMAPHORE_FIELD_EXCESS_SIGNALS].smallInt() + 1;
            return;
        }

        pushFront(firstWaitingProcess, system.processor(), System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                  System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
        if (system.processor()[System::PROCESSOR_FIELD_ACTIVE_PROCESS] != Nil::NIL) {
            pushBack(system.processor()[System::PROCESSOR_FIELD_ACTIVE_PROCESS], system.processor(),
                     System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                     System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
        }
        contextSwitchExpected = true;
    }

    ObjectPointer Interpreter::popFront(ObjectPointer list, SmallInteger first, SmallInteger last) {
        ObjectPointer next = list[first];
        if (next == Nil::NIL) {
            return Nil::NIL;
        }

        list[first] = next[System::LINK_NEXT];
        if (list[first] == Nil::NIL) {
            list[last] = Nil::NIL;
        }

        return next[System::LINK_VALUE];
    }

    void Interpreter::pushFront(ObjectPointer value, ObjectPointer list, SmallInteger first, SmallInteger last) {
        ObjectPointer link = system.memoryManager().makeObject(System::LINK_SIZE, system.typeLink());
        link[System::LINK_VALUE] = value;
        ObjectPointer head = list[first];
        if (head == Nil::NIL) {
            list[last] = link;
        } else {
            link[System::LINK_NEXT] = head;
        }
        list[first] = link;
    }

    void Interpreter::pushBack(ObjectPointer value, ObjectPointer list, SmallInteger first, SmallInteger last) {
        ObjectPointer link = system.memoryManager().makeObject(System::LINK_SIZE, system.typeLink());
        link[System::LINK_VALUE] = value;
        ObjectPointer tail = list[last];
        if (tail == Nil::NIL) {
            list[first] = link;
        } else {
            tail[System::LINK_NEXT] = link;
        }
        list[last] = link;
    }

    SmallInteger Interpreter::elapsedMicros() {
        long long int micros = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - startup).count();

        if (micros > SmallIntegers::maxSmallInt()) {
            startup = std::chrono::steady_clock::now();
            return 0;
        }

        return static_cast<SmallInteger>(micros);
    }

    void Interpreter::updateMetrics() {
        long long int micros = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - lastMetrics).count();
        if (micros == 0) {
            activeMicros = 0;
        } else {
            activePercent = 100 * activeMicros / micros;
        }

        instuctionsPerSecond = instuctionsExecuted;

        //    std::cout << "Metrics: " << activePercent << "%, " << instuctionsPerSecond << " op/s, GC: "
        //              << system.memoryManager().gcMicros() << "us" << std::endl;
        instuctionsExecuted = 0;
        activeMicros = 0;
        lastMetrics = std::chrono::steady_clock::now();
//        system.memoryManager().resetGCCounter();

    }

    void Interpreter::dispatchReturn(uint8_t index) {
        switch (index) {
            case OP_RETURN_RECEIVER_INDEX:
                returnValueTo(receiver, sender());
                return;
            case OP_RETURN_TRUE_INDEX:
                returnValueTo(system.valueTrue(), sender());
                return;
            case OP_RETURN_FALSE_INDEX:
                returnValueTo(system.valueFalse(), sender());
                return;
            case OP_RETURN_NIL_INDEX:
                returnValueTo(Nil::NIL, sender());
                return;
            case OP_RETURN_STACK_TOP_TO_SENDER_INDEX:
                returnValueTo(pop(), sender());
                return;
            case OP_RETURN_STACK_TO_TO_CALLER_INDEX:
                returnValueTo(pop(), caller());
                return;
            default:
                throw std::runtime_error("Invalid return instruction");
        }
    }

    void Interpreter::dispatchPush(uint8_t index) {
        switch (index) {
            case OP_PUSH_RECEIVER_INDEX:
                push(receiver);
                return;
            case OP_PUSH_TRUE_INDEX:
                push(system.valueTrue());
                return;
            case OP_PUSH_FALSE_INDEX:
                push(system.valueFalse());
                return;
            case OP_PUSH_NIL_INDEX:
                push(Nil::NIL);
                return;
            case OP_PUSH_MINUS_ONE_INDEX:
                push(ObjectPointer::forSmallInt(-1));
                return;
            case OP_PUSH_ZERO_INDEX:
                push(ObjectPointer::forSmallInt(0));
                return;
            case OP_PUSH_ONE_INDEX:
                push(ObjectPointer::forSmallInt(1));
                return;
            case OP_PUSH_TWO_INDEX:
                push(ObjectPointer::forSmallInt(2));
                return;
            default:
                throw std::runtime_error("Invalid push instruction");
        }

    }

    void Interpreter::queueInput(std::string input) {
        std::lock_guard lock(inputQueueMutex);
        queuedInputs.emplace_back(input);
        inputAvailable = true;
    }

    ObjectPointer Interpreter::nextQueuedInput() {
        std::lock_guard lock(inputQueueMutex);
        if (queuedInputs.empty()) {
            return Nil::NIL;
        }

        std::string result = queuedInputs.front();
        queuedInputs.pop_front();

        return system.memoryManager().makeString(result, system.typeString());
    }


}