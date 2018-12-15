//
// Created by Andreas Haufler on 25.11.18.
//

#ifndef MEM_SYMBOLTABLE_H
#define MEM_SYMBOLTABLE_H


#include "ObjectPointer.h"
#include "MemoryManager.h"

namespace pimii {

    class SymbolTable {
        MemoryManager &mm;
        ObjectPointer symbolType;
        ObjectPointer symbolTable;
        static const SmallInteger FIELD_TALLY;
        static const SmallInteger FIELD_TABLE;
        static const SmallInteger SIZE;

        void grow(ObjectPointer table);

        void reInsert(ObjectPointer table, ObjectPointer symbol);

    public:
        SymbolTable(MemoryManager &mm);

        ObjectPointer lookup(const std::string_view &name);

        void installTypes(ObjectPointer symbolTableType, ObjectPointer arrayType, ObjectPointer symbolType);

    };
}


#endif //MEM_SYMBOLTABLE_H
