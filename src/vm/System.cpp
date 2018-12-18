//
// Created by Andreas Haufler on 25.11.18.
//

#include <iostream>
#include "System.h"
#include "Primitives.h"
#include "../compiler/Methods.h"

namespace pimii {

    System::System()
            : mm(16000000), symbols(mm), dictionary(mm),
              nilType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              metaClassType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              classType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              objectType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              smallIntType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              symbolType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              stringType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              associationType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              arrayType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              byteArrayType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              methodContextType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              blockContextType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              compiledMethodType(mm.makeRootObject(TYPE_SIZE + 1, Nil::NIL)),
              linkType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              processType(mm.makeRootObject(TYPE_SIZE, Nil::NIL)),
              trueValue(mm.makeRootObject(0, Nil::NIL)),
              falseValue(mm.makeRootObject(0, Nil::NIL)),
              proc(mm.makeRootObject(PROCESSOR_SIZE, Nil::NIL)),
              specialSelectorArray(mm.makeRootObject(NUMBER_OF_SPECIAL_SELECTORS, Nil::NIL)) {


        // Create "MetaClass class"
        auto metaClassClassType = ObjectPointer(mm.makeObject(TYPE_SIZE, Nil::NIL));
        metaClassClassType[TYPE_FIELD_NAME] = symbols.lookup("MetaClass class");
        metaClassClassType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = TYPE_SIZE;

        // Create "MetaClass"
        const_cast<ObjectPointer*>(&metaClassType)->type(metaClassClassType);
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
        const_cast<ObjectPointer*>(&objectType)->type(objectClassClassType);
        objectType[TYPE_FIELD_NAME] = symbols.lookup("Object");
        objectType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = 0;
        dictionary.atPut(objectType[TYPE_FIELD_NAME], objectType);

        // Create "Behaviour"
        auto behaviourType = makeType(objectType, "Behaviour", TYPE_SIZE, TYPE_SIZE);

        // Make "Behaviour" the superclass of "MetaClass"
        metaClassType[TYPE_FIELD_SUPERTYPE] = behaviourType;
        metaClassClassType[TYPE_FIELD_SUPERTYPE] = behaviourType.type();

        // Create "Class"
        completeType(classType, behaviourType, "Class", TYPE_SIZE);

        // Make "Class" the superclass of "Object class"
        objectClassClassType[TYPE_FIELD_SUPERTYPE] = classType;

        // Create "Nil"
        completeType(nilType, objectType, "Nil", 0);

        // Create "SmallInt"
        completeType(smallIntType, objectType, "SmallInteger", 0);

        // Create "Symbol"
        completeType(symbolType, objectType, "Symbol", 0);

        // Create "String"
        completeType(stringType, objectType, "String", 0);

        // Create "Association"
        completeType(associationType, objectType, "Association", SystemDictionary::ASSOCIATION_SIZE);

        // Create "CompiledMethod"
        completeType(compiledMethodType, objectType, "CompiledMethod", Interpreter::COMPILED_METHOD_SIZE);

        // Create "MethodContext" and "BlockContext"
        completeType(blockContextType, objectType, "BlockContext", Interpreter::CONTEXT_FIXED_SIZE);
        completeType(methodContextType, objectType, "MethodContext", Interpreter::CONTEXT_FIXED_SIZE);

        // Create "Array"
        completeType(arrayType, objectType, "Array", 0);
        completeType(byteArrayType, objectType, "ByteArray", 0);

        // Create "Link"
        completeType(linkType, objectType, "Link", 2);

        // Create "Process"
        completeType(processType, objectType, "Process", 1);

        ObjectPointer symbolTableType = makeType(objectType, "SymbolTable", 2, TYPE_SIZE);
        symbols.installTypes(symbolTableType, arrayType, symbolType);

        ObjectPointer systemDictionaryType = makeType(objectType, "SystemDictionary", 2,
                                                      TYPE_SIZE);
        dictionary.installTypes(systemDictionaryType, arrayType, associationType);

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
        specialSelectorArray[Primitives::PRIMITIVE_EQUALITY] = symbols.lookup("==");
        specialSelectorArray[1] = symbols.lookup("<");
        specialSelectorArray[2] = symbols.lookup("<=");
        specialSelectorArray[3] = symbols.lookup(">");
        specialSelectorArray[4] = symbols.lookup(">=");
        specialSelectorArray[5] = symbols.lookup("+");
        specialSelectorArray[6] = symbols.lookup("-");
        specialSelectorArray[7] = symbols.lookup("*");
        specialSelectorArray[8] = symbols.lookup("/");
        specialSelectorArray[9] = symbols.lookup("%");
        specialSelectorArray[10] = symbols.lookup("basicNew");
        specialSelectorArray[11] = symbols.lookup("basicNew:");
        specialSelectorArray[12] = symbols.lookup("class");
        specialSelectorArray[13] = symbols.lookup("value");
        specialSelectorArray[14] = symbols.lookup("value:");
        specialSelectorArray[15] = symbols.lookup("value:value:");
        specialSelectorArray[16] = symbols.lookup("value:value:value:");
        specialSelectorArray[17] = symbols.lookup("withArgs:");
        specialSelectorArray[18] = symbols.lookup("perform:");
        specialSelectorArray[19] = symbols.lookup("perform:with:");
        specialSelectorArray[20] = symbols.lookup("perform:with:and:");
        specialSelectorArray[21] = symbols.lookup("perform:with:and:and:");
        specialSelectorArray[22] = symbols.lookup("perform:withArgs:");

        // From now on, these selectors are sent to the receiver (and might still invoke a primitive)
        // but might also be overwritten by a class.
        specialSelectorArray[23] = symbols.lookup("hash");
        specialSelectorArray[24] = symbols.lookup("size");
        specialSelectorArray[25] = symbols.lookup("at:");
        specialSelectorArray[26] = symbols.lookup("at:put:");
        specialSelectorArray[27] = symbols.lookup("asSymbol");
        specialSelectorArray[28] = symbols.lookup("asString");
        specialSelectorArray[29] = symbols.lookup("do:");
        specialSelectorArray[30] = symbols.lookup("each:");
        specialSelectorArray[31] = symbols.lookup("ifTrue:");
        specialSelectorArray[32] = symbols.lookup("ifFalse:");
        specialSelectorArray[33] = symbols.lookup("ifTrue:otherwise:");
        specialSelectorArray[34] = symbols.lookup("whileTrue:");

        specialSelectorArray.type(arrayType);

        ObjectPointer processSchedulerType = makeType(objectType, "ProcessScheduler", 0, TYPE_SIZE);
        proc.type(processSchedulerType);
        dictionary.atPut(symbols.lookup("Processor"), proc);

        ObjectPointer irqs = mm.makeObject(10, arrayType);
        proc[System::PROCESSOR_FIELD_IRQ_TABLE] = irqs;

        ObjectPointer semaphoreType = makeType(objectType, "Semaphore", System::SEMAPHORE_SIZE, TYPE_SIZE);
        ObjectPointer timerSemaphore = mm.makeObject(System::SEMAPHORE_SIZE, semaphoreType);
        irqs[0] = timerSemaphore;
        timerSemaphore[SEMAPHORE_FIELD_EXCESS_SIGNALS] = 0;
        dictionary.atPut(symbols.lookup("TimerSemaphore"), timerSemaphore);
    }


