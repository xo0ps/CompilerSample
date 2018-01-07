#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "Compiler.h"
#include "Grammer.h"

#define log
#define windows

#ifdef windows
#include <windows.h> //use it on windows
#else
#include <pthread.h>   //use it on linux
#endif

Compiler::Compiler()
{
#ifdef log
    out.open("logger.txt");
#endif
}

Compiler::~Compiler()
{
#ifdef log
    out.close();
#endif
}

void
Compiler::help()
{
    for( int i = 0 ; i < 20 ; i++ )
        std::cout<<std::endl;
    std::cout<<"Welcome to my Compiler"<<std::endl;
    std::cout<<"Your are allowed to use the following characters:"<<std::endl;
    std::cout<<"Characters A-Z for Non-Terminals and a-z for Terminals,"<<std::endl;
    std::cout<<"-> for up to down navigation and must be the second-third characters,"<<std::endl;
    std::cout<<"| for division,"<<std::endl;
    std::cout<<"0 for Epsilon."<<std::endl;
    std::cout<<"Do not use white spaces."<<std::endl;
    std::cout<<"Start first Grammer with S."<<std::endl;
    std::cout<<"Start each Grammer with Non-Terminals."<<std::endl;
    std::cout<<"Enter END to finish the Grammers."<<std::endl;
    std::cout<<"Enter NEW to correct the last entered Grammer."<<std::endl;
    std::cout<<std::endl;
}

void
Compiler::getInput()
{
    std::cout<<"Please Enter your Grammers:"<<std::endl;

    std::string s;
    int nr_grammers = 1;
    m_grammers_container.clear();
    while ( nr_grammers <= 26 )
    {
        std::getline(std::cin,s);

        if( s.size() > 200 )
        {
            std::cout<<"Grammer is too long. Enter something less than 200 characters."<<std::endl;
            continue;
        }

        if( s == "end" || s == "END" )
        {
            std::cout<<"Grammers Entered Successfully."<<std::endl;
            break;
        }

        if( s == "new" || s == "NEW" )
        {
            removeLastGrammer();
            std::cout<<"Last Grammer is removed. Enter another Grammer."<<std::endl;
            continue;
        }

        if( validate(s , nr_grammers) )
        {
            addToGrammer(s);
            nr_grammers++;
        }
        else
        {
            std::cout<<"Grammer Not Allowed. Enter a Correct Grammer!"<<std::endl;
        }
    }

#ifdef log
    out<<"m_grammers_container in getInput()"<<std::endl;
    for( unsigned int i = 0 ; i < m_grammers_container.size() ; i++ )
        out<<m_grammers_container[i]<<std::endl;
#endif
}

bool
Compiler::validate(std::string s , int nr_grammers )
{
    if( s.size() < 4 )
        return false;

    if( s[ s.size() - 1 ] == '|' )
        return false;

    if( nr_grammers == 1 )
    {
        if( s[0] != 'S' )
            return false;
    }

    if( ! ( s[0] >= 'A' && s[0] <= 'Z' ) )
        return false;

    if( ! ( s[1] == '-' && s[2] == '>' ) )
        return false;

    s.erase( 0 , 3 );
    bool break_flag = false;
    while( true )
    {
        size_t pos = s.find_first_of('|');
        if( pos == std::string::npos )
            break_flag = true;
        std::string ss = ( break_flag ) ? s : s.substr( 0 , pos );
        {
            for( unsigned int i = 0 ; i < ss.size() ; i++ )
            {
                if( ss[i] >= 'a' && ss[i] <= 'z' )
                    continue;

                if( ss[i] >= 'A' && ss[i] <= 'Z' )
                    continue;

                if( ss.size() == 1 && ss[i] == '0' )
                    continue;

                return false;
            }
        }
        if( break_flag )
            break;
        s.erase( 0 , pos + 1 );
    }

    return true;
}

void
Compiler::addToGrammer(std::string s)
{
    m_grammers_container.push_back(s);
}

