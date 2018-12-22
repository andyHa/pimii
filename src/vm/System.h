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

        static InputEvent m(SmallInteger key) {
            return {KEY, key, 0, 0, 0};
        }


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

        ObjectPointer trueValue;
        ObjectPointer falseValue;
        ObjectPointer proc;

        ObjectPointer specialSelectorArray;

        bool notifyTimerSemaphore;
        bool notifyInputSemaphore;
        std::mutex inputEventsMutex;
        std::deque<InputEvent> inputEvents;
        std::condition_variable interruptReceived;


        static constexpr SmallInteger NUMBER_OF_SPECIAL_SELECTORS = 35;

        void
        completeType(ObjectPointer type, ObjectPointer superType, const std::string& name,
                     SmallInteger effectiveFixedFields);

    public:
        static inline const SmallInteger TYPE_FIELD_SUPERTYPE = 0;
        static inline const SmallInteger TYPE_FIELD_NAME = 1;
        static inline const SmallInteger TYPE_FIELD_NUMBER_OF_FIXED_FIELDS = 2;
        static inline const SmallInteger TYPE_FIELD_FIELD_NAMES = 3;
        static inline const SmallInteger TYPE_FIELD_TALLY = 4;
        static inline const SmallInteger TYPE_FIELD_SELECTORS = 5;
        static inline const SmallInteger TYPE_FIELD_METHODS = 6;
        static inline const SmallInteger TYPE_SIZE = 7;

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
            std::lock_guard<std::mutex> lock(inputEventsMutex);

            inputEvents.push_back(event);
            if (!notifyInputSemaphore) {
                notifyInputSemaphore = true;
                interruptReceived.notify_one();
            }
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
