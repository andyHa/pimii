//
// Created by Andreas Haufler on 06.12.18.
//

#include <iostream>
#include <vector>
#include <string>
#include "SourceFileParser.h"
#include "Compiler.h"

namespace pimii {

    void SourceFileParser::compile() {
        while (tokenizer.current().type != EOI) {
            if (tokenizer.current().value == "Class:") {
                parseClassDefinition();
            } else if (tokenizer.current().value == "Methods:") {
                parseMethodsSection();
            } else if (tokenizer.current().value == "Class" && tokenizer.next().value == "Methods:") {
                parseClassMethodsSection();
            } else {
                errors.emplace_back(Error(tokenizer.currentLine(), "Unexpected Token:" + tokenizer.current().value));
                tokenizer.consume();
            }
        }
    }

    void SourceFileParser::parseClassDefinition() {
        SmallInteger lastLine = tokenizer.currentLine();
        tokenizer.consume();
        std::string className = tokenizer.consume().value;
        std::string superclassName = "Object";
        std::vector<std::string> instanceFields;
        std::vector<std::string> classFields;
        while (!tokenizer.current().isEOI() && tokenizer.currentLine() <= lastLine + 1) {
            lastLine = tokenizer.currentLine();
            if (tokenizer.current().value == "Superclass:") {
                tokenizer.consume();
                superclassName = tokenizer.consume().value;
                continue;
            }
            if (tokenizer.current().value == "Instance" && tokenizer.next().value == "Fields:") {
                tokenizer.consume();
                tokenizer.consume();
                while (tokenizer.current().type == NAME && tokenizer.currentLine() == lastLine) {
                    instanceFields.emplace_back(tokenizer.consume().value);
                }
                continue;
            }
            if (tokenizer.current().value == "Class" && tokenizer.next().value == "Fields:") {
                tokenizer.consume();
                tokenizer.consume();
                while (tokenizer.current().type == NAME && tokenizer.currentLine() == lastLine) {
                    classFields.emplace_back(tokenizer.consume().value);
                }
                continue;
            }

            //TODD horrible error
        }

        ObjectPointer superclassAsSymbol = system.getSymbolTable().lookup(superclassName);
        ObjectPointer superclass = system.getSystemDictionary().getValue(superclassAsSymbol);
        if (!system.is(superclass, system.getTypeSystem().classType)) {
            errors.emplace_back(Error(tokenizer.currentLine(), "Unknown superclass: " + superclassName));
            return;
        }

        ObjectPointer nameAsSymbol = system.getSymbolTable().lookup(className);
        ObjectPointer type = system.getSystemDictionary().getValue(nameAsSymbol);

        if (type == Nil::NIL) {
            createNewType(className, instanceFields, classFields, superclass, nameAsSymbol);
        } else {
            updateExistingType(type, instanceFields, classFields, superclass);
        }

        std::cout << "New class: " << className << std::endl;
    }

    void SourceFileParser::createNewType(const std::string& className, const std::vector<std::string>& instanceFields,
                                         const std::vector<std::string>& classFields, ObjectPointer superclass,
                                         ObjectPointer nameAsSymbol) {
        SmallInteger effectiveNumberOfFixedFields =
                superclass[TypeSystem::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() +
                (SmallInteger) instanceFields.size();
        SmallInteger effectiveFixedClassFields =
                superclass.type()[TypeSystem::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() +
                (SmallInteger) classFields.size();
        ObjectPointer newClass = system.getTypeSystem().makeType(superclass, className, effectiveNumberOfFixedFields,
                                                                 effectiveFixedClassFields);
        storeFields(newClass, instanceFields);
        storeFields(newClass.type(), classFields);
        system.getSystemDictionary().atPut(nameAsSymbol, newClass);
    }

    void SourceFileParser::updateExistingType(ObjectPointer type, const std::vector<std::string>& instanceFields,
                                              const std::vector<std::string>& classFields, ObjectPointer superclass) {
        SmallInteger effectiveNumberOfFixedFields =
                superclass[TypeSystem::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() +
                (SmallInteger) instanceFields.size();
        if (type[TypeSystem::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() != effectiveNumberOfFixedFields) {
            errors.emplace_back(
                    Error(tokenizer.currentLine(), "Number of instance fields cannot be changed after the fact!"));
            return;
        }
        SmallInteger effectiveNumberOfFixedClassFields =
                superclass.type()[TypeSystem::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() +
                (SmallInteger) classFields.size();
        if (type.type()[TypeSystem::TYPE_FIELD_NUMBER_OF_FIXED_FIELDS].smallInt() !=
            effectiveNumberOfFixedClassFields) {
            errors.emplace_back(
                    Error(tokenizer.currentLine(), "Number of class fields cannot be changed after the fact!"));
            return;
        }

        storeFields(type, instanceFields);
        storeFields(type.type(), classFields);
        type[TypeSystem::TYPE_FIELD_SUPERTYPE] = superclass;
    }

    void SourceFileParser::storeFields(ObjectPointer type, std::vector<std::string> fields) {
        if (fields.empty()) {
            return;
        }

        ObjectPointer fieldArray = system.getMemoryManager().makeObject(fields.size(),
                                                                        system.getTypeSystem().arrayType);
        for (SmallInteger index = 0; index < fields.size(); index++) {
            fieldArray[index] = system.getMemoryManager().makeString(fields[index],
                                                                     system.getTypeSystem().stringType);
        }
        type[TypeSystem::TYPE_FIELD_FIELD_NAMES] = fieldArray;
    }

    void SourceFileParser::parseMethodsSection() {
        tokenizer.consume();
        std::string className = tokenizer.consume().value;
        handleMethodsSection(className, false);
    }

    void SourceFileParser::parseClassMethodsSection() {
        tokenizer.consume();
        tokenizer.consume();
        std::string className = tokenizer.consume().value;
        handleMethodsSection(className, true);
    }

    void SourceFileParser::handleMethodsSection(const std::string& className, bool classMethod) {
        ObjectPointer classAsSymbol = system.getSymbolTable().lookup(className);
        ObjectPointer type = system.getSystemDictionary().getValue(classAsSymbol);
        if (!system.is(type, system.getTypeSystem().classType)) {
            errors.emplace_back(Error(tokenizer.currentLine(), "Unknown class: " + className));
            return;
        }

        if (classMethod) {
            type = type.type();
        }

        if (tokenizer.current().type == SEPARATOR) {
            tokenizer.consume();
        }

        while (!tokenizer.current().isEOI() && tokenizer.current().value != "Class:" &&
               tokenizer.current().value != "Methods:" &&
               (tokenizer.current().value != "Class" || tokenizer.next().value != "Methods:")) {

            Compiler compiler(tokenizer, errors, type);
            compiler.compileMethodAndAdd(system);
            std::cout << "New Method: " << className << " :: " << compiler.selector << std::endl;
            if (tokenizer.current().type == SEPARATOR) {
                tokenizer.consume();
            }
        }
    }

}
