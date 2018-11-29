//
// Created by Andreas Haufler on 23.11.18.
//

#include "SystemDictionary.h"
#include "Nil.h"

namespace pimii {

    const Offset SystemDictionary::DICTIONARY_SIZE = 2;
    const Offset SystemDictionary::DICTIONARY_FIELD_TALLY = 0;
    const Offset SystemDictionary::DICTIONARY_FIELD_TABLE = 1;
    const Offset SystemDictionary::ASSOCIATION_SIZE = 2;
    const Offset SystemDictionary::ASSOCIATION_FIELD_KEY = 0;
    const Offset SystemDictionary::ASSOCIATION_FIELD_VALUE = 1;

    SystemDictionary::SystemDictionary(MemoryManager &mm) : mm(mm), associationType(Nil::NIL), dictionary(
            mm.allocObject(DICTIONARY_SIZE, Nil::NIL)) {
        dictionary->fields[DICTIONARY_FIELD_TALLY] = ObjectPointer(0);
        dictionary->fields[DICTIONARY_FIELD_TABLE] = ObjectPointer(mm.allocObject(512, Nil::NIL));
    }


    Object *SystemDictionary::atPut(ObjectPointer key, ObjectPointer value, bool force) {
        Object *table = dictionary->fields[DICTIONARY_FIELD_TABLE].getObject();
        Offset index = key.hash() % table->size;

        for (Offset i = index; i < table->size; i++) {
            Object *result = tryInsert(i, table, key, value, force);
            if (result != nullptr) {
                return result;
            }
        }
        for (Offset i = 0; i < index; i++) {
            Object *result = tryInsert(i, table, key, value, force);
            if (result != nullptr) {
                return result;
            }
        }

        //TODO horror
        return nullptr;
    }

    Object *SystemDictionary::atPut(ObjectPointer key, ObjectPointer value) {
        return atPut(key, value, true);
    }

    Object *
    SystemDictionary::tryInsert(Offset index, Object *table, ObjectPointer key, ObjectPointer value, bool force) {
        ObjectPointer association = table->fields[index];
        if (association == Nil::NIL) {
            Object *newAssociation = mm.allocObject(ASSOCIATION_SIZE, associationType);
            newAssociation->fields[ASSOCIATION_FIELD_KEY] = key;
            newAssociation->fields[ASSOCIATION_FIELD_VALUE] = value;
            table->fields[index] = ObjectPointer(newAssociation);

            SmallInteger newSize = dictionary->fields[DICTIONARY_FIELD_TALLY].getInt() + 1;
            dictionary->fields[DICTIONARY_FIELD_TALLY] = ObjectPointer(newSize);
            if (newSize > table->size * 0.75) {
                grow(table);
            }
            return newAssociation;
        } else if (association.getObject()->fields[ASSOCIATION_FIELD_KEY] == key) {
            if (force) {
                association.getObject()->fields[ASSOCIATION_FIELD_VALUE] = value;
            }
            return association.getObject();
        } else {
            return nullptr;
        }
    }

    void SystemDictionary::grow(Object *table) {
        Object *newTable = mm.allocObject(table->size + 256, table->type);
        dictionary->fields[DICTIONARY_FIELD_TABLE] = ObjectPointer(newTable);

        for (Offset i = 0; i < table->size; i++) {
            if (table->fields[i] != Nil::NIL) {
                reInsert(newTable, table->fields[i]);
            }
        }

    }

    void SystemDictionary::reInsert(Object *table, ObjectPointer association) {
        ObjectPointer key = association.getObject()->fields[ASSOCIATION_FIELD_KEY];
        Offset index = key.hash() % table->size;

        for (Offset i = index; i < table->size; i++) {
            if (table->fields[i] == Nil::NIL) {
                table->fields[i] = association;
                return;
            }
        }
        for (Offset i = 0; i < index; i++) {
            if (table->fields[i] == Nil::NIL) {
                table->fields[i] = association;
                return;
            }
        }

        //TODO horror
    }

    Object *SystemDictionary::at(ObjectPointer key) {
        return atPut(key, Nil::NIL, false);
    }

    ObjectPointer SystemDictionary::getValue(ObjectPointer key) {
        return at(key)->fields[ASSOCIATION_FIELD_VALUE];
    }

    void SystemDictionary::installTypes(ObjectPointer systemDictionaryType, ObjectPointer arrayType,
                                        ObjectPointer associationType) {
        this->associationType = associationType;
        dictionary->type = systemDictionaryType;

        Object *table = dictionary->fields[DICTIONARY_FIELD_TABLE].getObject();
        table->type = arrayType;

        for (Offset i = 0; i < table->size; i++) {
            if (table->fields[i] != Nil::NIL) {
                table->fields[i].getObject()->type = associationType;
            }
        }
    }


}