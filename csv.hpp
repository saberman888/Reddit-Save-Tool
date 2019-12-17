#pragma once

#include <string>
#include <fstream>
#include <iterator>

class CSVWriter
{
    public:
        CSVWriter(std::string filename, std::string delim = ",") : lines(0), filename(filename), delimeter(delimeter){}
        
        template<class T>
        void addDatainRow(T first, T last);
        
        std::string filename;
        std::string delimeter;
        int lines;
        
        
};
