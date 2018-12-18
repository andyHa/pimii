//
// Created by Andreas Haufler on 25.11.18.
//

#include <iostream>
#include "SymbolTable.h"
#include "../common/Looping.h"

namespace pimii {

    SymbolTable::SymbolTable(MemoryManager& mm) : mm(mm), symbolType(Nil::NIL), symbolTable(
            mm.makeRootObject(SIZE, Nil::NIL)) {
        symbolTable[FIELD_TALLY] = 0;
        symbolTable[FIELD_TABLE] = ObjectPointer(mm.makeObject(512, Nil::NIL));
    }

    ObjectPointer SymbolTable::lookup(const std::string_view& name) {
        std::hash<std::string_view> hasher;
        auto hash = SmallIntegers::toSafeSmallInteger(hasher(name));
        ObjectPointer table = symbolTable[FIELD_TABLE];

        for (Looping loop = Looping(table.size(), hash); loop.hasNext(); loop.next()) {
            if (table[loop()] == Nil::NIL) {
                table[loop()] = mm.makeString(name, symbolType);
                SmallInteger newSize = symbolTable[FIELD_TALLY].smallInt() + 1;
                symbolTable[FIELD_TALLY] = newSize;
                if (newSize > table.size() * 0.75) {
                    grow(table);
                }
                return table[loop()];
            } else if (table[loop()].compare(name.data(), SmallIntegers::toSmallInteger(name.size() + 1)) == 0) {
                return table[loop()];
            }
        }

        throw std::runtime_error("Failed to lookup a symbol in the symbol table!");
    }

    void SymbolTable::grow(ObjectPointer table) {
        ObjectPointer newTable = mm.makeObject(table.size() + 256, table.type());
        symbolTable[FIELD_TABLE] = newTable;
        for (SmallInteger i = 0; i < table.size(); i++) {
            if (table[i] != Nil::NIL) {
                reInsert(newTable, table[i]);
            }
        }
    }

    void SymbolTable::reInsert(ObjectPointer table, ObjectPointer symbol) {
        SmallInteger hash = symbol.hashString();

        for (Looping loop = Looping(table.size(), hash); loop.hasNext(); loop.next()) {
            if (table[loop()] == Nil::NIL) {
                table[loop()] = symbol;
                return;
            }
        }

        throw std::runtime_error("Failed to re-insert a symbol in the symbol table!");
    }

    void SymbolTable::installTypes(ObjectPointer symbolTableType, ObjectPointer arrayType, ObjectPointer symbolType) {
        this->symbolType = symbolType;
        symbolTable.type(symbolTableType);

        ObjectPointer table = symbolTable[FIELD_TABLE];
        table.type(arrayType);

        for (SmallInteger i = 0; i < table.size(); i++) {
            if (table[i] != Nil::NIL) {
                table[i].type(symbolType);
            }
        }
    }

}
