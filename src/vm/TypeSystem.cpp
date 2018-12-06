//
// Created by Andreas Haufler on 25.11.18.
//

#include "TypeSystem.h"
#include "Interpreter.h"

namespace pimii {

    TypeSystem::TypeSystem(MemoryManager& mm, SymbolTable& symbols, SystemDictionary& systemDictionary) :
            mm(mm), symbols(symbols), systemDictionary(systemDictionary),
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
            compiledMethodType(mm.makeRootObject(TYPE_SIZE + 1, Nil::NIL)) {

        // Create "MetaClass class"
        auto metaClassClassType = ObjectPointer(mm.makeObject(TYPE_SIZE, Nil::NIL));
        metaClassClassType[TYPE_FIELD_NAME] = symbols.lookup("MetaClass class");
        metaClassClassType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = TYPE_SIZE;

        // Create "MetaClass"
        const_cast<ObjectPointer*>(&metaClassType)->type(metaClassClassType);
        metaClassType[TYPE_FIELD_NAME] = symbols.lookup("MetaClass");
        metaClassType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = TYPE_SIZE;
        systemDictionary.atPut(metaClassType[TYPE_FIELD_NAME], metaClassType);

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
        systemDictionary.atPut(objectType[TYPE_FIELD_NAME], objectType);

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

        // Create "Number" and "SmallInt"
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
    }

    ObjectPointer TypeSystem::makeType(ObjectPointer parent, const std::string& name, Offset effectiveFixedFields,
                                       Offset effetiveFixedClassFields) {
        ObjectPointer metaType = mm.makeObject(TYPE_SIZE, metaClassType);
        ObjectPointer type = mm.makeObject(effetiveFixedClassFields, ObjectPointer(metaType));
        metaType[TYPE_FIELD_NAME] = symbols.lookup(name + " class");
        metaType[TYPE_FIELD_SUPERTYPE] = parent.type();
        metaType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = effetiveFixedClassFields;
        type[TYPE_FIELD_NAME] = symbols.lookup(name);
        type[TYPE_FIELD_SUPERTYPE] = parent;
        type[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = effectiveFixedFields;

        systemDictionary.atPut(type[TYPE_FIELD_NAME], ObjectPointer(type));
        return ObjectPointer(type);
    }

    void TypeSystem::completeType(ObjectPointer type, ObjectPointer parent, const std::string& name,
                                  Offset effectiveFixedFields) {
        ObjectPointer metaType = mm.makeObject(TYPE_SIZE, metaClassType);
        type.type(metaType);

        metaType[TYPE_FIELD_NAME] = symbols.lookup(name + " class");
        metaType[TYPE_FIELD_SUPERTYPE] = parent.type();
        metaType[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = TYPE_SIZE;
        type[TYPE_FIELD_NAME] = symbols.lookup(name);
        type[TYPE_FIELD_SUPERTYPE] = parent;
        type[TYPE_FIELD_NUMBER_OF_FIXED_FIELDS] = effectiveFixedFields;

        systemDictionary.atPut(type[TYPE_FIELD_NAME], ObjectPointer(type));
    }


}
