#pragma once

#include <string>
#include <fstream>
#include <iterator>

class CSVWriter
{
    public:
        CSVWriter(std::string filename, std::string delim = ",") : lines(0), filename(filename), delimeter(delimeter){}
        
        template<class T>
        void addDatainRow(T first, T last)
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
        std::string filename;
        std::string delimeter;
        int lines;
        
        
};
