//
// Created by Andreas Haufler on 23.11.18.
//

#ifndef MEM_OBJECTPOINTER_H
#define MEM_OBJECTPOINTER_H

#include <string>

#include "types.h"

namespace pimii {

    class ObjectPointer {

        constexpr static Word TYPE_MASK = 0b11;
        constexpr static Word ODD_MASK = 0x0F;
        constexpr static Word GC_MASK = 0xF0;

        static constexpr SmallInteger usableSizeBytes() {
            return sizeof(Word) - 1;
        }

        constexpr Word sizeMask() const {
            Word mask = 0x00;
            for (int i = 0; i < usableSizeBytes(); i++) {
                mask = mask << 8;
                mask |= 0xFF;
            }

            return mask;
        }

        constexpr Word clearGCMask() const {
            Word mask = 0x0F;
            for (int i = 0; i < usableSizeBytes(); i++) {
                mask = mask << 8;
                mask |= 0xFF;
            }

            return mask;
        }

        enum ObjectPointerType : Word {
            OBJECT = 0,
            SMALL_INT = 1,
            BUFFER = 2,
            DECIMAL = 3
        };

        struct Object {
            Word size;
            Word type;
            Word fields[];
        };

        Word data;

        inline ObjectPointerType getObjectPointerType() const noexcept {
            return (ObjectPointerType) (data & TYPE_MASK);
        }

        inline Object* unmask() const {
            if (getObjectPointerType() != BUFFER && getObjectPointerType() != OBJECT) {
                throw std::bad_cast();
            }

            return (Object*) (data & ~TYPE_MASK);
        }

        inline Object* object() const {
            if (getObjectPointerType() != OBJECT) {
                throw std::bad_cast();
            }

            return unmask();
        }

        inline Object* buffer() const {
            if (getObjectPointerType() != BUFFER) {
                throw std::bad_cast();
            }

            return unmask();
        }

        SmallInteger highNibble() const {
            return unmask()->size >> (usableSizeBytes() * 8);
        }

        explicit ObjectPointer(Word data) noexcept : data(data) {};
    public:
        ObjectPointer() noexcept : data(0) {}

        ObjectPointer(const ObjectPointer& other) noexcept = default;

        static ObjectPointer forObject(Word data) noexcept {
            return ObjectPointer(data);
        }

        static ObjectPointer forSmallInt(SmallInteger intValue) noexcept {
            return ObjectPointer(((Word) (intValue) << 2) | ObjectPointerType::SMALL_INT);
        }

        static ObjectPointer forDecimal(Decimal floatValue) noexcept {
            return ObjectPointer(((Word) (floatValue) << 2) | ObjectPointerType::DECIMAL);
        }

        ObjectPointer(const void* object, ObjectPointer type, SmallInteger numberOfFields) noexcept : data(
                ((Word) object) | OBJECT) {
            unmask()->size = static_cast<Word>(numberOfFields);
            unmask()->type = reinterpret_cast<Word&>(type);
        };

        ObjectPointer(const void* object, ObjectPointer type, SmallInteger wordSize, SmallInteger odd) noexcept : data(
                ((Word) object) | BUFFER) {

            unmask()->size = (Word) wordSize | (Word) ((odd & ODD_MASK) << (usableSizeBytes() * 8));
            unmask()->type = *reinterpret_cast<Word*>(&type);
        };

        ObjectPointer type() const {
            return *reinterpret_cast<ObjectPointer*>(&unmask()->type);
        }

        void type(ObjectPointer newType) {
            unmask()->type = *reinterpret_cast<Word*>(&newType);
        }

        ObjectPointer gcSuccessor() const {
            return type();
        }

        void gcSuccessor(ObjectPointer successor) {
            type(successor);
        }

        char gcInfo() const {
            return (char) ((unmask()->size >> (usableSizeBytes() * 8)) & GC_MASK) >> 4;
        }

        void gcInfo(char info) {
            unmask()->size &= clearGCMask();
            unmask()->size |= (static_cast<Word> ((info << 4) & GC_MASK)) << (usableSizeBytes() * 8);
        }

        bool isSmallInt() const noexcept {
            return getObjectPointerType() == SMALL_INT;
        }

        SmallInteger smallInt() const {
            if (getObjectPointerType() != SMALL_INT) {
                throw std::bad_cast();
            }

            return static_cast<SmallInteger>(data >> 2);
        }

        inline bool isDecimal() const noexcept {
            return getObjectPointerType() == DECIMAL;
        }

        inline Decimal decimal() const {
            if (getObjectPointerType() != DECIMAL) {
                return smallInt();
            }

            return (Decimal) (data >> 2);
        }

        bool isNumeric() const noexcept {
            return isDecimal() || isSmallInt();
        }

        inline bool isObject() const noexcept {
            return getObjectPointerType() == OBJECT;
        }

