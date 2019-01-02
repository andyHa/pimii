//
// Created by Andreas Haufler on 23.11.18.
//

#include "SystemDictionary.h"
#include "../common/Looping.h"
#include "System.h"

namespace pimii {

    SystemDictionary::SystemDictionary(MemoryManager &mm) : mm(mm), associationType(Nil::NIL), dictionary(
            mm.makeRootObject(System::DICTIONARY_SIZE, Nil::NIL)) {
        dictionary[System::DICTIONARY_FIELD_TALLY] = 0;
        dictionary[System::DICTIONARY_FIELD_TABLE] = mm.makeObject(512, Nil::NIL);
    }

    ObjectPointer SystemDictionary::atPut(ObjectPointer key, ObjectPointer value, bool force) {
        ObjectPointer table = dictionary[System::DICTIONARY_FIELD_TABLE];

        for (Looping loop = Looping(table.size(), key.hash()); loop.hasNext(); loop.next()) {
            ObjectPointer association = table[loop()];
            if (association == Nil::NIL) {
                ObjectPointer newAssociation = mm.makeObject(System::ASSOCIATION_SIZE, associationType);
                newAssociation[System::ASSOCIATION_FIELD_KEY] = key;
                newAssociation[System::ASSOCIATION_FIELD_VALUE] = value;
                table[loop()] = ObjectPointer(newAssociation);

                SmallInteger newSize = dictionary[System::DICTIONARY_FIELD_TALLY].smallInt() + 1;
                dictionary[System::DICTIONARY_FIELD_TALLY] = newSize;
                if (newSize > table.size() * 0.75) {
                    grow(table);
                }
                return newAssociation;
            } else if (association[System::ASSOCIATION_FIELD_KEY] == key) {
                if (force) {
                    association[System::ASSOCIATION_FIELD_VALUE] = value;
                }
                return association;
            }
        }

        throw std::runtime_error("Failed to insert an association into the system dictionary!");
    }

    ObjectPointer SystemDictionary::atPut(ObjectPointer key, ObjectPointer value) {
        return atPut(key, value, true);
    }

    void SystemDictionary::grow(ObjectPointer table) {
        ObjectPointer newTable = mm.makeObject(table.size() + 256, table.type());
        dictionary[System::DICTIONARY_FIELD_TABLE] = newTable;

        for (SmallInteger i = 0; i < table.size(); i++) {
            if (table[i] != Nil::NIL) {
                reInsert(newTable, table[i]);
            }
        }
    }

    void SystemDictionary::reInsert(ObjectPointer table, ObjectPointer association) {
        ObjectPointer key = association[System::ASSOCIATION_FIELD_KEY];

        for (Looping loop = Looping(table.size(), key.hash()); loop.hasNext(); loop.next()) {
            if (table[loop()] == Nil::NIL) {
                table[loop()] = association;
                return;
            }
        }

        throw std::runtime_error("Failed to re-insert an association into the system dictionary!");
    }

    ObjectPointer SystemDictionary::at(ObjectPointer key) {
        return atPut(key, Nil::NIL, false);
    }

    ObjectPointer SystemDictionary::getValue(ObjectPointer key) {
        return at(key)[System::ASSOCIATION_FIELD_VALUE];
    }

    void SystemDictionary::installTypes(ObjectPointer systemDictionaryType, ObjectPointer arrayType,
                                        ObjectPointer associationType) {
        this->associationType = associationType;
        dictionary.type(systemDictionaryType);

        ObjectPointer table = dictionary[System::DICTIONARY_FIELD_TABLE];
        table.type(arrayType);

        for (SmallInteger i = 0; i < table.size(); i++) {
            if (table[i] != Nil::NIL) {
                table[i].type(associationType);
            }
        }
    }


}