void
Compiler::createGrammers()
{
    for( unsigned int iter = 0 ; iter < m_grammers_container.size() ; iter++ )
    {
        Grammer g;
        g.leftSide = m_grammers_container[iter][0];
        std::string line = m_grammers_container[iter];
        line.erase( 0 , 3 );
        while( true )
        {
            size_t pos = line.find_first_of('|');
            if( pos == std::string::npos )
                break;
            std::string rightSide = line.substr( 0 , pos );
            g.rightSides.push_back( rightSide );
            line.erase( 0 , pos + 1 );
        }
        g.rightSides.push_back( line );
        g.containsEpsilon = false;
        g.leftFactoring = false;
        g.leftRecursion = false;

        std::vector< char >first_terms;
        for( unsigned int i = 0 ; i < g.rightSides.size() ; i++ )
        {
            if( g.rightSides[i] == "0" )
                g.containsEpsilon = true;

            if( g.rightSides[i][0] == g.leftSide )
                g.leftRecursion = true;

            first_terms.push_back( g.rightSides[i][0] );
        }

        bool break_flag = false;
        for( unsigned int i = 0 ; i < first_terms.size() - 1 ; i++ )
        {
            if( break_flag )
                break;
            for( unsigned int j = i + 1 ; j < first_terms.size() ; j++ )
            {
                if( first_terms[i] == first_terms[j] )
                {
                    g.leftFactoring = true;
                    break_flag = true;
                    break;
                }
            }
        }

        m_grammers.push_back( g );
    }

#ifdef log
    out<<"\nm_grammers in createGrammers()"<<std::endl;
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        out<<std::endl;
        out<<m_grammers[i].leftSide<<"\t";
        for( unsigned int j = 0 ; j < m_grammers[i].rightSides.size() ; j++ )
            out<<m_grammers[i].rightSides[j]<<"\t";
        m_grammers[i].containsEpsilon ? out<<"true\t" : out<<"false\t";
        m_grammers[i].leftRecursion ? out<<"true\t" : out<<"false\t";
        m_grammers[i].leftFactoring ? out<<"true" : out<<"false";
    }
    out<<std::endl;
#endif
}

void
Compiler::makeGrammerLL1()
{
    if( ! areLeftSidesUnique() )
        makeUniqueLeftSides();

    omitRepeatedVariables();
    recheckGrammers( "makeUniqueLeftSides" );

    if( canRemoveLeftRecursion() )
        makeWithoutLeftRecursion();

    omitRepeatedVariables();
    recheckGrammers( "makeWithoutLeftRecursion" );

    if( findNullVariables() )
        makeWithoutEpsilon();

    omitRepeatedVariables();
    recheckGrammers( "makeWithoutEpsilon" );

    insertSingleNonTerminals();

    omitRepeatedVariables();
    recheckGrammers( "insertSingleNonTerminals" );

    insertSingleTerminals();

    omitRepeatedVariables();
    recheckGrammers( "insertSingleTerminals" );

    omitRemovedLeftSides();

    omitRepeatedVariables();
    recheckGrammers( "omitRemovedLeftSides" );

    //insertAllTermsInS();

    //omitRepeatedVariables();
    //recheckGrammers( "insertAllTermsInS" );

    if( canLeftFactoring() )
        makeWithLeftFactoring();

    //omitRepeatedVariables();
    //recheckGrammers( "makeWithLeftFactoring" );
}

void
Compiler::omitRepeatedVariables()
{
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        for( unsigned int j = 0 ; j < m_grammers[i].rightSides.size() - 1 ; j++ )
        {
            for( unsigned int k = j + 1 ; k < m_grammers[i].rightSides.size() ; k++ )
            {
                if( m_grammers[i].rightSides[j] == m_grammers[i].rightSides[k] )
                {
                    m_grammers[i].rightSides.erase( m_grammers[i].rightSides.begin() + k );
                    k--;
                }
            }
        }
    }
}

void
Compiler::omitRemovedLeftSides()
{
    char leftSides[ m_grammers.size() ];
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        leftSides[i] = m_grammers[i].leftSide;
    }

    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        for( unsigned int j = 0 ; j < m_grammers[i].rightSides.size() ; j++ )
        {
            for( unsigned int iter = 0 ; iter < m_grammers[i].rightSides[j].length() ; iter++ )
            {
                char this_char = m_grammers[i].rightSides[j][iter];
                if( this_char >= 'A' && this_char <= 'Z' )
                {
                    bool found = false;
                    for( unsigned int counter = 0 ; counter < m_grammers.size() ; counter++ )
                    {
                        if( leftSides[counter] == this_char )
                        {
                            found = true;
                            break;
                        }
                    }
                    if( ! found )
                    {
                        m_grammers[i].rightSides[j] = "";
                    }
                }
            }
        }
    }
}


