//
// Created by Andreas Haufler on 29.11.18.
//

#ifndef MEM_NIL_H
#define MEM_NIL_H

#include "ObjectPointer.h"

namespace pimii {
    class Nil {
    public:
        static const inline ObjectPointer NIL = {};
    };
}

#endif //MEM_NIL_H
