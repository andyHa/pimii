//
// Created by Andreas Haufler on 25.11.18.
//

#ifndef MEM_SYSTEM_H
#define MEM_SYSTEM_H

#include <bitset>
#include <mutex>

#include "../mem/MemoryManager.h"
#include "SymbolTable.h"
#include "SystemDictionary.h"

namespace pimii {

    enum InputEventType : SmallInteger {
        KEY = 1, MOUSE_MOVE, MOUSE_DOWN = 2, MOUSE_UP = 3
    };

    struct InputEvent {
        SmallInteger type;
        SmallInteger key;
        SmallInteger button;
        SmallInteger col;
        SmallInteger row;

        InputEvent(SmallInteger type, SmallInteger key, SmallInteger button, SmallInteger col, SmallInteger row) : type(
                type), key(key), button(button), col(col), row(row) {}

        static InputEvent keyPressed(SmallInteger key) {
            return {KEY, key, 0, 0, 0};
        }

    };

    enum OutputEventType : SmallInteger {
        SHOW_STRING, SHOW_BOX, MOVE_CURSOR, SHOW_CURSOR, HIDE_CURSOR, DRAW
    };

    struct OutputEvent {
        SmallInteger type;
        SmallInteger col;
        SmallInteger row;
        SmallInteger width;
        SmallInteger height;
        SmallInteger colorIndex;
        std::string string;
    };

    class System {
        MemoryManager mm;
        SymbolTable symbols;
        SystemDictionary dictionary;

        ObjectPointer nilType;
        ObjectPointer metaClassType;
        ObjectPointer classType;
        ObjectPointer objectType;
        ObjectPointer smallIntType;
        ObjectPointer symbolType;
        ObjectPointer stringType;
        ObjectPointer associationType;
        ObjectPointer arrayType;
        ObjectPointer byteArrayType;
        ObjectPointer methodContextType;
        ObjectPointer blockContextType;
        ObjectPointer compiledMethodType;
        ObjectPointer linkType;
        ObjectPointer processType;
        ObjectPointer inputEventType;
        ObjectPointer pointType;
        ObjectPointer characterType;

        ObjectPointer trueValue;
        ObjectPointer falseValue;
        ObjectPointer proc;

        ObjectPointer specialSelectorArray;

        bool notifyTimerSemaphore;
        bool notifyInputSemaphore;
        std::mutex inputEventsMutex;
        std::condition_variable interruptReceived;
        std::deque<InputEvent> inputEvents;
        std::deque<OutputEvent> outputEvents;

        void
        completeType(ObjectPointer type, ObjectPointer superType, const std::string& name,
                     SmallInteger effectiveFixedClassFields, SmallInteger effectiveFixedFields);

    public:
        static constexpr SmallInteger TYPE_FIELD_SUPERTYPE = 0;
        static constexpr SmallInteger TYPE_FIELD_NAME = 1;
        static constexpr SmallInteger TYPE_FIELD_NUMBER_OF_FIXED_FIELDS = 2;
        static constexpr SmallInteger TYPE_FIELD_FIELD_NAMES = 3;
        static constexpr SmallInteger TYPE_FIELD_TALLY = 4;
        static constexpr SmallInteger TYPE_FIELD_SELECTORS = 5;
        static constexpr SmallInteger TYPE_FIELD_METHODS = 6;
        static constexpr SmallInteger TYPE_SIZE = 7;

        static constexpr SmallInteger PROCESSOR_FIELD_ACTIVE_PROCESS = 0;
        static constexpr SmallInteger PROCESSOR_FIELD_TIMER_SEMAPHORE = 1;
        static constexpr SmallInteger PROCESSOR_FIELD_INPUT_SEMAPHORE = 2;
        static constexpr SmallInteger PROCESSOR_FIELD_FIRST_WAITING_PROCESS = 3;
        static constexpr SmallInteger PROCESSOR_FIELD_LAST_WAITING_PROCESS = 4;
        static constexpr SmallInteger PROCESSOR_SIZE = 5;

        static constexpr SmallInteger PROCESS_FIELD_CONTEXT = 0;
        static constexpr SmallInteger PROCESS_FIELD_TIME = 1;
        static constexpr SmallInteger PROCESS_SIZE = 2;

        static constexpr SmallInteger SEMAPHORE_FIELD_EXCESS_SIGNALS = 0;
        static constexpr SmallInteger SEMAPHORE_FIELD_FIRST_WAITING_PROCESS = 1;
        static constexpr SmallInteger SEMAPHORE_FIELD_LAST_WAITING_PROCESS = 2;
        static constexpr SmallInteger SEMAPHORE_SIZE = 3;

