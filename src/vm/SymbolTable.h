//
// Created by Andreas Haufler on 25.11.18.
//

#ifndef MEM_SYMBOLTABLE_H
#define MEM_SYMBOLTABLE_H


#include "../common/ObjectPointer.h"
#include "../mem/MemoryManager.h"

namespace pimii {

    class SymbolTable {
        MemoryManager &mm;
        ObjectPointer symbolType;
        ObjectPointer symbolTable;
        static constexpr SmallInteger FIELD_TALLY = 0;
        static constexpr SmallInteger FIELD_TABLE = 1;
        static constexpr SmallInteger SIZE = 2;

        void grow(ObjectPointer table);

        void reInsert(ObjectPointer table, ObjectPointer symbol);

    public:
        explicit SymbolTable(MemoryManager &mm);

        ObjectPointer lookup(const std::string_view &name);

        void installTypes(ObjectPointer symbolTableType, ObjectPointer arrayType, ObjectPointer symbolType);

        ObjectPointer getSymbolTable() {
            return symbolTable;
        }
    };
}


#endif //MEM_SYMBOLTABLE_H
