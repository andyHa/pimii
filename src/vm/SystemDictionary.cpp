//
// Created by Andreas Haufler on 23.11.18.
//

#include "SystemDictionary.h"

namespace pimii {

    const Offset SystemDictionary::DICTIONARY_SIZE = 2;
    const Offset SystemDictionary::DICTIONARY_FIELD_TALLY = 0;
    const Offset SystemDictionary::DICTIONARY_FIELD_TABLE = 1;
    const Offset SystemDictionary::ASSOCIATION_SIZE = 2;
    const Offset SystemDictionary::ASSOCIATION_FIELD_KEY = 0;
    const Offset SystemDictionary::ASSOCIATION_FIELD_VALUE = 1;

    SystemDictionary::SystemDictionary(MemoryManager &mm) : mm(mm), associationType(Nil::NIL), dictionary(
            mm.allocObject(DICTIONARY_SIZE, Nil::NIL)) {
        dictionary[DICTIONARY_FIELD_TALLY] = 0;
        dictionary[DICTIONARY_FIELD_TABLE] = mm.allocObject(512, Nil::NIL);
    }


    ObjectPointer SystemDictionary::atPut(ObjectPointer key, ObjectPointer value, bool force) {
        ObjectPointer table = dictionary[DICTIONARY_FIELD_TABLE];
        Offset index = key.hash() % table.size();

        for (Offset i = index; i < table.size(); i++) {
            ObjectPointer result = tryInsert(i, table, key, value, force);
            if (result != Nil::NIL) {
                return result;
            }
        }
        for (Offset i = 0; i < index; i++) {
            ObjectPointer result = tryInsert(i, table, key, value, force);
            if (result != Nil::NIL) {
                return result;
            }
        }

        //TODO horror
        return Nil::NIL;
    }

    ObjectPointer SystemDictionary::atPut(ObjectPointer key, ObjectPointer value) {
        return atPut(key, value, true);
    }

    ObjectPointer
    SystemDictionary::tryInsert(Offset index, ObjectPointer table, ObjectPointer key, ObjectPointer value, bool force) {
        ObjectPointer association = table[index];
        if (association == Nil::NIL) {
            ObjectPointer newAssociation = mm.allocObject(ASSOCIATION_SIZE, associationType);
            newAssociation[ASSOCIATION_FIELD_KEY] = key;
            newAssociation[ASSOCIATION_FIELD_VALUE] = value;
            table[index] = ObjectPointer(newAssociation);

            SmallInteger newSize = dictionary[DICTIONARY_FIELD_TALLY].smallInt() + 1;
            dictionary[DICTIONARY_FIELD_TALLY] = newSize;
            if (newSize > table.size() * 0.75) {
                grow(table);
            }
            return newAssociation;
        } else if (association[ASSOCIATION_FIELD_KEY] == key) {
            if (force) {
                association[ASSOCIATION_FIELD_VALUE] = value;
            }
            return association;
        } else {
            return Nil::NIL;
        }
    }

    void SystemDictionary::grow(ObjectPointer table) {
        ObjectPointer newTable = mm.allocObject(table.size() + 256, table.type());
        dictionary[DICTIONARY_FIELD_TABLE] = newTable;

        for (Offset i = 0; i < table.size(); i++) {
            if (table[i] != Nil::NIL) {
                reInsert(newTable, table[i]);
            }
        }

    }

    void SystemDictionary::reInsert(ObjectPointer table, ObjectPointer association) {
        ObjectPointer key = association[ASSOCIATION_FIELD_KEY];
        Offset index = key.hash() % table.size();

        for (Offset i = index; i < table.size(); i++) {
            if (table[i] == Nil::NIL) {
                table[i] = association;
                return;
            }
        }
        for (Offset i = 0; i < index; i++) {
            if (table[i] == Nil::NIL) {
                table[i] = association;
                return;
            }
        }

        //TODO horror
    }

    ObjectPointer SystemDictionary::at(ObjectPointer key) {
        return atPut(key, Nil::NIL, false);
    }

    ObjectPointer SystemDictionary::getValue(ObjectPointer key) {
        return at(key)[ASSOCIATION_FIELD_VALUE];
    }

    void SystemDictionary::installTypes(ObjectPointer systemDictionaryType, ObjectPointer arrayType,
                                        ObjectPointer associationType) {
        this->associationType = associationType;
        dictionary.type(systemDictionaryType);

        ObjectPointer table = dictionary[DICTIONARY_FIELD_TABLE];
        table.type(arrayType);

        for (Offset i = 0; i < table.size(); i++) {
            if (table[i] != Nil::NIL) {
                table[i].type(associationType);
            }
        }
    }


}