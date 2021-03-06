#include "cyp_command.hpp"

namespace cyp
{
	namespace command
	{
		void SystemCmd(const std::string& command)
		{
			system(command.c_str());
		}

		void PopenCmd(const std::string& command)
		{
			FILE* pipe = _popen(command.c_str(), "r");
			if (!pipe) throw std::runtime_error("popen() failed");
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