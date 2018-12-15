//
// Created by Andreas Haufler on 25.11.18.
//

#include <iostream>
#include "SymbolTable.h"
#include "Looping.h"

namespace pimii {

    const SmallInteger SymbolTable::FIELD_TALLY = 0;
    const SmallInteger SymbolTable::FIELD_TABLE = 1;
    const SmallInteger SymbolTable::SIZE = 2;

    SymbolTable::SymbolTable(MemoryManager &mm) : mm(mm), symbolType(Nil::NIL), symbolTable(
            mm.makeRootObject(SIZE, Nil::NIL)) {
        symbolTable[FIELD_TALLY] = 0;
        symbolTable[FIELD_TABLE] = ObjectPointer(mm.makeObject(512, Nil::NIL));
    }

    ObjectPointer SymbolTable::lookup(const std::string_view &name) {
        std::hash<std::string_view> hasher;
        auto hash = (SmallInteger) hasher(name);
        ObjectPointer table = symbolTable[FIELD_TABLE];

        for (Looping loop = Looping(table.size(), hash); loop.hasNext(); loop.next()) {
            if (table[loop()] == Nil::NIL) {
                //TODO make root string
                table[loop()] = mm.makeString(name, symbolType);
                SmallInteger newSize = symbolTable[FIELD_TALLY].smallInt() + 1;
                symbolTable[FIELD_TALLY] = newSize;
                if (newSize > table.size() * 0.75) {
                    grow(table);
                }
                return table[loop()];
            } else if (table[loop()].compare(name.data(), name.size() + 1) == 0) {
                return table[loop()];
            }
        }

        //TODO horrible error
        return Nil::NIL;
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

        // TODO horrible error
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