        inline ObjectPointer& operator[](SmallInteger index) const {
            if (index < 0 || index > size()) {
                throw std::range_error("Index out of range.");
            }
            return *reinterpret_cast<ObjectPointer*>(&object()->fields[index]);
        }

        inline bool isBuffer() const noexcept {
            return getObjectPointerType() == BUFFER;
        }

        inline char fetchByte(SmallInteger index) {
            if (index < 0 || index >= byteSize()) {
                throw std::range_error("Byte index out of range.");
            }
            return *(reinterpret_cast<char*>(&buffer()->fields[0]) + index);
        }

        inline void storeByte(SmallInteger index, char byte) {
            if (index < 0 || index >= byteSize()) {
                throw std::range_error("Byte index out of range.");
            }
            *(reinterpret_cast<char*>(&buffer()->fields[0]) + index) = byte;
        }

        void loadFrom(const void* src, SmallInteger byteLength) {
            if (byteLength > byteSize()) {
                throw std::range_error("byteLength index out of range.");
            }
            std::memcpy(&(buffer())->fields[0], src, static_cast<size_t>(byteLength));
        }

        void storeTo(void* dest, SmallInteger byteLength) {
            if (byteLength > byteSize()) {
                throw std::range_error("byteLength index out of range.");
            }
            std::memcpy(dest, &buffer()->fields[0], static_cast<size_t>(byteLength));
        }

        void transferBytesTo(SmallInteger start, ObjectPointer dest, SmallInteger destStart, SmallInteger byteLength) {
            if (start < 0 || destStart < 0 || byteLength > byteSize() - start || byteLength > dest.byteSize() - destStart) {
                throw std::range_error("byteLength index out of range.");
            }
            std::memcpy(reinterpret_cast<char*>(&dest.buffer()->fields[0]) + destStart,
                        reinterpret_cast<char*>( &buffer()->fields[0]) + start,
                        static_cast<size_t>(byteLength));
        }

        void
        transferFieldsTo(SmallInteger start, ObjectPointer dest, SmallInteger destStart, SmallInteger numberOfFields) {
            if (start < 0 || destStart < 0 || numberOfFields > size() - start || numberOfFields > dest.size() - destStart) {
                throw std::range_error("numberOfFields index out of range.");
            }
            std::memcpy(&dest.object()->fields[destStart], &object()->fields[start], numberOfFields * sizeof(Word));
        }

        std::string_view stringView() const {
            return std::string_view(byteArray());
        }

        char* byteArray() const {
            return reinterpret_cast<char*>(&buffer()->fields[0]);
        }

        SmallInteger hash() const {
            char* data = byteArray();
            SmallInteger size = byteSize();

            return hashByteArray(data, size);
        }

        static SmallInteger hashByteArray(const char* data, SmallInteger size) {
            SmallInteger result = 0;
            for (int i = 0; i < size; i++) {
                result += data[i];
            }

            if (result < 0) {
                result *= -1;
            }

            return result;
        }

        int compare(const void* other, SmallInteger length) const {
            SmallInteger thisSize = byteSize();
            if (thisSize > length) {
                return 1;
            } else if (thisSize < length) {
                return -1;
            }

            return memcmp(&buffer()->fields[0], other, static_cast<size_t>(thisSize));
        }

        int compareTo(ObjectPointer other) const {
            SmallInteger thisSize = byteSize();
            SmallInteger otherSize = other.byteSize();
            if (thisSize > otherSize) {
                return 1;
            } else if (thisSize < otherSize) {
                return -1;
            }

            return memcmp(&buffer()->fields[0], &other.buffer()->fields[0], static_cast<size_t>(thisSize));
        }

        SmallInteger size() const {
            return static_cast<SmallInteger>(unmask()->size & sizeMask());
        }

        SmallInteger byteSize() const {
            Word wordSize = buffer()->size & sizeMask();
            Word odd = highNibble() & ODD_MASK;

            return static_cast<SmallInteger>((wordSize * sizeof(Word)) - odd);
        }

        inline ObjectPointer& operator=(const ObjectPointer& rhs) noexcept = default;

        inline ObjectPointer& operator=(const SmallInteger& rhs) noexcept {
            data = (static_cast<Word> (rhs) << 2) | ObjectPointerType::SMALL_INT;
            return *this;
        }

        inline bool operator==(const ObjectPointer& rhs) const noexcept {
            return data == rhs.data;
        }

        inline bool operator!=(const ObjectPointer& rhs) const noexcept {
            return data != rhs.data;
        }

        inline SmallInteger id() const noexcept {
            return SmallIntegers::toSafeSmallInteger(data);
        }

    };

    class Nil {
    public:
        static const inline ObjectPointer NIL;
    };

}


#endif //MEM_OBJECTPOINTER_H
