//
// Created by Andreas Haufler on 23.11.18.
//

#ifndef MEM_OBJECTPOINTER_H
#define MEM_OBJECTPOINTER_H

#include <string>
#include <vector>

#define PIMII_ENABLE_CHECKS

namespace pimii {

    typedef unsigned long long Word;
    typedef int_fast32_t SmallInteger;
    typedef uint_fast32_t Offset;

    enum ObjectPointerType : Word {
        OBJECT = 0,
        SMALL_INT = 1,
        BYTES = 2,
        WORDS = 3
    };

    struct Object;
    struct WordBuffer;
    struct ByteBuffer;

    class ObjectPointer {
        Word data;

    public:

        static const inline Word  MASK = 0b11;
        static const inline Word UNMASK= ~(Word) 0b11;

        inline ObjectPointer() noexcept : data(0) {};

        explicit inline ObjectPointer(SmallInteger intValue) noexcept : data(
                ((Word) (intValue) << 2) | ObjectPointerType::SMALL_INT) {};

        explicit inline ObjectPointer(const Object *object) noexcept : data((Word) object) {};

        explicit inline ObjectPointer(WordBuffer *buffer) noexcept : data((Word) buffer | ObjectPointerType::WORDS) {};

        explicit inline ObjectPointer(ByteBuffer *buffer) noexcept : data((Word) buffer | ObjectPointerType::BYTES) {};

        inline ObjectPointer(const ObjectPointer &other) noexcept = default;

        inline ObjectPointerType getObjectPointerType() const noexcept {
            return (ObjectPointerType) (data & MASK);
        }

        inline SmallInteger getInt() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != SMALL_INT) {
                throw std::bad_cast();
            }
#endif
            return (SmallInteger) (data >> 2);
        }

        inline Object *getObject() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != OBJECT) {
                throw std::bad_cast();
            }
#endif
            return (Object *) (data & UNMASK);
        }

        inline WordBuffer *getWords() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != WORDS) {
                throw std::bad_cast();
            }
#endif

            return (WordBuffer *) (data & UNMASK);
        }

        inline ByteBuffer *getBytes() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BYTES) {
                throw std::bad_cast();
            }
#endif

            return (ByteBuffer *) (data & UNMASK);
        }

        inline ObjectPointer &operator=(const ObjectPointer &rhs) noexcept {
            data = rhs.data;
            return *this;
        }

        inline bool operator==(const ObjectPointer &rhs) const noexcept {
            return data == rhs.data;
        }

        inline bool operator!=(const ObjectPointer &rhs) const noexcept {
            return data != rhs.data;
        }

        inline Offset hash() const noexcept{
            return (Offset) data;
        }

    };


    struct Object {
        Offset size;
        ObjectPointer type;
        ObjectPointer fields[];

        Object(const Object &other) = delete;
    };

    struct WordBuffer {
        Offset size;
        ObjectPointer type;
        Word words[];
    };

    struct ByteBuffer {
        Offset size;
        ObjectPointer type;
        Offset odd;
        uint8_t bytes[];
    };


}


#endif //MEM_OBJECTPOINTER_H
