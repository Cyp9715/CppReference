#include "cyp_command.hpp"

#include <cstdio>
#include <cstdlib>

namespace cyp
{
	namespace command
	{
		void SystemCmd(const std::string& command)
		{
			std::system(command.c_str());
		}

		void PopenCmd(const std::string& command)
		{
			char buffer[128];
			FILE* pipe = _popen(command.c_str(), "r");
			if (!pipe) throw std::runtime_error("popen() failed");
			while (fgets(buffer, sizeof buffer, pipe) != NULL)
			{
			}
			_pclose(pipe);
		}

		std::string GetStrPopenCmd(const std::string& command)
		{
			char buffer[128];
			std::string result = "";
			FILE* pipe = _popen(command.c_str(), "r");
			if (!pipe) throw std::runtime_error("popen() failed!");
			try
			{
				while (fgets(buffer, sizeof buffer, pipe) != NULL)
				{
					result += buffer;
				}
			}
			catch (...)
			{
				_pclose(pipe);
				throw;
			}
			_pclose(pipe);
			return result;
		}
	}
}