bool
Compiler::canLeftFactoring()
{
    for( unsigned int iter = 0 ; iter < m_grammers.size() ; iter++ )
    {
        if( m_grammers[iter].leftFactoring )
            return true;
    }
    return false;
}

void
Compiler::insertSingleTerminals()
{
    while( true )
    {
        bool changed = false;
        for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
        {
            if( m_grammers[i].leftSide == 'S' )
                continue;
            for( unsigned int iter1 = 0 ; iter1 < m_grammers[i].rightSides.size() ; iter1++ )
            {
                if( m_grammers[i].rightSides[iter1].size() != 1 )
                    continue;
                char rightside = m_grammers[i].rightSides[iter1][0];
                if( rightside >= 'a' && rightside <= 'z' )
                {
                    char leftSide = m_grammers[i].leftSide;
                    for( unsigned int j = 0 ; j < m_grammers.size() ; j++ )
                    {
                        if( m_grammers[j].leftSide != leftSide )
                        {
                            for( unsigned int k = 0 ; k < m_grammers[j].rightSides.size() ; k++ )
                            {
                                std::string old_variable = m_grammers[j].rightSides[k];
                                char * c_old_variable = new char[ old_variable.length() + 1 ];
                                std::strcpy( c_old_variable , old_variable.c_str() );
                                for( unsigned int iter = 0 ; iter < old_variable.length() + 1 ; iter++ )
                                {
                                    if( c_old_variable[ iter ] == leftSide )
                                    {
                                        c_old_variable[ iter ] = '1';
                                        std::string new_variable( c_old_variable );
                                        size_t pos = new_variable.find_first_of( '1' );
                                        std::string rs;
                                        rs += rightside;
                                        new_variable.replace( pos , 1 , rs );
                                        m_grammers[j].rightSides.push_back( new_variable );
                                        c_old_variable[ iter ] = leftSide;
                                        changed = true;
                                    }
                                }
                                delete[] c_old_variable;
                            }
                        }
                    }
                    m_grammers[i].rightSides[iter1] = "";
                }
            }
        }
        if( ! changed )
            break;
    }
}

void
Compiler::insertSingleNonTerminals()
{
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( m_grammers[i].leftSide == 'S' )
            continue;
        for( unsigned int iter = 0 ; iter < m_grammers[i].rightSides.size() ; iter++ )
        {
            if( m_grammers[i].rightSides[iter].size() != 1 )
                continue;
            char rightSide = m_grammers[i].rightSides[iter][0];
            if( rightSide >= 'A' && rightSide <= 'Z' )
            {
                for( unsigned int j = 0 ; j < m_grammers.size() ; j++ )
                {
                    if( m_grammers[j].leftSide == rightSide )
                    {
                        m_grammers[i].rightSides[iter] = "";
                        for( unsigned int k = 0 ; k < m_grammers[j].rightSides.size() ; k++ )
                            m_grammers[i].rightSides.push_back( m_grammers[j].rightSides[k] );
                        m_grammers.erase( m_grammers.begin() + j );
                        break;
                    }
                }
            }
        }
    }
}

