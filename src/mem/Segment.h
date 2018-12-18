//
// Created by Andreas Haufler on 2018-12-16.
//

#ifndef PIMII_SEGMENT_H
#define PIMII_SEGMENT_H

#include <memory>
#include "../common/types.h"

namespace pimii {

    class Segment {
        Word* basePointer;
        Word* allocationPointer;
        Word* endPointer;
        Word size;
        std::shared_ptr<Segment> previous;
    public:
        explicit Segment(Word sizeOfPool, std::shared_ptr<Segment> previous) : size(sizeOfPool),
                                                                               previous(std::move(previous)) {
            basePointer = (Word*) calloc(size, sizeof(Word));
            endPointer = basePointer + size;
            allocationPointer = basePointer;
            memset(allocationPointer, 0, (endPointer - allocationPointer) * sizeof(Word));
        }

        ~Segment() {
            free(basePointer);
        }

        Word sizeInWord() {
            return size;
        }

        Word* alloc(SmallInteger numberOfWords) {

            if (allocationPointer + numberOfWords < endPointer) {
                Word* result = allocationPointer;
                allocationPointer += numberOfWords;
                return result;
            } else {
                return nullptr;
            }
        }

        const std::shared_ptr<Segment>& previousSegment() {
            return previous;
        }

    };

}

#endif //PIMII_SEGMENT_H