        static constexpr SmallInteger LINK_VALUE = 0;
        static constexpr SmallInteger LINK_NEXT = 1;
        static constexpr SmallInteger LINK_SIZE = 2;

        static constexpr SmallInteger COMPILED_METHOD_SIZE = 4;
        static constexpr SmallInteger COMPILED_METHOD_FIELD_HEADER = 0;
        static constexpr SmallInteger COMPILED_METHOD_FIELD_OPCODES = 1;
        static constexpr SmallInteger COMPILED_METHOD_FIELD_OWNER = 2;
        static constexpr SmallInteger COMPILED_METHOD_FIELD_SELECTOR = 3;
        static constexpr SmallInteger COMPILED_METHOD_FIELD_LITERALS_START = 4;
        static constexpr SmallInteger COMPILED_METHOD_TYPE_FIELD_SPECIAL_SELECTORS = TYPE_SIZE;

        static constexpr SmallInteger CONTEXT_FIXED_SIZE = 6;
        static constexpr SmallInteger CONTEXT_SIZE = 32;
        static constexpr SmallInteger CONTEXT_SENDER_FIELD = 0;
        static constexpr SmallInteger CONTEXT_CALLER_FIELD = 0;
        static constexpr SmallInteger CONTEXT_IP_FIELD = 1;
        static constexpr SmallInteger CONTEXT_SP_FIELD = 2;
        static constexpr SmallInteger CONTEXT_METHOD_FIELD = 3;
        static constexpr SmallInteger CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD = 3;
        static constexpr SmallInteger CONTEXT_INITIAL_IP_FIELD = 4;
        static constexpr SmallInteger CONTEXT_HOME_FIELD = 5;
        static constexpr SmallInteger CONTEXT_RECEIVER_FIELD = 5;

        static constexpr SmallInteger DICTIONARY_SIZE = 2;
        static constexpr SmallInteger DICTIONARY_FIELD_TALLY = 0;
        static constexpr SmallInteger DICTIONARY_FIELD_TABLE = 1;

        static constexpr SmallInteger ASSOCIATION_SIZE = 2;
        static constexpr SmallInteger ASSOCIATION_FIELD_KEY = 0;
        static constexpr SmallInteger ASSOCIATION_FIELD_VALUE = 1;

        static constexpr SmallInteger CHARACTER_TYPE_CHARACTERS_FIELD = TYPE_SIZE;
        static constexpr SmallInteger STRING_TYPE_EMPTY_STRING_FIELD = TYPE_SIZE;

        // These are preferred primitives. This list matches the special selectors in System.cpp
        // and these primitives will be invoked in place of an actual method call.
        static constexpr SmallInteger PRIMITIVE_EQUALITY = 0;
        static constexpr SmallInteger PRIMITIVE_LESS_THAN = PRIMITIVE_EQUALITY + 1;
        static constexpr SmallInteger PRIMITIVE_LESS_THAN_OR_EQUAL = PRIMITIVE_LESS_THAN + 1;
        static constexpr SmallInteger PRIMITIVE_GREATER_THAN = PRIMITIVE_LESS_THAN_OR_EQUAL + 1;
        static constexpr SmallInteger PRIMITIVE_GREATER_THAN_OR_EQUAL = PRIMITIVE_GREATER_THAN + 1;

        static constexpr SmallInteger PRIMITIVE_ADD = PRIMITIVE_GREATER_THAN_OR_EQUAL + 1;
        static constexpr SmallInteger PRIMITIVE_SUBTRACT = PRIMITIVE_ADD + 1;
        static constexpr SmallInteger PRIMITIVE_MULTIPLY = PRIMITIVE_SUBTRACT + 1;
        static constexpr SmallInteger PRIMITIVE_DIVIDE = PRIMITIVE_MULTIPLY + 1;
        static constexpr SmallInteger PRIMITIVE_REMAINDER = PRIMITIVE_DIVIDE + 1;
        static constexpr SmallInteger PRIMITIVE_BIT_AND = PRIMITIVE_REMAINDER + 1;
        static constexpr SmallInteger PRIMITIVE_BIT_OR = PRIMITIVE_BIT_AND + 1;
        static constexpr SmallInteger PRIMITIVE_BIT_INVERT = PRIMITIVE_BIT_OR + 1;
        static constexpr SmallInteger PRIMITIVE_SHIFT_LEFT = PRIMITIVE_BIT_INVERT + 1;
        static constexpr SmallInteger PRIMITIVE_SHIFT_RIGHT = PRIMITIVE_SHIFT_LEFT + 1;

