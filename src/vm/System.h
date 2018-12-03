//
// Created by Andreas Haufler on 25.11.18.
//

#ifndef MEM_SYSTEM_H
#define MEM_SYSTEM_H

#include "MemoryManager.h"
#include "TypeSystem.h"
#include "SymbolTable.h"
#include "SystemDictionary.h"

namespace pimii {

    class System {
        MemoryManager mm;
        SymbolTable symbols;
        SystemDictionary dictionary;
        TypeSystem types;
        ObjectPointer specialSelectors;

        static const inline Offset NUMBER_OF_SPECIAL_SELECTORS = 35;
    public:
        System();

        MemoryManager &getMemoryManager();

        TypeSystem &getTypeSystem();

        SymbolTable &getSymbolTable();

        SystemDictionary &getSystemDictionary();

        ObjectPointer getSpecialSelector(Offset index);

        ObjectPointer getSpecialSelectors();

        int getSpecialSelectorIndex(const std::string &name);

        const ObjectPointer trueValue;
        const ObjectPointer falseValue;

        ObjectPointer getType(ObjectPointer obj);

        std::string info(ObjectPointer obj);

        void debug(ObjectPointer obj);

        void debugCompiledMethod(ObjectPointer method);
    };

}


#endif //MEM_SYSTEM_H
