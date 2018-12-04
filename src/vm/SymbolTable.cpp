//
// Created by Andreas Haufler on 25.11.18.
//

#include <iostream>
#include "SymbolTable.h"

namespace pimii {

    const Offset SymbolTable::FIELD_TALLY = 0;
    const Offset SymbolTable::FIELD_TABLE = 1;
    const Offset SymbolTable::SIZE = 2;

    SymbolTable::SymbolTable(MemoryManager &mm) : mm(mm), symbolType(Nil::NIL), symbolTable(
            mm.makeRootObject(SIZE, Nil::NIL)) {
        symbolTable[FIELD_TALLY] = 0;
        symbolTable[FIELD_TABLE] = ObjectPointer(mm.makeObject(512, Nil::NIL));
    }

    ObjectPointer SymbolTable::lookup(const std::string_view &name) {
        std::hash<std::string_view> hasher;
        Offset hash = (Offset) hasher(name);
        ObjectPointer table = symbolTable[FIELD_TABLE];

        Offset index = hash % table.size();
        for (Offset i = index; i < table.size(); i++) {
            ObjectPointer result = tryInsert(i, table, name);
            if (result != Nil::NIL) {
                return result;
            }
        }
        for (Offset i = 0; i < index; i++) {
            ObjectPointer result = tryInsert(i, table, name);
            if (result != Nil::NIL) {
                return result;
            }
        }

        //TODO horrible error
        return Nil::NIL;
    }

    ObjectPointer SymbolTable::tryInsert(Offset index, ObjectPointer table, const std::string_view &name) {
        if (table[index] == Nil::NIL) {
            //TODO make root string
            table[index] = mm.makeString(name, symbolType);
            SmallInteger newSize = symbolTable[FIELD_TALLY].smallInt() + 1;
            symbolTable[FIELD_TALLY] = newSize;
            if (newSize > table.size() * 0.75) {
                grow(table);
            }
            return table[index];
        } else if (table[index].compare(name.data(), name.size() + 1) == 0) {
            return table[index];
        }
        return Nil::NIL;
    }

    void SymbolTable::grow(ObjectPointer table) {
        ObjectPointer newTable = mm.makeObject(table.size() + 256, table.type());
        symbolTable[FIELD_TABLE] = newTable;
        for (Offset i = 0; i < table.size(); i++) {
            if (table[i] != Nil::NIL) {
                reInsert(newTable, table[i]);
            }
        }

    }

    void SymbolTable::reInsert(ObjectPointer table, ObjectPointer symbol) {
        Offset hash = symbol.hashString();

        Offset index = hash % table.size();
        for (Offset i = index; i < table.size(); i++) {
            if (table[i] == Nil::NIL) {
                table[i] = symbol;
                return;
            }
        }
        for (Offset i = 0; i < index; i++) {
            if (table[i] == Nil::NIL) {
                table[i] = symbol;
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

        for (Offset i = 0; i < table.size(); i++) {
            if (table[i] != Nil::NIL) {
                table[i].type(symbolType);
            }
        }

    }

}
