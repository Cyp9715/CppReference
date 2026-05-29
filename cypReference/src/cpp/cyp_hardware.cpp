#include "cyp_hardware.hpp"

namespace cyp
{
	namespace hardware
	{
		Monitor::Monitor()
		{
			ZeroMemory(&displayDevice, sizeof(displayDevice));
			ZeroMemory(&devmode, sizeof(devmode));
			displayDevice.cb = sizeof(displayDevice);
			devmode.dmSize = sizeof(devmode);
		}

		Monitor::~Monitor()
		{
		}

		std::vector<Monitor::Info> Monitor::GetDisplayInfo()
		{
			Info info;
			v_info.clear();

			int deviceIndex = 0;
			
			while (EnumDisplayDevices(NULL, deviceIndex++, &displayDevice, 0))
			{
				std::string deviceName = displayDevice.DeviceName;

				int monitorIndex = 0;
				DISPLAY_DEVICE monitorDevice;
				ZeroMemory(&monitorDevice, sizeof(monitorDevice));
				monitorDevice.cb = sizeof(monitorDevice);

				while (EnumDisplayDevices(deviceName.c_str(), monitorIndex++, &monitorDevice, 0))
				{
					ZeroMemory(&devmode, sizeof(devmode));
					devmode.dmSize = sizeof(devmode);

					if (!EnumDisplaySettings(deviceName.c_str(), ENUM_REGISTRY_SETTINGS, &devmode))
					{
						ZeroMemory(&monitorDevice, sizeof(monitorDevice));
						monitorDevice.cb = sizeof(monitorDevice);
						continue;
					}

					info.monitorName = monitorDevice.DeviceString;
					info.bitsPerPel = devmode.dmBitsPerPel;
					info.pelsWidth = devmode.dmPelsWidth;
					info.pelsHeight = devmode.dmPelsHeight;
					info.displayFlags = devmode.dmDisplayFlags;
					info.displayFrequency = devmode.dmDisplayFrequency;
					info.position_x = devmode.dmPosition.x;
					info.position_y = devmode.dmPosition.y;

					v_info.push_back(info);

					ZeroMemory(&monitorDevice, sizeof(monitorDevice));
					monitorDevice.cb = sizeof(monitorDevice);
				}

				ZeroMemory(&displayDevice, sizeof(displayDevice));
				displayDevice.cb = sizeof(displayDevice);
			}
			
			return v_info;
		}
	}
}
