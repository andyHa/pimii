//
// Created by Andreas Haufler on 25.11.18.
//

#ifndef MEM_SYSTEM_H
#define MEM_SYSTEM_H

#include <bitset>
#include <mutex>

#include "MemoryManager.h"
#include "TypeSystem.h"
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
        TypeSystem types;
        ObjectPointer specialSelectors;
        std::bitset<8> irqs;

        static constexpr SmallInteger NUMBER_OF_SPECIAL_SELECTORS = 35;
    public:
        static constexpr SmallInteger PROCESSOR_FIELD_ACTIVE_PROCESS = 0;
        static constexpr SmallInteger PROCESSOR_FIELD_IRQ_TABLE = 1;
        static constexpr SmallInteger PROCESSOR_FIELD_FIRST_WAITING_PROCESS = 2;
        static constexpr SmallInteger PROCESSOR_FIELD_LAST_WAITING_PROCESS = 3;
        static constexpr SmallInteger PROCESSOR_SIZE = 4;

        static constexpr SmallInteger PROCESS_FIELD_CONTEXT = 0;
        static constexpr SmallInteger PROCESS_SIZE = 1;

        static constexpr SmallInteger SEMAPHORE_FIELD_EXCESS_SIGNALS = 0;
        static constexpr SmallInteger SEMAPHORE_FIELD_FIRST_WAITING_PROCESS = 1;
        static constexpr SmallInteger SEMAPHORE_FIELD_LAST_WAITING_PROCESS = 2;
        static constexpr SmallInteger SEMAPHORE_SIZE = 3;

        static constexpr SmallInteger LINK_VALUE = 0;
        static constexpr SmallInteger LINK_NEXT = 1;
        static constexpr SmallInteger LINK_SIZE = 2;

        std::condition_variable work_available;
        std::mutex work_lock;

        System();

        MemoryManager &getMemoryManager();

        TypeSystem &getTypeSystem();

        SymbolTable &getSymbolTable();

        SystemDictionary &getSystemDictionary();

        ObjectPointer getSpecialSelector(SmallInteger index);

        ObjectPointer getSpecialSelectors();

        int getSpecialSelectorIndex(const std::string &name);

        const ObjectPointer trueValue;
        const ObjectPointer falseValue;
        const ObjectPointer processor;

        ObjectPointer getType(ObjectPointer obj);

        bool is(ObjectPointer instance, ObjectPointer type);

        ObjectPointer newInstance(ObjectPointer type, SmallInteger extraFields);

        void irq(Interrupt interrupt) {
            irqs[interrupt] = true;
            work_available.notify_one();
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

        std::string info(ObjectPointer obj);

        void debug(ObjectPointer obj);

        void debugCompiledMethod(ObjectPointer method);
    };

}


#endif //MEM_SYSTEM_H
