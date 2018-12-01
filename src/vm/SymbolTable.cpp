//
// Created by Andreas Haufler on 25.11.18.
//

#include <iostream>
#include "SymbolTable.h"
#include "Strings.h"
#include "Nil.h"

namespace pimii {

    const Offset SymbolTable::FIELD_TALLY = 0;
    const Offset SymbolTable::FIELD_TABLE = 1;
    const Offset SymbolTable::SIZE = 2;

    SymbolTable::SymbolTable(MemoryManager &mm) : mm(mm), symbolType(Nil::NIL), symbolTable(
            mm.allocObject(SIZE, Nil::NIL)) {
        symbolTable->fields[FIELD_TALLY] = ObjectPointer(0);
        symbolTable->fields[FIELD_TABLE] = ObjectPointer(mm.allocObject(512, Nil::NIL));
    }

    ObjectPointer SymbolTable::lookup(const std::string &name) {
        Offset hash = Strings::hash(name.c_str());
        Object *table = symbolTable->fields[FIELD_TABLE].getObject();

        Offset index = hash % table->size;
        for (Offset i = index; i < table->size; i++) {
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

    ObjectPointer SymbolTable::tryInsert(Offset index, Object *table, const std::string &name) {
        if (table->fields[index] == Nil::NIL) {
            table->fields[index] = Strings::make(mm, symbolType, name);
            SmallInteger newSize = symbolTable->fields[FIELD_TALLY].getInt() + 1;
            symbolTable->fields[FIELD_TALLY] = ObjectPointer(newSize);
            if (newSize > table->size * 0.75) {
                grow(table);
            }
            return table->fields[index];
        } else if (Strings::areEqual(table->fields[index], name)) {
            return table->fields[index];
        }
        return Nil::NIL;
    }

    void SymbolTable::grow(Object *table) {
        Object *newTable = mm.allocObject(table->size + 256, table->type);
        symbolTable->fields[FIELD_TABLE] = ObjectPointer(newTable);
        for (Offset i = 0; i < table->size; i++) {
            if (table->fields[i] != Nil::NIL) {
                reInsert(newTable, table->fields[i]);
            }
        }

    }

    void SymbolTable::reInsert(Object *table, ObjectPointer symbol) {
        Offset hash = Strings::hash(symbol);

        Offset index = hash % table->size;
        for (Offset i = index; i < table->size; i++) {
            if (table->fields[i] == Nil::NIL) {
                table->fields[i] = symbol;
                return;
            }
        }
        for (Offset i = 0; i < index; i++) {
            if (table->fields[i] == Nil::NIL) {
                table->fields[i] = symbol;
                return;
            }
        }

        // TODO horrible error
    }

    void SymbolTable::installTypes(ObjectPointer symbolTableType, ObjectPointer arrayType, ObjectPointer symbolType) {
        this->symbolType = symbolType;
        symbolTable->type = symbolTableType;

        Object *table = symbolTable->fields[FIELD_TABLE].getObject();
        table->type = arrayType;

        for (Offset i = 0; i < table->size; i++) {
            if (table->fields[i] != Nil::NIL) {
                table->fields[i].getBytes()->type = symbolType;
            }
        }

    }

}
