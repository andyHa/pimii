//
// Created by Andreas Haufler on 25.11.18.
//

#include "TypeSystem.h"
#include "Nil.h"

namespace pimii {

    TypeSystem::TypeSystem(MemoryManager &mm, SymbolTable &symbols, SystemDictionary &systemDictionary) :
            mm(mm), symbols(symbols), systemDictionary(systemDictionary),
            nilType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            metaClassType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            classType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            objectType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            smallIntType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            symbolType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            stringType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            associationType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            arrayType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            byteArrayType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            methodContextType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            blockContextType(mm.allocObject(TYPE_SIZE, Nil::NIL)),
            compiledMethodType(mm.allocObject(TYPE_SIZE + 1, Nil::NIL)) {

        // Create "MetaClass class"
        auto metaClassClassType = ObjectPointer(mm.allocObject(TYPE_SIZE, Nil::NIL));
        metaClassClassType.getObject()->fields[TYPE_FIELD_NAME] = symbols.lookup("MetaClass class");

        // Create "MetaClass"
        metaClassType.getObject()->type = metaClassClassType;
        metaClassType.getObject()->fields[TYPE_FIELD_NAME] = symbols.lookup("MetaClass");
        systemDictionary.atPut(metaClassType.getObject()->fields[TYPE_FIELD_NAME], metaClassType);

        // Set type of "MetaClass class" to "MetaClass"
        metaClassClassType.getObject()->type = metaClassType;

        // Create "Object class"
        auto objectClassClassType = ObjectPointer(mm.allocObject(TYPE_SIZE, metaClassType));
        objectClassClassType.getObject()->fields[TYPE_FIELD_NAME] = symbols.lookup("Object class");

        // Create "Object"
        objectType.getObject()->type = objectClassClassType;
        objectType.getObject()->fields[TYPE_FIELD_NAME] = symbols.lookup("Object");
        systemDictionary.atPut(objectType.getObject()->fields[TYPE_FIELD_NAME], objectType);

        // Create "Behaviour"
        auto behaviourType = makeType(objectType, "Behaviour", 0);

        // Make "Behaviour" the superclass of "MetaClass"
        metaClassType.getObject()->fields[TYPE_FIELD_SUPERTYPE] = behaviourType;
        metaClassClassType.getObject()->fields[TYPE_FIELD_SUPERTYPE] = behaviourType.getObject()->type;

        // Create "Class"
        completeType(classType, behaviourType, "Class");

        // Make "Class" the superclass of "Object class"
        objectClassClassType.getObject()->fields[TYPE_FIELD_SUPERTYPE] = classType;

        // Create "Nil"
        completeType(nilType, objectType, "Nil");

        // Create "Number" and "SmallInt"
        auto numberType = makeType(objectType, "Number", 0);
        completeType(smallIntType, numberType, "SmallInteger");

        // Create "Symbol"
        completeType(symbolType, objectType, "Symbol");

        // Create "String"
        completeType(stringType, objectType, "String");

        // Create "Association"
        completeType(associationType, objectType, "Association");

        // Create "CompiledMethod"
        completeType(compiledMethodType, objectType, "CompiledMethod");

        // Create "MethodContext" and "BlockContext"
        auto contextType = makeType(objectType, "Context", 0);
        completeType(blockContextType, contextType, "BlockContext");
        completeType(methodContextType, contextType, "MethodContext");

        // Create "Array"
        auto collectionType = makeType(objectType, "Collection", 0);
        completeType(arrayType, collectionType, "Array");
        completeType(byteArrayType, collectionType, "ByteArray");
    }

    ObjectPointer TypeSystem::makeType(ObjectPointer parent, const std::string &name, Offset typeFields) {
        Object *metaType = mm.allocObject(TYPE_SIZE, metaClassType);
        Object *type = mm.allocObject(TYPE_SIZE + typeFields, ObjectPointer(metaType));
        metaType->fields[TYPE_FIELD_NAME] = symbols.lookup(name + " class");
        metaType->fields[TYPE_FIELD_SUPERTYPE] = parent.getObject()->type;
        type->fields[TYPE_FIELD_NAME] = symbols.lookup(name);
        type->fields[TYPE_FIELD_SUPERTYPE] = parent;

        systemDictionary.atPut(type->fields[TYPE_FIELD_NAME], ObjectPointer(type));
        return ObjectPointer(type);
    }

    void TypeSystem::completeType(ObjectPointer type, ObjectPointer parent, const std::string &name) {
        Object *typeObject = type.getObject();
        Object *metaType = mm.allocObject(TYPE_SIZE, metaClassType);
        typeObject->type = ObjectPointer(metaType);

        metaType->fields[TYPE_FIELD_NAME] = symbols.lookup(name + " class");
        metaType->fields[TYPE_FIELD_SUPERTYPE] = parent.getObject()->type;
        typeObject->fields[TYPE_FIELD_NAME] = symbols.lookup(name);
        typeObject->fields[TYPE_FIELD_SUPERTYPE] = parent;

        systemDictionary.atPut(typeObject->fields[TYPE_FIELD_NAME], ObjectPointer(type));
    }


}
