#include "base.hpp"

namespace RST
{
  bool Write(std::filesystem::path destination, const std::string& buffer){
    std::ofstream out(destination.string(), std::ios::out | std::ios::binary);
    if(out.good()){
      out << buffer;
      return true;
    } else {
      return false;
    }
  }

  std::vector<std::string> splitString(std::string data, char delimeter)
  {
    std::string temp;
    std::vector<std::string> returnList;
    for(auto ch : data){
      if(ch == delimeter){
        returnList.push_back(temp);
        temp.clear();
      } else {
        temp += ch;
      }
    }
    return returnList;
  }

  State Download(const std::string URL){
    BasicRequest Handle;
    Handle.Setup(URL);
    State result = Handle.SendRequest();
    Handle.Cleanup();
    return result;
  }

}