        static constexpr SmallInteger PRIMITIVE_BASIC_NEW = PRIMITIVE_SHIFT_RIGHT + 1;
        static constexpr SmallInteger PRIMITIVE_BASIC_NEW_WITH = PRIMITIVE_BASIC_NEW + 1;
        static constexpr SmallInteger PRIMITIVE_BASIC_ALLOC_WITH = PRIMITIVE_BASIC_NEW_WITH + 1;
        static constexpr SmallInteger PRIMITIVE_BYTE_AT = PRIMITIVE_BASIC_ALLOC_WITH + 1;
        static constexpr SmallInteger PRIMITIVE_BYTE_AT_PUT = PRIMITIVE_BYTE_AT + 1;
        static constexpr SmallInteger PRIMITIVE_TRANSFER_BYTES = PRIMITIVE_BYTE_AT_PUT + 1;
        static constexpr SmallInteger PRIMITIVE_COMPARE_BYTES = PRIMITIVE_TRANSFER_BYTES + 1;
        static constexpr SmallInteger PRIMITIVE_HASH_BYTES = PRIMITIVE_COMPARE_BYTES + 1;
        static constexpr SmallInteger PRIMITIVE_CLASS = PRIMITIVE_HASH_BYTES + 1;
        static constexpr SmallInteger PRIMITIVE_VALUE_NO_ARG = PRIMITIVE_CLASS + 1;
        static constexpr SmallInteger PRIMITIVE_VALUE_ONE_ARG = PRIMITIVE_VALUE_NO_ARG + 1;
        static constexpr SmallInteger PRIMITIVE_VALUE_TWO_ARGS = PRIMITIVE_VALUE_ONE_ARG + 1;
        static constexpr SmallInteger PRIMITIVE_VALUE_THREE_ARGS = PRIMITIVE_VALUE_TWO_ARGS + 1;
        static constexpr SmallInteger PRIMITIVE_VALUE_N_ARGS = PRIMITIVE_VALUE_THREE_ARGS + 1;
        static constexpr SmallInteger PRIMITIVE_PERFORM_NO_ARG = PRIMITIVE_VALUE_N_ARGS + 1;
        static constexpr SmallInteger PRIMITIVE_PERFORM_ONE_ARG = PRIMITIVE_PERFORM_NO_ARG + 1;
        static constexpr SmallInteger PRIMITIVE_PERFORM_TWO_ARGS = PRIMITIVE_PERFORM_ONE_ARG + 1;
        static constexpr SmallInteger PRIMITIVE_PERFORM_THREE_ARGS = PRIMITIVE_PERFORM_TWO_ARGS + 1;
        static constexpr SmallInteger PRIMITIVE_PERFORM_N_ARGS = PRIMITIVE_PERFORM_THREE_ARGS + 1;

        static constexpr SmallInteger LAST_PREFERRED_PRIMITIVE_INDEX = PRIMITIVE_PERFORM_N_ARGS;

        // These primitives can be invoked via methods but also be overwritten by classes.
        static constexpr SmallInteger PRIMITIVE_ID = PRIMITIVE_PERFORM_N_ARGS + 1;
        static constexpr SmallInteger PRIMITIVE_SIZE = PRIMITIVE_ID + 1;
        static constexpr SmallInteger PRIMITIVE_OBJECT_SIZE = PRIMITIVE_SIZE + 1;
        static constexpr SmallInteger PRIMITIVE_FORK = PRIMITIVE_OBJECT_SIZE + 1;
        static constexpr SmallInteger PRIMITIVE_WAIT = PRIMITIVE_FORK + 1;
        static constexpr SmallInteger PRIMITIVE_SIGNAL = PRIMITIVE_WAIT + 1;
        static constexpr SmallInteger PRIMITIVE_AT = PRIMITIVE_SIGNAL + 1;
        static constexpr SmallInteger PRIMITIVE_AT_PUT = PRIMITIVE_AT + 1;
        static constexpr SmallInteger PRIMITIVE_TRANSFER = PRIMITIVE_AT_PUT + 1;
        static constexpr SmallInteger PRIMITIVE_TERMINAL_NEXT_EVENT = PRIMITIVE_TRANSFER + 1;
        static constexpr SmallInteger PRIMITIVE_TERMINAL_SIZE = PRIMITIVE_TERMINAL_NEXT_EVENT + 1;
        static constexpr SmallInteger PRIMITIVE_TERMINAL_SHOW_STRING = PRIMITIVE_TERMINAL_SIZE + 1;
        static constexpr SmallInteger PRIMITIVE_TERMINAL_SHOW_BOX = PRIMITIVE_TERMINAL_SHOW_STRING + 1;
        static constexpr SmallInteger PRIMITIVE_TERMINAL_SHOW_CURSOR = PRIMITIVE_TERMINAL_SHOW_BOX + 1;
        static constexpr SmallInteger PRIMITIVE_TERMINAL_HIDE_CURSOR = PRIMITIVE_TERMINAL_SHOW_CURSOR + 1;
        static constexpr SmallInteger PRIMITIVE_TERMINAL_DRAW = PRIMITIVE_TERMINAL_HIDE_CURSOR + 1;


