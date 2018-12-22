//
// Created by Andreas Haufler on 23.11.18.
//

#ifndef MEM_INTERPRETER_H
#define MEM_INTERPRETER_H

#include "System.h"

namespace pimii {


    class Interpreter {
        System& system;
        SmallInteger ip;
        SmallInteger sp;
        ObjectPointer activeContext;
        ObjectPointer homeContext;
        ObjectPointer method;
        ObjectPointer opCodes;
        SmallInteger maxIP;
        SmallInteger temporaryCount;
        ObjectPointer receiver;
        ObjectPointer rootProcess;
        bool contextSwitchExpected;
        std::mutex irq_lock;

        std::chrono::steady_clock::time_point startup;
        std::chrono::steady_clock::time_point lastContextSwitch;
        std::chrono::steady_clock::time_point lastMetrics;
        SmallInteger activeMicros;
        SmallInteger activePercent;
        SmallInteger instuctionsExecuted;
        SmallInteger instuctionsPerSecond;

        uint8_t fetchInstruction();

        void dispatchInstruction(uint8_t opCode);


        void dispatchReturn(uint8_t index);

        void dispatchPush(uint8_t index);
        void dispatchJump(uint8_t code, uint8_t index);

        void storeContextRegisters();

        void fetchContextRegisters();

        ObjectPointer findMethod(ObjectPointer type, ObjectPointer selector);

        ObjectPointer findMethodInType(ObjectPointer type, ObjectPointer selector);

        bool executePrimitive(SmallInteger index, SmallInteger numberOfArguments);


        void performBlockCopy(uint8_t index);

        ObjectPointer sender();

        ObjectPointer caller();

        ObjectPointer temporary(SmallInteger index);

        void temporary(SmallInteger index, ObjectPointer value);

        ObjectPointer literal(SmallInteger index);

        void returnValueTo(ObjectPointer returnValue, ObjectPointer targetContext);

        void send(ObjectPointer selector, SmallInteger numArguments);

        bool isBlockContext(ObjectPointer context);

    public:

        static constexpr SmallInteger COMPILED_METHOD_SIZE = 2;
        static constexpr SmallInteger COMPILED_METHOD_FIELD_HEADER = 0;
        static constexpr SmallInteger COMPILED_METHOD_FIELD_OPCODES = 1;
        static constexpr SmallInteger COMPILED_METHOD_FIELD_LITERALS_START = 2;
        static constexpr SmallInteger COMPILED_METHOD_TYPE_FIELD_SPECIAL_SELECTORS = 6;

        static constexpr SmallInteger CONTEXT_FIXED_SIZE = 6;
        static constexpr SmallInteger CONTEXT_SENDER_FIELD = 0;
        static constexpr SmallInteger CONTEXT_CALLER_FIELD = 0;
        static constexpr SmallInteger CONTEXT_IP_FIELD = 1;
        static constexpr SmallInteger CONTEXT_SP_FIELD = 2;
        static constexpr SmallInteger CONTEXT_METHOD_FIELD = 3;
        static constexpr SmallInteger CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD = 3;
        static constexpr SmallInteger CONTEXT_INITIAL_IP_FIELD = 4;
        static constexpr SmallInteger CONTEXT_HOME_FIELD = 5;
        static constexpr SmallInteger CONTEXT_RECEIVER_FIELD = 5;

