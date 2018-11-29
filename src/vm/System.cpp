//
// Created by Andreas Haufler on 25.11.18.
//

#include "System.h"
#include "Nil.h"

namespace pimii {

    System::System()
            : mm(), symbols(mm), dictionary(mm), types(mm, symbols, dictionary),
              trueValue(mm.allocObject(0, Nil::NIL)),
              falseValue(mm.allocObject(0, Nil::NIL)),
              specialSelectors(mm.allocObject(NUMBER_OF_SPECIAL_SELECTORS, types.arrayType)) {

        ObjectPointer symbolTableType = types.makeType(types.objectType, "SymbolTable", 0);
        symbols.installTypes(symbolTableType, types.arrayType, types.symbolType);

        ObjectPointer systemDictionaryType = types.makeType(types.objectType, "SystemDictionary", 0);
        dictionary.installTypes(systemDictionaryType, types.arrayType, types.associationType);

        // Setup booleans
        ObjectPointer booleanType = types.makeType(types.objectType, "Boolean", 0);
        ObjectPointer trueType = types.makeType(booleanType, "True", 0);
        trueValue.getObject()->type = trueType;
        dictionary.atPut(symbols.lookup("true"), trueValue);
        ObjectPointer falseType = types.makeType(booleanType, "False", 0);
        falseValue.getObject()->type = falseType;
        dictionary.atPut(symbols.lookup("false"), falseValue);

        dictionary.atPut(symbols.lookup("nil"), Nil::NIL);

        // These special selectors will first call their assigned primitive (in Primitives.h) and only
        // send the selector if the primitive rejected execution.
        specialSelectors->fields[0] = symbols.lookup("==");
        specialSelectors->fields[1] = symbols.lookup("<");
        specialSelectors->fields[2] = symbols.lookup("<=");
        specialSelectors->fields[3] = symbols.lookup(">");
        specialSelectors->fields[4] = symbols.lookup(">=");
        specialSelectors->fields[5] = symbols.lookup("+");
        specialSelectors->fields[6] = symbols.lookup("-");
        specialSelectors->fields[7] = symbols.lookup("*");
        specialSelectors->fields[8] = symbols.lookup("/");
        specialSelectors->fields[9] = symbols.lookup("%");
        specialSelectors->fields[10] = symbols.lookup("basicNew");
        specialSelectors->fields[11] = symbols.lookup("basicNew:");
        specialSelectors->fields[12] = symbols.lookup("class");
        specialSelectors->fields[13] = symbols.lookup("blockCopy");
        specialSelectors->fields[14] = symbols.lookup("value");
        specialSelectors->fields[15] = symbols.lookup("value:");
        specialSelectors->fields[16] = symbols.lookup("value:value:");
        specialSelectors->fields[17] = symbols.lookup("value:value:value:");
        specialSelectors->fields[18] = symbols.lookup("values:");
        specialSelectors->fields[19] = symbols.lookup("perform:");
        specialSelectors->fields[20] = symbols.lookup("perform:with:");
        specialSelectors->fields[21] = symbols.lookup("perform:with:and:");
        specialSelectors->fields[22] = symbols.lookup("perform:with:and:and:");
        specialSelectors->fields[23] = symbols.lookup("perform:withArray:");

        // From now on, these selectors are sent to the receiver (and might still invoke a primitive)
        // but might also be overwritten by a class.
        specialSelectors->fields[24] = symbols.lookup("hash");
        specialSelectors->fields[25] = symbols.lookup("size");
        specialSelectors->fields[26] = symbols.lookup("at:");
        specialSelectors->fields[27] = symbols.lookup("at:put:");
        specialSelectors->fields[28] = symbols.lookup("asSymbol");
        specialSelectors->fields[29] = symbols.lookup("asString");
        specialSelectors->fields[30] = symbols.lookup("do:");
        specialSelectors->fields[31] = symbols.lookup("each:");
        specialSelectors->fields[32] = symbols.lookup("ifTrue:");
        specialSelectors->fields[33] = symbols.lookup("ifFalse:");
        specialSelectors->fields[34] = symbols.lookup("ifTrue:otherwise:");
        specialSelectors->fields[35] = symbols.lookup("whileTrue:");
    }

    ObjectPointer System::getType(ObjectPointer obj) {
        if (obj == Nil::NIL) {
            return types.nilType;
        }

        switch (obj.getObjectPointerType()) {
            case OBJECT:
                return obj.getObject()->type;
            case BYTES:
                return obj.getBytes()->type;
            case WORDS:
                return obj.getWords()->type;
            case SMALL_INT:
                return types.smallIntType;
        }
    }

    MemoryManager &System::getMemoryManager() {
        return mm;
    }

    TypeSystem &System::getTypeSystem() {
        return types;
    }

    SymbolTable &System::getSymbolTable() {
        return symbols;
    }

    SystemDictionary &System::getSystemDictionary() {
        return dictionary;
    }

    ObjectPointer System::getSpecialSelector(Offset index) {
        return specialSelectors->fields[index];
    }

    ObjectPointer System::getSpecialSelectors() {
        return ObjectPointer(specialSelectors);
    }

    int System::getSpecialSelectorIndex(const std::string &name) {
        ObjectPointer symbol = symbols.lookup(name);
        for (Offset index = 0; index <= NUMBER_OF_SPECIAL_SELECTORS; index++) {
            if (symbol == specialSelectors->fields[index]) {
                return index;
            }
        }

        return -1;
    }
}