    ObjectPointer System::makeType(ObjectPointer parent, const std::string& name, SmallInteger effectiveFixedFields,
                                   SmallInteger effetiveFixedClassFields) {
        ObjectPointer metaType = mm.makeObject(TYPE_SIZE, metaClassType);
        ObjectPointer type = mm.makeObject(effetiveFixedClassFields, ObjectPointer(metaType));
        metaType[TYPE_FIELD_NAME] = symbols.lookup(name + " class");
        metaType[TYPE_FIELD_SUPERTYPE] = parent.type();
        metaType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = effetiveFixedClassFields;
        type[TYPE_FIELD_NAME] = symbols.lookup(name);
        type[TYPE_FIELD_SUPERTYPE] = parent;
        type[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = effectiveFixedFields;

        dictionary.atPut(type[TYPE_FIELD_NAME], ObjectPointer(type));
        return ObjectPointer(type);
    }

    void System::completeType(ObjectPointer type, ObjectPointer parent, const std::string& name,
                              SmallInteger effectiveFixedFields) {
        ObjectPointer metaType = mm.makeObject(TYPE_SIZE, metaClassType);
        type.type(metaType);

        metaType[TYPE_FIELD_NAME] = symbols.lookup(name + " class");
        metaType[TYPE_FIELD_SUPERTYPE] = parent.type();
        metaType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = TYPE_SIZE;
        type[TYPE_FIELD_NAME] = symbols.lookup(name);
        type[TYPE_FIELD_SUPERTYPE] = parent;
        type[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = effectiveFixedFields;

        dictionary.atPut(type[TYPE_FIELD_NAME], ObjectPointer(type));
    }

    SmallInteger System::specialSelectorIndex(const std::string& name) {
        ObjectPointer symbol = symbols.lookup(name);
        for (SmallInteger index = 0; index < NUMBER_OF_SPECIAL_SELECTORS; index++) {
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
