//
// Created by Andreas Haufler on 25.11.18.
//

#include <iostream>
#include "System.h"
#include "Primitives.h"
#include "../compiler/Methods.h"

namespace pimii {

    System::System()
            : mm(), symbols(mm), dictionary(mm),
              nilType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              metaClassType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              classType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              objectType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              smallIntType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              symbolType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              stringType(mm.makeRootObject(TYPE_SIZE + 1, Nil::NIL)),
              associationType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              arrayType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              byteArrayType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              methodContextType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              blockContextType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              compiledMethodType(mm.makeRootObject(TYPE_SIZE + 1, Nil::NIL)),
              linkType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              processType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              inputEventType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              pointType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              characterType(mm.makeRootObject(TYPE_SIZE + 1, Nil::NIL)),
              trueValue(mm.makeRootObject(0, Nil::NIL)),
              falseValue(mm.makeRootObject(0, Nil::NIL)),
              proc(mm.makeRootObject(PROCESSOR_SIZE, Nil::NIL)),
              specialSelectorArray(mm.makeRootObject(LAST_PREFERRED_PRIMITIVE_INDEX + 15, Nil::NIL)) {


        // Create "MetaClass class"
        auto metaClassClassType = ObjectPointer(mm.makeObject(TYPE_SIZE, Nil::NIL));
        metaClassClassType[TYPE_FIELD_NAME] = symbols.lookup("MetaClass class");
        metaClassClassType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = TYPE_SIZE;

        // Create "MetaClass"
        metaClassType.type(metaClassClassType);
        metaClassType[TYPE_FIELD_NAME] = symbols.lookup("MetaClass");
        metaClassType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = TYPE_SIZE;
        dictionary.atPut(metaClassType[TYPE_FIELD_NAME], metaClassType);

        // Set type of "MetaClass class" to "MetaClass"
        metaClassClassType.type(metaClassType);

        // Create "Object class"
        auto objectClassClassType = mm.makeObject(TYPE_SIZE, metaClassType);
        objectClassClassType[TYPE_FIELD_NAME] = symbols.lookup("Object class");
        objectClassClassType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = TYPE_SIZE;

        // Create "Object"
        objectType.type(objectClassClassType);
        objectType[TYPE_FIELD_NAME] = symbols.lookup("Object");
        objectType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = 0;
        dictionary.atPut(objectType[TYPE_FIELD_NAME], objectType);

        // Create "Behaviour"
        auto behaviourType = makeType(objectType, "Behaviour", TYPE_SIZE, TYPE_SIZE);

        // Make "Behaviour" the superclass of "MetaClass"
        metaClassType[TYPE_FIELD_SUPERTYPE] = behaviourType;
        metaClassClassType[TYPE_FIELD_SUPERTYPE] = behaviourType.type();

        // Create "Class"
        completeType(classType, behaviourType, "Class", TYPE_SIZE, TYPE_SIZE);

        // Make "Class" the superclass of "Object class"
        objectClassClassType[TYPE_FIELD_SUPERTYPE] = classType;

        // Create "Nil"
        completeType(nilType, objectType, "Nil", TYPE_SIZE, 0);

        // Create "SmallInt"
        completeType(smallIntType, objectType, "SmallInteger", TYPE_SIZE, 0);

        // Create "Symbol"
        completeType(symbolType, objectType, "Symbol", TYPE_SIZE, 0);

        // Create "String"
        completeType(stringType, objectType, "String", TYPE_SIZE + 1, 0);
        stringType[STRING_TYPE_EMPTY_STRING_FIELD] = mm.makeBuffer(1, stringType);

        // Create "Association"
        completeType(associationType, objectType, "Association", TYPE_SIZE, System::ASSOCIATION_SIZE);

        // Create "CompiledMethod"
        completeType(compiledMethodType, objectType, "CompiledMethod", TYPE_SIZE + 1, COMPILED_METHOD_SIZE);

        // Create "MethodContext" and "BlockContext"
        completeType(blockContextType, objectType, "BlockContext", TYPE_SIZE, CONTEXT_FIXED_SIZE);
        completeType(methodContextType, objectType, "MethodContext", TYPE_SIZE, CONTEXT_FIXED_SIZE);

        // Create "Array"
        completeType(arrayType, objectType, "Array", TYPE_SIZE, 0);
        completeType(byteArrayType, objectType, "ByteArray", TYPE_SIZE, 0);

        // Create "Link"
        completeType(linkType, objectType, "Link", TYPE_SIZE, 2);

        // Create "Process"
        completeType(processType, objectType, "Process", TYPE_SIZE, 1);

        // Create "InputEvent"
        completeType(inputEventType, objectType, "InputEvent", TYPE_SIZE, 5);

        // Create "Point"
        completeType(pointType, objectType, "Point", TYPE_SIZE, 2);

        // Create "Character"
        completeType(characterType, objectType, "Character", TYPE_SIZE + 1, 1);
        characterType[TYPE_SIZE] = mm.makeRootObject(256, arrayType);
        for (SmallInteger i = 0; i <= 255; i++) {
            ObjectPointer character = mm.makeRootObject(1, characterType);
            character[0] = i;
            characterType[TYPE_SIZE][i] = character;
        }

        ObjectPointer symbolTableType = makeType(objectType, "SymbolTable", 2, TYPE_SIZE);
        symbols.installTypes(symbolTableType, arrayType, symbolType);
        dictionary.atPut(symbols.lookup("Symbols"), symbolTable().getSymbolTable());

        ObjectPointer systemDictionaryType = makeType(objectType, "IdentityDictionary", 2,
                                                      TYPE_SIZE);
        dictionary.installTypes(systemDictionaryType, arrayType, associationType);
        dictionary.atPut(symbols.lookup("SmallTalk"), dictionary.getDictionary());

        // Setup booleans
        ObjectPointer trueType = makeType(objectType, "True", 0, TYPE_SIZE);
        trueValue.type(trueType);
        dictionary.atPut(symbols.lookup("true"), trueValue);
        ObjectPointer falseType = makeType(objectType, "False", 0, TYPE_SIZE);
        falseValue.type(falseType);
        dictionary.atPut(symbols.lookup("false"), falseValue);

        dictionary.atPut(symbols.lookup("nil"), Nil::NIL);

        // These special selectors will first call their assigned primitive (in Primitives.h) and only
        // send the selector if the primitive rejected execution.
        specialSelectorArray[PRIMITIVE_EQUALITY] = symbols.lookup("==");
        specialSelectorArray[PRIMITIVE_LESS_THAN] = symbols.lookup("<");
        specialSelectorArray[PRIMITIVE_LESS_THAN_OR_EQUAL] = symbols.lookup("<=");
        specialSelectorArray[PRIMITIVE_GREATER_THAN] = symbols.lookup(">");
        specialSelectorArray[PRIMITIVE_GREATER_THAN_OR_EQUAL] = symbols.lookup(">=");
        specialSelectorArray[PRIMITIVE_ADD] = symbols.lookup("+");
        specialSelectorArray[PRIMITIVE_SUBTRACT] = symbols.lookup("-");
        specialSelectorArray[PRIMITIVE_MULTIPLY] = symbols.lookup("*");
        specialSelectorArray[PRIMITIVE_DIVIDE] = symbols.lookup("//");
        specialSelectorArray[PRIMITIVE_REMAINDER] = symbols.lookup("%");
        specialSelectorArray[PRIMITIVE_BIT_AND] = symbols.lookup("bitAnd:");
        specialSelectorArray[PRIMITIVE_BIT_OR] = symbols.lookup("bitOr:");
        specialSelectorArray[PRIMITIVE_BIT_INVERT] = symbols.lookup("bitInvert");
        specialSelectorArray[PRIMITIVE_SHIFT_LEFT] = symbols.lookup("<<");
        specialSelectorArray[PRIMITIVE_SHIFT_RIGHT] = symbols.lookup(">>");
        specialSelectorArray[PRIMITIVE_BASIC_NEW] = symbols.lookup("basicNew");
        specialSelectorArray[PRIMITIVE_BASIC_NEW_WITH] = symbols.lookup("basicNew:");
        specialSelectorArray[PRIMITIVE_BASIC_ALLOC_WITH] = symbols.lookup("basicAlloc:");
        specialSelectorArray[PRIMITIVE_BYTE_AT] = symbols.lookup("byteAt:");
        specialSelectorArray[PRIMITIVE_BYTE_AT_PUT] = symbols.lookup("byteAtPut:");
        specialSelectorArray[PRIMITIVE_TRANSFER_BYTES] = symbols.lookup(
                "transferBytesTo:index:destIndex:length:");
        specialSelectorArray[PRIMITIVE_COMPARE_BYTES] = symbols.lookup("compareBytes:");
        specialSelectorArray[PRIMITIVE_HASH_BYTES] = symbols.lookup("hashBytes");
        specialSelectorArray[PRIMITIVE_CLASS] = symbols.lookup("class");
        specialSelectorArray[PRIMITIVE_VALUE_NO_ARG] = symbols.lookup("value");
        specialSelectorArray[PRIMITIVE_VALUE_ONE_ARG] = symbols.lookup("value:");
        specialSelectorArray[PRIMITIVE_VALUE_TWO_ARGS] = symbols.lookup("value:value:");
        specialSelectorArray[PRIMITIVE_VALUE_THREE_ARGS] = symbols.lookup("value:value:value:");
        specialSelectorArray[PRIMITIVE_VALUE_N_ARGS] = symbols.lookup("withArgs:");
        specialSelectorArray[PRIMITIVE_PERFORM_NO_ARG] = symbols.lookup("perform:");
        specialSelectorArray[PRIMITIVE_PERFORM_ONE_ARG] = symbols.lookup("perform:with:");
        specialSelectorArray[PRIMITIVE_VALUE_TWO_ARGS] = symbols.lookup("perform:with:and:");
        specialSelectorArray[PRIMITIVE_VALUE_THREE_ARGS] = symbols.lookup("perform:with:and:and:");
        specialSelectorArray[PRIMITIVE_PERFORM_N_ARGS] = symbols.lookup("perform:withArgs:");

        // From now on, these selectors are sent to the receiver (and might still invoke a primitive)
        // but might also be overwritten by a class.
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 1] = symbols.lookup("id");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 2] = symbols.lookup("size");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 3] = symbols.lookup("at:");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 4] = symbols.lookup("at:put:");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 5] = symbols.lookup("asString");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 6] = symbols.lookup("/");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 7] = symbols.lookup("&");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 8] = symbols.lookup("|");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 9] = symbols.lookup("and:");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 10] = symbols.lookup("or:");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 11] = symbols.lookup("to:do");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 12] = symbols.lookup("do:");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 13] = symbols.lookup("collect:");
        specialSelectorArray[LAST_PREFERRED_PRIMITIVE_INDEX + 14] = symbols.lookup("reject:");

        specialSelectorArray.type(arrayType);
        compiledMethodType[COMPILED_METHOD_TYPE_FIELD_SPECIAL_SELECTORS] = specialSelectorArray;


        ObjectPointer processSchedulerType = makeType(objectType, "ProcessScheduler", System::PROCESSOR_SIZE,
                                                      TYPE_SIZE);
        proc.type(processSchedulerType);
        dictionary.atPut(symbols.lookup("Processor"), proc);

        ObjectPointer semaphoreType = makeType(objectType, "Semaphore", System::SEMAPHORE_SIZE, TYPE_SIZE);
        ObjectPointer timerSemaphore = mm.makeObject(System::SEMAPHORE_SIZE, semaphoreType);
        timerSemaphore[SEMAPHORE_FIELD_EXCESS_SIGNALS] = 0;
        proc[System::PROCESSOR_FIELD_TIMER_SEMAPHORE] = timerSemaphore;
        dictionary.atPut(symbols.lookup("TimerSemaphore"), timerSemaphore);

        ObjectPointer inputSemaphore = mm.makeObject(System::SEMAPHORE_SIZE, semaphoreType);
        inputSemaphore[SEMAPHORE_FIELD_EXCESS_SIGNALS] = 0;
        proc[System::PROCESSOR_FIELD_INPUT_SEMAPHORE] = inputSemaphore;
        dictionary.atPut(symbols.lookup("InputSemaphore"), inputSemaphore);
    }


    ObjectPointer System::makeType(ObjectPointer parent, const std::string& name, SmallInteger effectiveFixedFields,
                                   SmallInteger effectiveFixedClassFields) {
        ObjectPointer metaType = mm.makeObject(TYPE_SIZE, metaClassType);
        ObjectPointer type = mm.makeObject(effectiveFixedClassFields, ObjectPointer(metaType));
        metaType[TYPE_FIELD_NAME] = symbols.lookup(name + " class");
        metaType[TYPE_FIELD_SUPERTYPE] = parent.type();
        metaType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = effectiveFixedClassFields;
        type[TYPE_FIELD_NAME] = symbols.lookup(name);
        type[TYPE_FIELD_SUPERTYPE] = parent;
        type[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = effectiveFixedFields;

        dictionary.atPut(type[TYPE_FIELD_NAME], ObjectPointer(type));
        return ObjectPointer(type);
    }

    void System::completeType(ObjectPointer type, ObjectPointer parent, const std::string& name,
                              SmallInteger effectiveFixedClassFields, SmallInteger effectiveFixedFields) {
        ObjectPointer metaType = mm.makeObject(TYPE_SIZE, metaClassType);
        type.type(metaType);

        metaType[TYPE_FIELD_NAME] = symbols.lookup(name + " class");
        metaType[TYPE_FIELD_SUPERTYPE] = parent.type();
        metaType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = effectiveFixedClassFields;
        type[TYPE_FIELD_NAME] = symbols.lookup(name);
        type[TYPE_FIELD_SUPERTYPE] = parent;
        type[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = effectiveFixedFields;

        dictionary.atPut(type[TYPE_FIELD_NAME], ObjectPointer(type));
    }

    SmallInteger System::specialSelectorIndex(const std::string& name) {
        ObjectPointer symbol = symbols.lookup(name);
        for (SmallInteger index = 0; index < specialSelectorArray.size(); index++) {
            if (symbol == specialSelectorArray[index]) {
                return index;
            }
        }

        return -1;
    }

    bool System::is(ObjectPointer instance, ObjectPointer expectedType) {
        if (expectedType.type() != metaClassType) {
            //TODO
            //   return false;
        }
        ObjectPointer instanceType = type(instance);
        while (instanceType != Nil::NIL && instanceType != expectedType) {
            instanceType = instanceType[TYPE_FIELD_SUPERTYPE];
        }

        return instanceType == expectedType;
    }

}
