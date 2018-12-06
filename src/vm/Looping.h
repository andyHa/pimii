//
// Created by Andreas Haufler on 04.12.18.
//

#ifndef PIMII_LOOPING_H
#define PIMII_LOOPING_H

#include "ObjectPointer.h"

namespace pimii {

    class Looping {
        Offset max;
        Offset start;
        Offset index;
        bool wrapped;
    public:
        Looping( Offset size, Offset initialIndex) : max(size), start(initialIndex % max), index(start),
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

        Offset operator()() {
            return index;
        }

    };

}

#endif //PIMII_LOOPING_H