        System();

        ObjectPointer makeType(ObjectPointer parent, const std::string& name, SmallInteger effectiveFixedFields,
                               SmallInteger effectiveFixedClassFields);

        MemoryManager& memoryManager() {
            return mm;
        }

        SymbolTable& symbolTable() {
            return symbols;
        }

        SystemDictionary& systemDictionary() {
            return dictionary;
        }

        ObjectPointer typeSymbol() {
            return symbolType;
        };

        ObjectPointer typeClass() {
            return classType;
        };

        ObjectPointer typeString() {
            return stringType;
        };

        ObjectPointer typeArray() {
            return arrayType;
        };

        ObjectPointer typeByteArray() {
            return byteArrayType;
        };

        ObjectPointer typeMethodContext() {
            return methodContextType;
        };

        ObjectPointer typeBlockContext() {
            return blockContextType;
        };

        ObjectPointer typeCompiledMethod() {
            return compiledMethodType;
        };

        ObjectPointer typeLink() {
            return linkType;
        };

        ObjectPointer typePoint() {
            return pointType;
        };

        ObjectPointer typeProcess() {
            return processType;
        };

        ObjectPointer typeCharacter() {
            return characterType;
        };

        ObjectPointer valueTrue() {
            return trueValue;
        };

        ObjectPointer valueFalse() {
            return falseValue;
        };

        ObjectPointer processor() {
            return proc;
        };

        ObjectPointer specialSelector(SmallInteger index) {
            return specialSelectorArray[index];
        }

        ObjectPointer specialSelectors() {
            return specialSelectorArray;
        }

        SmallInteger specialSelectorIndex(const std::string& name);


        ObjectPointer type(ObjectPointer obj) {
            if (obj == Nil::NIL) {
                return nilType;
            }

            if (obj.isBuffer() || obj.isObject()) {
                return obj.type();
            }

            if (obj.isSmallInt()) {
                return smallIntType;
            }

            throw std::runtime_error("Unknown object.");
        }

        bool is(ObjectPointer instance, ObjectPointer type);

        void fireTimer() {
            if (!notifyTimerSemaphore) {
                notifyTimerSemaphore = true;
                interruptReceived.notify_one();
            }
        }

        void recordInputEvent(InputEvent event) {
            /*   std::lock_guard<std::mutex> lock(inputEventsMutex);

               inputEvents.push_back(event);
               if (!notifyInputSemaphore) {
                   notifyInputSemaphore = true;
                   interruptReceived.notify_one();
               }
               */
        }

        ObjectPointer popInputEvent() {
            std::lock_guard<std::mutex> lock(inputEventsMutex);
            if (inputEvents.empty()) {
                return Nil::NIL;
            }

            InputEvent sourceEvent = inputEvents.front();
            inputEvents.pop_front();

            ObjectPointer result = mm.makeObject(5, inputEventType);
            result[0] = sourceEvent.type;
            result[1] = sourceEvent.key;
            result[2] = sourceEvent.button;
            result[3] = sourceEvent.col;
            result[4] = sourceEvent.row;

            return result;
        }

        bool shouldNotifySemaphore() {
            return notifyTimerSemaphore || notifyInputSemaphore;
        }

        bool shouldNotifyTimerSemaphore() {
            return notifyTimerSemaphore;
        }

        bool shouldNotifyInputSemaphore() {
            return notifyInputSemaphore;
        }

        void clearNotifications() {
            notifyTimerSemaphore = false;
            notifyInputSemaphore = false;
        }

        std::condition_variable& didReceiveInterrupt() {
            return interruptReceived;
        }
    };

}


#endif //MEM_SYSTEM_H
