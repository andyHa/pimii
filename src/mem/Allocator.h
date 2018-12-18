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
        std::shared_ptr <Segment> currentSegment;
        Word allocated;
        Word used;
        Word counter;
        Word limit;
        bool gcRecommended;

        static constexpr Word SEGMENT_SIZE = 400000;

        Word* allocateInNewSegment(SmallInteger numberOfWords) {
            if (allocated + SEGMENT_SIZE > limit) {
                return nullptr;
            }
            currentSegment = std::make_shared<Segment>(SEGMENT_SIZE, currentSegment);
            allocated += SEGMENT_SIZE;
            used += numberOfWords;
            counter++;
            gcRecommended = true;

            return currentSegment->alloc(numberOfWords);
        }

    public:
        Allocator(Word limit) : currentSegment(nullptr), allocated(0), used(0), counter(0), limit(limit) {}

        Word objectCount() const {
            return counter;
        }

        Word allocatedWords() const {
            return allocated;
        }

        Word usedWords() const {
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

        const std::shared_ptr <Segment>& lastSegment() {
            return currentSegment;
        }

        bool shouldGC() {
            return gcRecommended;
        }

        void resetGCRecommendation() {
            gcRecommended = false;
        }
    };
}


#endif //PIMII_ALLOCATOR_H
