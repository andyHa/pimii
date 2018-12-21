//
// Created by Andreas Haufler on 2018-12-16.
//

#ifndef PIMII_ALLOCATOR_H
#define PIMII_ALLOCATOR_H

#include <memory>
#include "Segment.h"
#include "../common/types.h"

namespace pimii {

    class Allocator {
        std::shared_ptr<Segment> currentSegment;
        SmallInteger allocated;
        SmallInteger used;
        SmallInteger counter;
        SmallInteger maxSegments;


        Word* allocateInNewSegment(SmallInteger numberOfWords) {
            if (allocated >= maxSegments) {
                return nullptr;
            }
            currentSegment = std::make_shared<Segment>(SEGMENT_SIZE, currentSegment);
            allocated += 1;
            used += numberOfWords;
            counter++;

            return currentSegment->alloc(numberOfWords);
        }

    public:
        static constexpr SmallInteger SEGMENT_SIZE = 640000;

        explicit Allocator(SmallInteger maxSegments) : currentSegment(nullptr), allocated(0), used(0), counter(0),
                                                       maxSegments(maxSegments) {}

        SmallInteger objectCount() const {
            return counter;
        }

        SmallInteger allocatedWords() const {
            return allocated * SEGMENT_SIZE;
        }

        SmallInteger numberOfSegments() const {
            return allocated;
        }

        SmallInteger usedWords() const {
            return used;
        }

        Word* alloc(SmallInteger numberOfWords) {
            if (currentSegment != nullptr) {
                Word* result = currentSegment->alloc(numberOfWords);
                if (result != nullptr) {
                    used += numberOfWords;
                    counter++;
                    return result;
                }
            }

            return allocateInNewSegment(numberOfWords);
        }

        const std::shared_ptr<Segment>& lastSegment() {
            return currentSegment;
        }
    };
}


#endif //PIMII_ALLOCATOR_H