void
Compiler::insertAllTermsInS()
{
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( m_grammers[i].leftSide != 'S' )
            continue;

        for( unsigned int j = 0 ; j < m_grammers[i].rightSides.size() ; j++ ) //this is S
        {
            std::string this_variable = m_grammers[i].rightSides[j];
            for( unsigned int k = 0 ; k < this_variable.size() ; k++ )
            {
                char this_char = this_variable[k];
                if( this_char >= 'a' && this_char <= 'z' )
                    continue;

                for( unsigned int l = 0 ; l < m_grammers.size() ; l++ )
                {
                    if( this_char != m_grammers[l].leftSide )
                        continue;

                    for( unsigned int m = 0 ; m < m_grammers[l].rightSides.size() ; m++ )
                    {
                        std::string new_variable = this_variable;
                        if( k != 0 && this_variable[k-1] == m_grammers[l].rightSides[m][0] )
                            continue;
                        new_variable.replace( k , m_grammers[l].rightSides[m].length() , m_grammers[l].rightSides[m] );
                        m_grammers[i].rightSides.push_back( new_variable );
                    }
                }
            }
        }
    }

    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( m_grammers[i].leftSide != 'S' )
        {
            m_grammers.erase( m_grammers.begin() + i );
            i--;
        }
    }
}

bool
Compiler::findNullVariables()
{
    m_grammers2 = m_grammers;
    bool found = false;

    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( m_grammers[i].containsEpsilon )
        {
            m_null_variables.push_back( m_grammers[i].leftSide );
            found = true;
        }
    }
    /*
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( m_grammers[i].containsEpsilon )
            continue;
        for( unsigned int j = 0 ; j < m_grammers[i].rightSides.size() ; j++ )
        {
            if( m_grammers[i].rightSides[j].size() == m_null_variables.size() )
            {
                char nulls[ m_null_variables.size() ];
                char rightnulls[ m_null_variables.size() ];
                for( unsigned int counter = 0 ; counter < m_null_variables.size() ; counter++ )
                {
                    nulls[counter] = m_null_variables[counter];
                    rightnulls[counter] = m_grammers[i].rightSides[counter];
                }
                for( unsigned int counter = 0 ; counter < m_null_variables.size() - 1 ; counter++ )
                {
                    for( unsigned int counter2 = counter + 1 ; counter2 < m_null_variables.size() ; counter2++ )
                    {
                        if( nulls[counter] > nulls[counter2] )
                        {
                            char tmp = nulls[counter];
                            nulls[counter] = nulls[counter2];
                            nulls[counter2] = tmp;
                        }
                        if( rightnulls[counter] > rightnulls[counter2] )
                        {
                            char tmp = rightnulls[counter];
                            rightnulls[counter] = rightnulls[counter2];
                            rightnulls[counter2] = tmp;
                        }
                    }
                }
                bool must_add = true;
                for( unsigned int counter = 0 ; counter < m_null_variables.size() ; counter++ )
                {
                    if( nulls[counter] != rightnulls[counter] )
                        must_add = false;
                }
            }
        }

    }
    */
    return found;
}

bool
Compiler::canRemoveLeftRecursion()
{
    bool found = false;

    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        for( unsigned int j = 0 ; j < m_grammers[i].rightSides.size() ; j++ )
        {
            if( m_grammers[i].rightSides[j][0] == m_grammers[i].leftSide )
            {
                m_left_recursion_variables.push_back( m_grammers[i].leftSide );
                found = true;
            }
        }
    }

    return found;

}

void
Compiler::recheckGrammers( std::string method_name )
{
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        for( unsigned int j = 0 ; j < m_grammers[i].rightSides.size() ; j++ )
        {
            if( m_grammers[i].rightSides[j] == "" || m_grammers[i].rightSides[j] == " " )
            {
                m_grammers[i].rightSides.erase( m_grammers[i].rightSides.begin() + j );
                j--;
            }
        }

    }

    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( m_grammers[i].rightSides.size() == 0 )
        {
            m_grammers.erase( m_grammers.begin() + i );
            i--;
        }
    }

    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        m_grammers[i].containsEpsilon = false;
        m_grammers[i].leftRecursion = false;
        m_grammers[i].leftFactoring = false;
        std::vector< char >first_terms;
        for( unsigned int j = 0 ; j < m_grammers[i].rightSides.size() ; j++ )
        {
            if( m_grammers[i].rightSides[j] == "0" )
                m_grammers[i].containsEpsilon = true;

            if( m_grammers[i].rightSides[j][0] == m_grammers[i].leftSide )
                m_grammers[i].leftRecursion = true;

            first_terms.push_back( m_grammers[i].rightSides[j][0] );
        }
        bool break_flag = false;
        for( unsigned int iter = 0 ; iter < first_terms.size() - 1 ; iter++ )
        {
            if( break_flag )
                break;
            for( unsigned int j = iter + 1 ; j < first_terms.size() ; j++ )
            {
                if( first_terms[iter] == first_terms[j] )
                {
                    m_grammers[i].leftFactoring = true;
                    break_flag = true;
                    break;
                }
            }
        }
    }

