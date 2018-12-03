//
// Created by Andreas Haufler on 25.11.18.
//

#include <iostream>
#include "System.h"
#include "Primitives.h"
#include "Methods.h"

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
        const_cast<ObjectPointer *>(&trueValue)->type(trueType);
        dictionary.atPut(symbols.lookup("true"), trueValue);
        ObjectPointer falseType = types.makeType(booleanType, "False", 0);
        const_cast<ObjectPointer *>(&falseValue)->type(falseType);
        dictionary.atPut(symbols.lookup("false"), falseValue);

        dictionary.atPut(symbols.lookup("nil"), Nil::NIL);

        // These special selectors will first call their assigned primitive (in Primitives.h) and only
        // send the selector if the primitive rejected execution.
        specialSelectors[Primitives::PRIMITIVE_EQUALITY] = symbols.lookup("==");
        specialSelectors[1] = symbols.lookup("<");
        specialSelectors[2] = symbols.lookup("<=");
        specialSelectors[3] = symbols.lookup(">");
        specialSelectors[4] = symbols.lookup(">=");
        specialSelectors[5] = symbols.lookup("+");
        specialSelectors[6] = symbols.lookup("-");
        specialSelectors[7] = symbols.lookup("*");
        specialSelectors[8] = symbols.lookup("/");
        specialSelectors[9] = symbols.lookup("%");
        specialSelectors[10] = symbols.lookup("basicNew");
        specialSelectors[11] = symbols.lookup("basicNew:");
        specialSelectors[12] = symbols.lookup("class");
        specialSelectors[13] = symbols.lookup("value");
        specialSelectors[14] = symbols.lookup("value:");
        specialSelectors[15] = symbols.lookup("value:value:");
        specialSelectors[16] = symbols.lookup("value:value:value:");
        specialSelectors[17] = symbols.lookup("withArgs:");
        specialSelectors[18] = symbols.lookup("perform:");
        specialSelectors[19] = symbols.lookup("perform:with:");
        specialSelectors[20] = symbols.lookup("perform:with:and:");
        specialSelectors[21] = symbols.lookup("perform:with:and:and:");
        specialSelectors[22] = symbols.lookup("perform:withArgs:");

        // From now on, these selectors are sent to the receiver (and might still invoke a primitive)
        // but might also be overwritten by a class.
        specialSelectors[23] = symbols.lookup("hash");
        specialSelectors[24] = symbols.lookup("size");
        specialSelectors[25] = symbols.lookup("at:");
        specialSelectors[26] = symbols.lookup("at:put:");
        specialSelectors[27] = symbols.lookup("asSymbol");
        specialSelectors[28] = symbols.lookup("asString");
        specialSelectors[29] = symbols.lookup("do:");
        specialSelectors[30] = symbols.lookup("each:");
        specialSelectors[31] = symbols.lookup("ifTrue:");
        specialSelectors[32] = symbols.lookup("ifFalse:");
        specialSelectors[33] = symbols.lookup("ifTrue:otherwise:");
        specialSelectors[34] = symbols.lookup("whileTrue:");
    }

    ObjectPointer System::getType(ObjectPointer obj) {
        if (obj == Nil::NIL) {
            return types.nilType;
        }

        if (obj.isBuffer() || obj.isObject()) {
            return obj.type();
        }

        if (obj.isSmallInt()) {
            return types.smallIntType;
        }

        //TODO float;
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
        return specialSelectors[index];
    }

    ObjectPointer System::getSpecialSelectors() {
        return ObjectPointer(specialSelectors);
    }

    int System::getSpecialSelectorIndex(const std::string &name) {
        ObjectPointer symbol = symbols.lookup(name);
        debug(symbol);
        for (Offset index = 0; index < NUMBER_OF_SPECIAL_SELECTORS; index++) {
            debug(specialSelectors[index]);
            if (symbol == specialSelectors[index]) {
                return index;
            }
        }

        return -1;
    }

    std::string System::info(ObjectPointer obj) {
        if (obj == Nil::NIL) {
            return "nil";
        }

        if (obj.isObject()) {
            if (obj.type().isObject() && obj.type() != Nil::NIL &&
                obj.type()[TypeSystem::TYPE_FIELD_NAME].isBuffer()) {
                return "(" +
                       std::string(obj.type()[TypeSystem::TYPE_FIELD_NAME].stringView()) +
                       ")";
            } else {
                return "?";
            }
        } else if (obj.isSmallInt()) {
            return std::string(std::to_string(obj.smallInt()));
        } else if (obj.isBuffer()) {
            if (obj.type() == types.stringType) {
                return std::string(obj.stringView());
            };
            if (obj.type() == types.symbolType) {
                return "#" + std::string(obj.stringView());
            };
            return "Bytes: " + std::to_string(obj.byteSize());
        }

        return "??";
    }

    void System::debug(ObjectPointer obj) {
        if (obj == Nil::NIL) {
            std::cout << "nil" << std::endl;
        }
        if (obj.isObject()) {
            if (obj.type() == types.compiledMethodType) {
                debugCompiledMethod(obj);
                return;
            }
        //    std::cout << obj.type()[TypeSystem::TYPE_FIELD_NAME].stringView() << std::endl;
            std::cout << "---------------" << std::endl;
            for (auto i = 0; i < obj.size(); i++) {
                if (obj == Nil::NIL) {
                    std::cout << "nil" << std::endl;
                } else {
                    std::cout << info(obj[i]) << std::endl;
                }

            }
            std::cout << "---------------" << std::endl << std::endl;
        } else {
            std::cout << info(obj) << std::endl;
        }
    }

    void System::debugCompiledMethod(ObjectPointer method) {
//        std::cout << "Method" << std::endl;
//        std::cout << "---------------" << std::endl;
//        for (auto i = Interpreter::COMPILED_METHOD_FIELD_LITERALS_START; i < method.getObject()->size; i++) {
//            std::cout << info(method.getObject()->fields[i]) << std::endl;
//
//        }
//        std::cout << "---------------" << std::endl;
//        ByteBuffer *opcodes = method.getObject()->fields[Interpreter::COMPILED_METHOD_FIELD_OPCODES].getBytes();
//        for (auto i = 0; i < opcodes->size * sizeof(Word) - opcodes->odd; i++) {
//            std::cout << std::to_string(i) << ": " << std::to_string(opcodes->bytes[i] & 0b11111) << " "
//                      << std::to_string((opcodes->bytes[i] & 0b11100000) >> 5) << " - "
//                      << std::to_string(opcodes->bytes[i])
//                      << std::endl;
//        }
//        std::cout << "---------------" << std::endl << std::endl;

    }
}
