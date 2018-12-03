//
// Created by Andreas Haufler on 25.11.18.
//

#include "MemoryManager.h"

namespace pimii {


    ObjectPointer MemoryManager::allocBytes(Offset numberOfBytes, ObjectPointer type) {
        Offset numberOfWords = numberOfBytes / sizeof(Word);
        Offset odd = sizeof(Word) - (numberOfBytes % sizeof(Word));
        if (odd != 0) {
            numberOfWords++;
        }

        auto data = malloc(sizeof(Word) + sizeof(ObjectPointer) * (numberOfWords + 1));
        memset(data, 0, sizeof(Word) + sizeof(ObjectPointer) * (numberOfWords + 1));

        return ObjectPointer(data, type, numberOfWords, odd);
    }

    ObjectPointer MemoryManager::allocString(std::string_view string, ObjectPointer type) {
        unsigned long byteLength = string.size() + 1;
        ObjectPointer obj = allocBytes(byteLength, type);

        obj.loadFrom(string.data(), byteLength);
        return obj;
    }

}