#ifdef log
    out<<"\nm_grammers in "<<method_name<<"()"<<std::endl;
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        out<<std::endl;
        out<<m_grammers[i].leftSide<<"\t";
        for( unsigned int j = 0 ; j < m_grammers[i].rightSides.size() ; j++ )
            out<<m_grammers[i].rightSides[j]<<"\t";
        m_grammers[i].containsEpsilon ? out<<"true\t" : out<<"false\t";
        m_grammers[i].leftRecursion ? out<<"true\t" : out<<"false\t";
        m_grammers[i].leftFactoring ? out<<"true" : out<<"false";
    }
    out<<std::endl;
#endif
}

void
Compiler::waiting( int type )
{
    switch (type)
    {
    case PARSING:
        std::cout<<"Grammer is being Parsed!"<<std::endl;
        std::cout<<"Please Wait";
        break;

    case LL1_CALCULATING:
        std::cout<<"Grammer is being LL(1)!"<<std::endl;
        std::cout<<"Please Wait";
        break;

    default:
        std::cout<<"Something is being Processed!"<<std::endl;
        std::cout<<"Please Wait";
        break;
    }

    for( int i = 0 ; i < 3 ; i++ )
    {
#ifdef windows
        //Sleep(1000); //windows
#else
        //sleep(1);   //linux
#endif
        std::cout<<".";
    }
    std::cout<<std::endl;
}

bool
Compiler::isGrammerLL1()
{
    createGrammers();

    //if( ! areLeftSidesUnique() )
    //   return false;

    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( m_grammers[i].containsEpsilon )
            return false;
        if( m_grammers[i].leftFactoring )
            return false;
        if( m_grammers[i].leftRecursion )
            return false;
    }

    return true;
}


bool
Compiler::areLeftSidesUnique()
{
    for( unsigned int i = 0 ; i < m_grammers.size() - 1 ; i++ )
    {
        for( unsigned int j = i + 1 ; j < m_grammers.size() ; j++ )
        {
            if( m_grammers[i].leftSide == m_grammers[j].leftSide )
                return false;
        }
    }

    return true;
}

void
Compiler::makeUniqueLeftSides()
{
    for( unsigned int i = 0 ; i < m_grammers.size() - 1 ; i++ )
    {
        for( unsigned int j = i + 1 ; j < m_grammers.size() ; j++ )
        {
            if( m_grammers[i].leftSide == m_grammers[j].leftSide )
            {
                for( unsigned int k = 0 ; k < m_grammers[j].rightSides.size() ; k++ )
                {
                    m_grammers[i].rightSides.push_back( m_grammers[j].rightSides[k] );
                }
                m_grammers.erase( m_grammers.begin() + j );
                j--;
            }
        }
    }
}

void
Compiler::outputFirsts()
{
    std::cout<<"Firsts are as Below:"<<std::endl;
    std::vector< std::pair< char , std::vector< std::string > > >firsts;
    for( unsigned int i = 0 ; i < m_grammers2.size() ; i++ )
    {
        char leftSide = m_grammers[i].leftSide;
        std::vector< std::string >firstss;
        for( unsigned int j = 0 ; j < m_grammers2[i].rightSides.size() ; j++ )
        {
            std::string this_first = findFirstOfThisVariable( m_grammers2.rightSides[j] );
            firstss.push_back( this_first );
        }
        firsts.push_back( std::make_pair( leftSide , firstss ) );
    }

    for( unsigned int i = 0 ; i < firsts.size() ; i++ )
    {
        std::cout<<"First("<<firsts[i].first<<")={";
        for( unsigned int j = 0 ; j < firsts[i].second.size() ; j++ )
        {
            std::cout<<firsts[i].second[j];
            if( j != firsts[i].second.size() - 1 )
                std::cout<<",";
        }
        std::cout<<"}"<<std::endl;
    }
}

