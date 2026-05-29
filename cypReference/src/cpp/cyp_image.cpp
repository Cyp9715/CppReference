#include "cyp_image.hpp"

#include <cstdlib>

namespace cyp
{
	namespace image
	{
		int ScreenShot::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
		{
			UINT  num = 0;          // number of image encoders
			UINT  size = 0;         // size of the image encoder array in bytes

			Gdiplus::GetImageEncodersSize(&num, &size);
			if (size == 0)
				return -1;  // Failure

			Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(std::malloc(size));
			if (pImageCodecInfo == NULL)
				return -1;  // Failure

			if (GetImageEncoders(num, size, pImageCodecInfo) != Gdiplus::Ok)
			{
				std::free(pImageCodecInfo);
				return -1;
			}

			for (UINT j = 0; j < num; ++j)
			{
				if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
				{
					*pClsid = pImageCodecInfo[j].Clsid;
					std::free(pImageCodecInfo);
					return j;  // Success
				}
			}

			std::free(pImageCodecInfo);
			return -1;  // Failure
		}

		void ScreenShot::Capture(std::string filePath_, imageType type, UINT startX, UINT startY, UINT endX, UINT endY)
		{
			if (endX <= startX) return;
			if (endY <= startY) return;

			int w = static_cast<int>(endX - startX);
			int h = static_cast<int>(endY - startY);

			std::wstring temp_wString = std::wstring(filePath_.begin(), filePath_.end());
			const wchar_t* filePath = temp_wString.c_str();

			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			ULONG_PTR gdiplusToken;
			if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok)
			{
				throw std::runtime_error("GdiplusStartup failed");
			}

			HDC hScreen = NULL;
			HDC hDc = NULL;
			HBITMAP hBitmap = NULL;
			HGDIOBJ old_obj = NULL;

			auto cleanup = [&]() -> void
				{
					if (old_obj && hDc)
					{
						SelectObject(hDc, old_obj);
					}

					if (hBitmap)
					{
						DeleteObject(hBitmap);
					}

					if (hDc)
					{
						DeleteDC(hDc);
					}

					if (hScreen)
					{
						ReleaseDC(HWND_DESKTOP, hScreen);
					}

					Gdiplus::GdiplusShutdown(gdiplusToken);
				};

			hScreen = GetDC(HWND_DESKTOP);
			if (!hScreen)
			{
				cleanup();
				throw std::runtime_error("GetDC failed");
			}

			hDc = CreateCompatibleDC(hScreen);
			hBitmap = CreateCompatibleBitmap(hScreen, w, h);
			if (!hDc || !hBitmap)
			{
				cleanup();
				throw std::runtime_error("CreateCompatibleDC or CreateCompatibleBitmap failed");
			}

			old_obj = SelectObject(hDc, hBitmap);
			if (!old_obj)
			{
				cleanup();
				throw std::runtime_error("SelectObject failed");
			}

			if (!BitBlt(hDc, 0, 0, w, h, hScreen, startX, startY, SRCCOPY))
			{
				cleanup();
				throw std::runtime_error("BitBlt failed");
			}

			CLSID clsid;
			int encoderIndex = -1;

			switch (type)
			{
			case imageType::bmp:
				encoderIndex = GetEncoderClsid(L"image/bmp", &clsid);
				break;
			case imageType::png:
				encoderIndex = GetEncoderClsid(L"image/png", &clsid);
				break;
			}

			if (encoderIndex < 0)
			{
				cleanup();
				throw std::runtime_error("image encoder not found");
			}

			//GDI+ expects Unicode filenames
			Gdiplus::Status saveStatus = Gdiplus::GenericError;
			{
				Gdiplus::Bitmap bitmap(hBitmap, NULL);
				saveStatus = bitmap.Save(filePath, &clsid);
			}

			cleanup();

			if (saveStatus != Gdiplus::Ok)
			{
				throw std::runtime_error("bitmap.Save failed");
			}
		}

	}
}
