#include "sbjson.hpp"

namespace SBJSON
{
	bool TryGetBool(const nlohmann::json& data, std::string tag)
	{
		if(data.contains(tag))
		{
			if(!data.at(tag).is_null())
			{
				return data.at(tag).get<bool>();
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

}
