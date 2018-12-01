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
        static const Offset FIELD_TALLY;
        static const Offset FIELD_TABLE;
        static const Offset SIZE;

        ObjectPointer tryInsert(Offset index, ObjectPointer table, const std::string &name);

        void grow(ObjectPointer table);

        void reInsert(ObjectPointer table, ObjectPointer symbol);

    public:
        SymbolTable(MemoryManager &mm);

        ObjectPointer lookup(const std::string &name);

        void installTypes(ObjectPointer symbolTableType, ObjectPointer arrayType, ObjectPointer symbolType);

    };
}


#endif //MEM_SYMBOLTABLE_H
