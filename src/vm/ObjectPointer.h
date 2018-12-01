//
// Created by Andreas Haufler on 23.11.18.
//

#ifndef MEM_OBJECTPOINTER_H
#define MEM_OBJECTPOINTER_H

#include <cinttypes>
#include <typeinfo>
#include <cstring>
#include <string>

#define PIMII_ENABLE_CHECKS

namespace pimii {

    typedef unsigned long long Word;
    typedef int_fast32_t SmallInteger;
    typedef float Decimal;
    typedef uint_fast16_t Offset;

    enum ObjectPointerType : Word {
        OBJECT = 0,
        SMALL_INT = 1,
        BUFFER = 2,
        DECIMAL = 3
    };

    class ObjectPointer {
        struct Object {
            Word size;
            Word type;
            Word fields[];
        };


        Word data;
    public:

        static const inline Word MASK = 0b11;
        static const inline Word UNMASK = ~(Word) 0b11;

        inline ObjectPointer() noexcept : data(0) {};

        explicit inline ObjectPointer(SmallInteger intValue) noexcept : data(
                ((Word) (intValue) << 2) | ObjectPointerType::SMALL_INT) {};

        explicit inline ObjectPointer(Decimal floatValue) noexcept : data(
                ((Word) (floatValue) << 2) | ObjectPointerType::DECIMAL) {};

        explicit inline ObjectPointer(const void *object, ObjectPointerType type) noexcept : data(
                ((Word) object) | type) {};

        inline ObjectPointer(const ObjectPointer &other) noexcept = default;

        inline ObjectPointerType getObjectPointerType() const noexcept {
            return (ObjectPointerType) (data & MASK);
        }

        inline bool isSmallInt() const noexcept {
            return getObjectPointerType() == SMALL_INT;
        }

        inline SmallInteger smallInt() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != SMALL_INT) {
                throw std::bad_cast();
            }
#endif
            return (SmallInteger) (data >> 2);
        }

        inline bool isDecimal() const noexcept {
            return getObjectPointerType() == DECIMAL;
        }

        inline Decimal decimal() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != DECIMAL) {
                throw std::bad_cast();
            }
#endif
            return (Decimal) (data >> 2);
        }

        inline bool isObject() const noexcept {
            return getObjectPointerType() == OBJECT;
        }

        inline ObjectPointer &operator[](Offset index) {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != OBJECT) {
                throw std::bad_cast();
            }
#endif
            return *reinterpret_cast<ObjectPointer *>(&((Object *) data)->fields[index]);
        }

        inline bool isBuffer() const noexcept {
            return getObjectPointerType() == BUFFER;
        }

        inline char fetchByte(Offset index) {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BUFFER) {
                throw std::bad_cast();
            }
#endif

            return *(reinterpret_cast<char *>(&((Object *) data)->fields) + index);
        }

        void loadFrom(void *src, Offset byteLength) {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BUFFER) {
                throw std::bad_cast();
            }
#endif
            std::memcpy(&((Object *) (data & UNMASK))->fields[0], src, byteLength);
        }

        void storeTo(void *dest, Offset byteLength) {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BUFFER) {
                throw std::bad_cast();
            }
#endif
            std::memcpy(dest, &((Object *) (data & UNMASK))->fields[0], byteLength);
        }

        void transferBytesTo(ObjectPointer dest, Offset byteLength) {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BUFFER) {
                throw std::bad_cast();
            }
            if (dest.getObjectPointerType() != BUFFER) {
                throw std::bad_cast();
            }
#endif
            std::memcpy(&((Object *) dest.data)->fields[0], &((Object *) (data & UNMASK))->fields[0], byteLength);
        }

        void transferFieldsTo(Offset start, ObjectPointer dest, Offset destStart, Offset numberOfFields) {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != OBJECT) {
                throw std::bad_cast();
            }
            if (dest.getObjectPointerType() != OBJECT) {
                throw std::bad_cast();
            }
#endif
            std::memcpy(&((Object *) dest.data)->fields[destStart], &((Object *) data)->fields[start], numberOfFields *
                                                                                                       sizeof(Word));
        }

        std::u32string_view stringView() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BUFFER) {
                throw std::bad_cast();
            }
#endif
            return std::u32string_view(reinterpret_cast<char32_t *>(&(((Object *) (data & UNMASK))->fields[0])));
        }

        Offset hashString() const noexcept {
            std::hash<std::u32string_view> fn;
            return (Offset) fn(stringView());
        }

        int compareTo(ObjectPointer other) const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BUFFER) {
                throw std::bad_cast();
            }
            if (other.getObjectPointerType() != BUFFER) {
                throw std::bad_cast();
            }
#endif
            Offset thisSize = byteSize();
            Offset otherSize = other.byteSize();
            if (thisSize != otherSize) {
                return thisSize - otherSize;
            }

            auto thisObject = (Object *) data;
            auto otherObject = (Object *) other.data;

            return memcmp(&thisObject->fields[0], &otherObject->fields[0], thisSize);
        }

        Offset byteSize() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BUFFER) {
                throw std::bad_cast();
            }
#endif

            return 0;
        }

        inline ObjectPointer &operator=(const ObjectPointer &rhs) noexcept = default;

        inline ObjectPointer &operator=(const SmallInteger &rhs) noexcept {
            data = ((Word) (rhs) << 2) | ObjectPointerType::SMALL_INT;
            return *this;
        }

        inline ObjectPointer &operator=(const Decimal &rhs) noexcept {
            data = ((Word) (rhs) << 2) | ObjectPointerType::DECIMAL;
            return *this;
        }

        inline bool operator==(const ObjectPointer &rhs) const noexcept {
            return data == rhs.data;
        }

        inline bool operator!=(const ObjectPointer &rhs) const noexcept {
            return data != rhs.data;
        }

        inline Offset hash() const noexcept {
            return (Offset) data;
        }

    };


}


#endif //MEM_OBJECTPOINTER_H
