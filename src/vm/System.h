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

    enum Interrupt : uint8_t {
        IRQ_TIMER = 0
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

        ObjectPointer trueValue;
        ObjectPointer falseValue;
        ObjectPointer proc;

        ObjectPointer specialSelectorArray;
        std::bitset<8> irqs;
        std::condition_variable irq_available;


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
        static constexpr SmallInteger PROCESSOR_FIELD_IRQ_TABLE = 1;
        static constexpr SmallInteger PROCESSOR_FIELD_FIRST_WAITING_PROCESS = 2;
        static constexpr SmallInteger PROCESSOR_FIELD_LAST_WAITING_PROCESS = 3;
        static constexpr SmallInteger PROCESSOR_SIZE = 4;

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
                               SmallInteger effetiveFixedClassFields);

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

        void irq(Interrupt interrupt) {
            irqs[interrupt] = true;
            irq_available.notify_one();
        }

        bool hasIRQ() {
            return irqs.any();
        }

        void clearIRQ() {
            irqs[0] = false;
        }

        bool isIRQ(Interrupt interrupt) {
            return irqs[interrupt];
        }

        std::condition_variable& irgAvailable() {
            return irq_available;
        }
    };

}


#endif //MEM_SYSTEM_H
