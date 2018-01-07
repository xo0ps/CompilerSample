#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED
#include <vector>
#include <cstring>
#include <fstream>
#include "Grammer.h"

enum WaitType
{
    PARSING,
    LL1_CALCULATING,
};

class Compiler
{
    private:
        std::vector< std::string >m_grammers_container;
        bool validate(std::string s , int nr_grammers);
        void addToGrammer(std::string s);
        void createGrammers();
        std::vector< Grammer > m_grammers;
        std::vector< Grammer > m_grammers2;
        void makeUniqueLeftSides();
        bool areLeftSidesUnique();
        void recheckGrammers( std::string method_name );
        std::vector< char > m_null_variables;
        bool findNullVariables();
        void makeWithoutEpsilon();
        void makeWithLeftFactoring();
        void makeWithoutLeftRecursion();
        void omitRepeatedVariables();
        bool canLeftFactoring();
        void insertSingleTerminals();
        void insertSingleNonTerminals();
        bool canRemoveLeftRecursion();
        std::vector< char > m_left_recursion_variables;
        char findNewLeftSideChar();
        void omitRemovedLeftSides();
        void insertAllTermsInS();
        void removeLastGrammer();

    public:
        std::ofstream out;
        Compiler();
        ~Compiler();
        void help();
        void getInput();
        bool isGrammerLL1();
        void makeGrammerLL1();
        void outputLL1();
        void outputFirsts();
        void outputFollows();
        void outputParsingTable();
        void waiting( int type );
        void outputWhyNotLL1();

};

#endif // COMPILER_H_INCLUDED
