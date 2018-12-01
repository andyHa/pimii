//
// Created by Andreas Haufler on 25.11.18.
//

#include <iostream>
#include "Strings.h"

namespace pimii {

    bool Strings::areEqual(ObjectPointer stringObject, std::string string) {
        ByteBuffer *buffer = stringObject.getBytes();
        Offset length = buffer->size * sizeof(Word) - buffer->odd;
        const char *other = string.c_str();

        for (Offset i = 0; i < length; i++) {
            if (buffer->bytes[i] != other[i]) {
                std::cout << buffer->bytes << " != " << string << std::endl;
                return false;
            }

            if (other[i] == '\0') {
                return true;
            }
        }

        std::cout << buffer->bytes << " != " << string << std::endl;
        return false;
    }

    Offset Strings::hash(ObjectPointer stringObject) {
        return hash((const char *) stringObject.getBytes()->bytes);
    }

    Offset Strings::hash(const char *string) {
        Offset result = 0;
        while (*string != '\0') {
            result += *string;
            string++;
        }

        return result;
    }

    ObjectPointer Strings::make(MemoryManager &mm, ObjectPointer type, std::string string) {
        ByteBuffer *buffer = mm.allocBytes((Offset) string.size() + 1, type);
        std::memcpy(buffer->bytes, string.data(), string.size());
        buffer->bytes[string.size()] ='\0';
        return ObjectPointer(buffer);
    }

}
