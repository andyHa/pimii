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
        Object* symbolTable;
        static const Offset FIELD_TALLY;
        static const Offset FIELD_TABLE;
        static const Offset SIZE;

        ObjectPointer tryInsert(Offset index, Object *table, const std::string &name);
        void grow(Object *table);
        void reInsert(Object *table, ObjectPointer symbol);
    public:
        SymbolTable(MemoryManager &mm);

        ObjectPointer lookup(const std::string &name);

        void installTypes(ObjectPointer symbolTableType, ObjectPointer arrayType, ObjectPointer symbolType);


        ObjectPointer lookupString(ObjectPointer stringObject);
    };
}


#endif //MEM_SYMBOLTABLE_H