void
Compiler::outputFollows()
{
    std::cout<<"Follows are as Below:"<<std::endl;
    std::vector< std::pair< char , std::vector< std::string > > >follows;
    for( unsigned int i = 0 ; i < m_grammers2.size() ; i++ )
    {
        char leftSide = m_grammers[i].leftSide;
        std::vector< std::string >followss;
        for( unsigned int j = 0 ; j < m_grammers2[i].rightSides.size() ; j++ )
        {
            std::string this_first = findFollowsOfThisVariable( m_grammers2.rightSides[j] );
            followss.push_back( this_first );
        }
        follows.push_back( std::make_pair( leftSide , followss ) );
    }

    for( unsigned int i = 0 ; i < follows.size() ; i++ )
    {
        std::cout<<"Follow("<<follows[i].first<<")={";
        for( unsigned int j = 0 ; j < follows[i].second.size() ; j++ )
        {
            std::cout<<follows[i].second[j];
            if( j != follows[i].second.size() - 1 )
                std::cout<<",";
        }
        std::cout<<"}"<<std::endl;
    }
}

void
Compiler::outputLL1()
{
    std::cout<<"LL(1) Grammer is as Below:"<<std::endl;
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        std::cout<<m_grammers[i].leftSide<<"->";
        for( unsigned int j = 0 ; j < m_grammers[i].rightSides.size() ; j++ )
        {
            std::cout<<m_grammers[i].rightSides[j];
            if( j != m_grammers[i].rightSides.size() - 1 )
                std::cout<<"|";
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}

void
Compiler::outputParsingTable()
{

}

void
Compiler::makeWithoutEpsilon()
{
    for( unsigned int i = 0 ; i < m_null_variables.size() ; i++ )
    {
        char this_null_var = m_null_variables[i];

        //here we delete 0 from each grammer
        for( unsigned int j = 0 ; j < m_grammers.size() ; j++ )
        {
            if( m_grammers[j].leftSide == this_null_var )
            {
                for( unsigned int k = 0 ; k < m_grammers[j].rightSides.size() ; k++ )
                {
                    if( m_grammers[j].rightSides[k] == "0" )
                    {
                        m_grammers[j].rightSides.erase( m_grammers[j].rightSides.begin() + k );
                        k--;
                    }
                }
            }
        }

        //here we insert 0 to each another grammers
        for( unsigned int j = 0 ; j < m_grammers.size() ; j++ )
        {
            for( unsigned int k = 0 ; k < m_grammers[j].rightSides.size() ; k++ )
            {
                std::string old_variable = m_grammers[j].rightSides[k];
                char * c_old_variable = new char[ old_variable.length() + 1 ];
                std::strcpy( c_old_variable , old_variable.c_str() );
                for( unsigned int iter = 0 ; iter < old_variable.length() + 1 ; iter++ )
                {
                    if( c_old_variable[ iter ] == this_null_var )
                    {
                        c_old_variable[ iter ] = '1';
                        std::string new_variable( c_old_variable );
                        size_t pos = new_variable.find_first_of( '1' );
                        new_variable.replace( pos , 1 , "" );
                        m_grammers[j].rightSides.push_back( new_variable );
                        c_old_variable[ iter ] = this_null_var;
                    }
                }

                for( unsigned int iter = 0 ; iter < old_variable.length() + 1 ; iter++ )
                {
                    if( c_old_variable[ iter ] == this_null_var )
                    {
                        c_old_variable[ iter ] = '1';
                        std::string new_variable( c_old_variable );
                        size_t pos = new_variable.find_first_of( '1' );
                        new_variable.replace( pos , 1 , "" );
                        m_grammers[j].rightSides.push_back( new_variable );
                    }
                }

                delete[] c_old_variable;
            }
        }
    }

    m_null_variables.clear();
}

void
Compiler::makeWithoutLeftRecursion()
{
    for( unsigned int i = 0 ; i < m_left_recursion_variables.size() ; i++ )
    {
        char this_left_recursion_char = m_left_recursion_variables[i];

        for( unsigned int j = 0 ; j < m_grammers.size() ; j++ )
        {
            if( m_grammers[j].leftSide == this_left_recursion_char )
            {
                std::vector< std::string > rightSides = m_grammers[j].rightSides;
                m_grammers[j].rightSides.clear();
                char newgrammerleftside = findNewLeftSideChar();
                if( newgrammerleftside == '0' )
                {
                    std::cout<<"LeftSide Character Limitation Exceeded. Programm Will now Close!"<<std::endl;
                    exit(1);
                }
                for( unsigned int k = 0 ; k < rightSides.size() ; k++ )
                {
                    if( rightSides[k] == "0" )
                    {
                        std::string newgrammerstr = "";
                        newgrammerstr += newgrammerleftside;
                        m_grammers[j].rightSides.push_back( newgrammerstr );
                        m_grammers[j].rightSides.push_back( "0" );
                        continue;
                    }

                    if( rightSides[k][0] == this_left_recursion_char )
                    {
                        std::string firstrightside = rightSides[k];
                        firstrightside = firstrightside.erase(0,1) + this_left_recursion_char;
                        Grammer g;
                        g.leftSide = newgrammerleftside;
                        g.rightSides.push_back( firstrightside );
                        g.rightSides.push_back("0");
                        m_grammers.push_back(g);
                        continue;
                    }

                    std::string newrightside = rightSides[k];
                    newrightside += newgrammerleftside;
                    m_grammers[j].rightSides.push_back( newrightside );
                }
            }
        }
    }

    m_left_recursion_variables.clear();
}

void
Compiler::makeWithLeftFactoring()
{
    //inja ro anjam nadadam
    /*
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( ! m_grammers[i].leftFactoring )
            continue;
        std::vector< std::string >rightSides = m_grammers[i].rightSides;
        std::vector< std::vector< std::string > >selection;
        for( unsigned int j = 0 ; j < rightSides.size() - 1 ; j++ )
        {
            std::vector< std::string >vect;
            for( unsigned int k = j + 1 ; k < rightSides.size() ; k++ )
            {
                if( rightSides[j][0] == rightSides[k][0] )
                {
                    vect.push_back(  );
                }
            }
            selection.push_back( vect );
        }
    }
    */
}

char
Compiler::findNewLeftSideChar()
{
    const char available_left_sides[] = { 'A' , 'B' , 'C' , 'D' , 'E' , 'F' , 'G' , 'H' , 'I' , 'J' , 'K' ,
                                          'L' , 'M' , 'N' , 'O' , 'P' , 'Q' , 'R' , 'S' , 'T' , 'U' , 'V' ,
                                          'W' , 'X' , 'Y' , 'Z'
                                        };

    const unsigned int available_left_sides_number = 26;

    if( m_grammers.size() >= available_left_sides_number )
        return '0';

    char current_left_sides [ m_grammers.size() ];

    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
        current_left_sides[i] = m_grammers[i].leftSide;


    for( unsigned int i = 0 ; i < m_grammers.size() - 1 ; i++ )
    {
        for( unsigned int j = i + 1 ; j < m_grammers.size() ; j++ )
        {
            if( current_left_sides[i] > current_left_sides[j] )
            {
                char tmp = current_left_sides[i];
                current_left_sides[i] = current_left_sides[j];
                current_left_sides[j] = tmp;
            }
        }
    }

    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( current_left_sides[i] != available_left_sides[i] )
            return available_left_sides[i];
    }

    return '0';
}

void
Compiler::outputWhyNotLL1()
{
    int counter = 1;
    std::cout<<"Because Grammer has:"<<std::endl;
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( m_grammers[i].containsEpsilon )
        {
            std::cout<<counter<<". Epsilons"<<std::endl;
            counter++;
            break;
        }
    }
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( m_grammers[i].leftFactoring )
        {
            std::cout<<counter<<". Left Factorings"<<std::endl;
            counter++;
            break;
        }
    }
    for( unsigned int i = 0 ; i < m_grammers.size() ; i++ )
    {
        if( m_grammers[i].leftRecursion )
        {
            std::cout<<counter<<". Left Recursions"<<std::endl;
            break;
        }
    }

    std::cout<<std::endl;
}

void
Compiler::removeLastGrammer()
{
    m_grammers_container.pop_back();
}
