#include "base.hpp"

namespace RST
{
	bool Write(std::filesystem::path destination, const std::string& buffer) {
		std::ofstream out(destination.string(), std::ios::out | std::ios::binary);
		if (!out.good()) {
			return false;
		}
		else {
			out << buffer;
			return true;
		}
	}

	std::vector<std::string> splitString(std::string data, char delimeter)
	{
		std::string temp;
		std::vector<std::string> returnList;
		std::stringstream ss(data);

		while (std::getline(ss, temp, delimeter))
		{
			returnList.push_back(temp);
		}
		return returnList;
	}

	State Download(const std::string URL) {
		BasicRequest Handle;
		Handle.Setup(URL);
		Handle.SetOpt(CURLOPT_FOLLOWLOCATION, 1L);
		State result = Handle.SendRequest();
		Handle.Cleanup();
		return result;
	}
	std::string SearchAndReplace(std::string Input, const std::string ToBeReplaced, const std::string Replacement)
	{
		if (Input.find(ToBeReplaced) != std::string::npos)
		{
			Input.replace(Input.find(ToBeReplaced), ToBeReplaced.size() - 1, Replacement);
		}
		return Input;
	}


}
