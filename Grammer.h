#ifndef GRAMMER_H_INCLUDED
#define GRAMMER_H_INCLUDED

class Grammer
{
    public:
        char leftSide;
        std::vector< std::string> rightSides;
        bool containsEpsilon;
        bool leftRecursion;
        bool leftFactoring;




};

#endif // GRAMMER_H_INCLUDED