        static constexpr uint8_t OP_RETURN = 0;
        static constexpr uint8_t OP_RETURN_RECEIVER_INDEX = 0;
        static constexpr uint8_t OP_RETURN_TRUE_INDEX = 1;
        static constexpr uint8_t OP_RETURN_FALSE_INDEX = 2;
        static constexpr uint8_t OP_RETURN_NIL_INDEX = 3;
        static constexpr uint8_t OP_RETURN_STACK_TOP_TO_SENDER_INDEX = 4;
        static constexpr uint8_t OP_RETURN_STACK_TO_TO_CALLER_INDEX = 5;
        static constexpr uint8_t OP_PUSH_LITERAL_CONSTANT = 2;
        static constexpr uint8_t OP_PUSH_LITERAL_VARIABLE = 3;
        static constexpr uint8_t OP_PUSH_TEMPORARY = 4;
        static constexpr uint8_t OP_PUSH_RECEIVER_FIELD = 5;
        static constexpr uint8_t OP_PUSH = 6;
        static constexpr uint8_t OP_PUSH_RECEIVER_INDEX = 0;
        static constexpr uint8_t OP_PUSH_TRUE_INDEX = 1;
        static constexpr uint8_t OP_PUSH_FALSE_INDEX = 2;
        static constexpr uint8_t OP_PUSH_NIL_INDEX = 3;
        static constexpr uint8_t OP_PUSH_MINUS_ONE_INDEX = 4;
        static constexpr uint8_t OP_PUSH_ZERO_INDEX = 5;
        static constexpr uint8_t OP_PUSH_ONE_INDEX = 6;
        static constexpr uint8_t OP_PUSH_TWO_INDEX = 7;
        static constexpr uint8_t OP_POP_AND_STORE_RECEIVER_FIELD = 7;
        static constexpr uint8_t OP_POP_AND_STORE_IN_TEMPORARY = 8;
        static constexpr uint8_t OP_POP_AND_STORE_IN_LITERAL_VARIABLE = 9;
        static constexpr uint8_t OP_POP = 10;
        static constexpr uint8_t OP_DUPLICAE_STACK_TOP = 11;
        static constexpr uint8_t OP_SEND_LITERAL_SELECTOR_WITH_NO_ARGS = 12;
        static constexpr uint8_t OP_SEND_LITERAL_SELECTOR_WITH_ONE_ARG = 13;
        static constexpr uint8_t OP_SEND_LITERAL_SELECTOR_WITH_TWO_ARGS = 14;
        static constexpr uint8_t OP_SEND_LITERAL_SELECTOR_WITH_N_ARGS = 15;
        static constexpr uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_NO_ARGS = 16;
        static constexpr uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_ONE_ARG = 17;
        static constexpr uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_TWO_ARGS = 18;
        static constexpr uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_N_ARGS = 19;
        static constexpr uint8_t OP_JUMP_ON_TRUE = 20;
        static constexpr uint8_t OP_JUMP_ON_FALSE = 21;
        static constexpr uint8_t OP_JUMP_ALWAYS = 22;
        static constexpr uint8_t OP_JUMP_BACK = 23;
        static constexpr uint8_t OP_BLOCK_COPY = 24;

        static constexpr uint8_t LAST_PREFERRED_PRIMITIVE_SELECTOR = 22;

        explicit Interpreter(System& system);

        ObjectPointer currentActiveContext() {
            return activeContext;
        }


        SmallInteger elapsedMicros();


        void newActiveContext(ObjectPointer context);

        void run(ObjectPointer rootContext);

        void updateMetrics();

        void push(ObjectPointer value) {
            SmallInteger index = basePointer() + (sp++);
            if (index >= activeContext.size()) {
                throw std::overflow_error("stack overflow");
            }
            activeContext[index] = value;
        }


        ObjectPointer pop() {
            if (sp == 0) {
                throw std::underflow_error("stack underflow");
            }
            return activeContext[basePointer() + (--sp)];
        }

        ObjectPointer stackTop() {
            if (sp == 0) {
                throw std::underflow_error("stack underflow");
            }
            return activeContext[basePointer() + (sp - 1)];
        }

        ObjectPointer stackValue(SmallInteger offset) {
            SmallInteger effectiveStackPointer = sp - offset;
            if (effectiveStackPointer <= 0) {
                throw std::underflow_error("stack underflow");
            }
            return activeContext[basePointer() + (effectiveStackPointer - 1)];
        }

        void pop(SmallInteger number) {
            if (sp >= number) {
                sp -= number;
            } else {
                throw std::underflow_error("stack underflow");
            }
        }

        void unPop(SmallInteger number) {
            if (sp + number >= activeContext.size()) {
                throw std::overflow_error("stack overflow");
            }

            sp += number;
        }

        SmallInteger stackPointer() {
            return sp;
        }

        SmallInteger basePointer() {
            return CONTEXT_FIXED_SIZE + temporaryCount;
        }

        ObjectPointer popFront(ObjectPointer list, SmallInteger first, SmallInteger last);

        void pushFront(ObjectPointer value, ObjectPointer list, SmallInteger first, SmallInteger last);

        void pushBack(ObjectPointer value, ObjectPointer list, SmallInteger first, SmallInteger last);

        void signalSemaphore(ObjectPointer semaphore);

        void requestContextSwitch() {
            contextSwitchExpected = true;
        }

        void notifySemaphores();

        void handleContextSwitch();
    };

}


#endif //MEM_INTERPRETER_H
