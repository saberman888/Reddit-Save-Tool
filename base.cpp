#include "base.hpp"
#include <iostream>
namespace RST
{
	bool Write(std::filesystem::path destination, const std::string& buffer) {

		std::ofstream out;
		// Prepare an exeption incase opening fails
		std::ios_base::iostate exceptionMask = out.exceptions() | std::ios::failbit;
		out.exceptions(exceptionMask);
		try {
			out.open(destination.string(), std::ios::out | std::ios::binary);
			out << buffer;
			return true;
		}
		catch (std::system_error& e) {
			std::cerr << e.what() << std::endl;
			throw;
		}
		return false;
			
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
	int Progress(void* ptr, double TotalDownloaded, double NowDownloaded, double TotalToDownload)
	{
		return 1;
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
			Input.replace(Input.find(ToBeReplaced), ToBeReplaced.size(), Replacement);
		}
		return Input;
	}


}
