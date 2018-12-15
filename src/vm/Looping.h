//
// Created by Andreas Haufler on 04.12.18.
//

#ifndef PIMII_LOOPING_H
#define PIMII_LOOPING_H

#include "ObjectPointer.h"

namespace pimii {

    class Looping {
        SmallInteger max;
        SmallInteger start;
        SmallInteger index;
        bool wrapped;
    public:
        Looping( SmallInteger size, SmallInteger initialIndex) : max(size), start(initialIndex % max), index(start),
                                                    wrapped(false) {};

        bool hasNext() {
            return index != start || !wrapped;
        }

        void next() {
            index++;
            if (index >= max) {
                index = 0;
                wrapped = true;
            }
        }

        SmallInteger operator()() {
            return index;
        }

    };

}

#endif //PIMII_LOOPING_H
