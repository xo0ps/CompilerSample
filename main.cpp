#include <iostream>
#include <cstring>
#include <cstdlib>
#include "Compiler.h"
#include "Grammer.h"



int main( int argc , char ** argv )
{
    Compiler * comp;
    std::string again;
    do
    {

        comp = new Compiler();
        if( ! comp )
        {
            std::cout<<"Memory Allocation Error!"<<std::endl;
            return EXIT_FAILURE;
        }
        comp->help();
        comp->getInput();
        std::cout<<"======================================"<<std::endl;
        comp->waiting( PARSING );
        if( comp->isGrammerLL1() )
        {
            std::cout<<"Grammer is LL(1)"<<std::endl<<std::endl;
        }
        else
        {
            std::cout<<"Grammer is Not LL(1)"<<std::endl<<std::endl;
            comp->outputWhyNotLL1();
            comp->waiting( LL1_CALCULATING );
            comp->makeGrammerLL1();
            comp->outputLL1();
        }
        comp->outputFirsts();
        comp->outputFollows();
        comp->outputParsingTable();
        delete comp;

        std::cout<<"Want to try another Grammer?(y/n) ";
        std::getline( std::cin , again);
    }
    while( again == "y" || again == "Y" );

    return EXIT_SUCCESS;
}
