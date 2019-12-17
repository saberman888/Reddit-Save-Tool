#include "csv.hpp"

template<class T>
void CSVWriter::addDatainRow(T first, T last)
{
    std::fstream file;
    file.open(filename, std::ios::out | (lines ? std::ios::app : std::ios::trunc));
    
    for(; first != last;)
    {
        file << *first;
        if(++first != last)
            file << delimeter;
    }
    file << "\n";
    lines++;
    file.close();
}
