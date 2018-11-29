//
// Created by Andreas Haufler on 25.11.18.
//

#ifndef MEM_STRINGS_H
#define MEM_STRINGS_H

#include <string>
#include "ObjectPointer.h"
#include "TypeSystem.h"

namespace pimii {

    class Strings {
    public:
        static bool areEqual(ObjectPointer stringObject, std::string string);

        static Offset hash(ObjectPointer stringObject);

        static Offset hash(const char *string);

        static ObjectPointer make(MemoryManager &mm, ObjectPointer type, std::string string);
    };

}


#endif //MEM_STRINGS_H
