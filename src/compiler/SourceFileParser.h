//
// Created by Andreas Haufler on 06.12.18.
//

#ifndef PIMII_SOURCEFILEPARSER_H
#define PIMII_SOURCEFILEPARSER_H


#include "Tokenizer.h"
#include "../vm/System.h"

namespace pimii {

    class SourceFileParser {
        System& system;
        std::vector<Error> errors;
        Tokenizer tokenizer;

        void parseClassDefinition();

        void createNewType(const std::string& className, const std::vector<std::string>& instanceFields,
                           const std::vector<std::string>& classFields, ObjectPointer superclass,
                           ObjectPointer nameAsSymbol);

        void updateExistingType(ObjectPointer type, const std::vector<std::string>& instanceFields,
                                const std::vector<std::string>& classFields, ObjectPointer superclass);

        void storeFields(ObjectPointer type, std::vector<std::string> fields);

        void parseMethodsSection();

        void parseClassMethodsSection();

        void handleMethodsSection(const std::string& className, bool classMethod);

    public:
        SourceFileParser(System& system, std::string_view source) : system(system), tokenizer(source, errors) {}

        void compile();

        const std::vector<Error>& getErrors() {
            return errors;
        }


    };
}

#endif //PIMII_SOURCEFILEPARSER_H
