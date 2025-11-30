#include "ScreenScaner.h"
#include <atlimage.h>
#include <atlsafe.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <leptonica/allheaders.h>
#include <stdlib.h>
#include <tesseract/baseapi.h>
#include <tgbot/Bot.h>
#include <vector>
#include <Windows.h>


using namespace std;


ScreenScaner::ScreenScaner()
{
	// get screen dimensions
	x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
	y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
	x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	w = x2 - x1;
	h = y2 - y1;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::Status startupStatus = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (startupStatus != Gdiplus::Ok) {
		cout << "Ошибка инициализации GDI+" << endl;
		return;
	}

}

bool ScreenScaner::checkPixel(int x, int y, int R, int G, int B)
{
	HDC hScreen = GetDC(nullptr);
	COLORREF c = GetPixel(hScreen, x, y);
	ReleaseDC(nullptr, hScreen);
	if ((int)GetRValue(c) == R && (int)GetGValue(c) == G && (int)GetBValue(c) == B)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ScreenScaner::checkPixel(int x, int y, int R, int G, int B, int delta)
{
	HDC hScreen = GetDC(nullptr);
	COLORREF c = GetPixel(hScreen, x, y);
	ReleaseDC(nullptr, hScreen);
	if (abs((int)GetRValue(c) - R) <= delta && abs((int)GetGValue(c) - G) <= delta && abs((int)GetBValue(c) - B) <= delta)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ScreenScaner::end()
{
	ReleaseDC(NULL, this->hScreen);
	Gdiplus::GdiplusShutdown(gdiplusToken);
	cout << "Screenshot saved as Screenshot.png" << endl;
}

void ScreenScaner::makeScreenshot(bool onlywhite)
{
	std::string path = "./Screenshot.png"; // Шлях до файлу, куди буде збережений скріншот

	// Створюємо сумісний контекст пристрою (DC) для екрану
	HDC hScreen = GetDC(nullptr);
	HDC hDC = CreateCompatibleDC(hScreen);

	// Створюємо сумісний bitmap з розмірами екрану
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, screenX, screenY);
	if (hBitmap == NULL) {
		std::cerr << "Fail during creating Bitmap" << std::endl;
	}

	// Зберігаємо старий об'єкт, пов'язаний з DC, та вибираємо новий bitmap
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);

	// Копіюємо вміст екрану в bitmap
	BitBlt(hDC, 0, 0, screenX, screenY, hScreen, 0, 0, SRCCOPY);

	// Підготуємо буфер для зберігання даних зображення
	std::vector<BYTE> buf;
	IStream* stream = NULL;
	HRESULT hr = CreateStreamOnHGlobal(0, TRUE, &stream);
	if (hr != S_OK) {
		std::cerr << "Fail during saving image: " << hr << std::endl;
	}
	CImage image;
	ULARGE_INTEGER liSize;

	// Приєднуємо bitmap до об'єкту CImage
	image.Attach(hBitmap);

	//Перебор пикселей
	cout << "Cheking pixels" << endl;
	if (onlywhite)
	{
		for (int y = 0; y < screenY; ++y) {
			for (int x = 0; x < screenX; ++x) {
				COLORREF color = image.GetPixel(x, y);
				BYTE red = GetRValue(color);
				BYTE green = GetGValue(color);
				BYTE blue = GetBValue(color);

				// Проверяем, если цвет не белый (255, 255, 255), перекрашиваем в черный (0, 0, 0)
				if (red != 255 || green != 255 || blue != 255) {
					image.SetPixel(x, y, RGB(0, 0, 0));
				}
			}
		}
	}

	// Зберігаємо зображення у форматі JPEG у потік
	image.Save(stream, Gdiplus::ImageFormatPNG);

	// Отримуємо розмір потоку
	IStream_Size(stream, &liSize);
	DWORD len = liSize.LowPart;

	// Скидаємо вказівник потоку на початок
	IStream_Reset(stream);

	// Змінюємо розмір буфера відповідно до розміру потоку
	buf.resize(len);

	// Читаємо дані з потоку в буфер
	IStream_Read(stream, &buf[0], len);

	// Звільняємо потік
	stream->Release();

	// Відкриваємо файл для запису в бінарному режимі
	std::fstream fi;
	fi.open(path, std::fstream::binary | std::fstream::out);
	if (!fi.is_open()) {
		std::cerr << "Faillure during opening file" << std::endl;
	}

	// Записуємо дані з буфера у файл
	fi.write(reinterpret_cast<const char*>(&buf[0]), buf.size() * sizeof(BYTE));

	// Закриваємо файл
	fi.close();

	// Відновлюємо старий об'єкт, пов'язаний з DC, та очищаємо створені об'єкти
	SelectObject(hDC, old_obj);
	DeleteObject(hBitmap);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);

}

// Получение CLSID для кодека PNG
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	UINT num = 0;
	UINT size = 0;

	// Получаем количество кодеков и их общий размер
	Gdiplus::Status status = Gdiplus::GetImageEncodersSize(&num, &size);
	if (status != Gdiplus::Ok || num == 0 || size == 0) {
		std::cerr << "Ошибка получения размера кодеков: " << status << std::endl;
		return -1;
	}

	// Выделяем память под кодеки
	Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)malloc(size);
	if (pImageCodecInfo == NULL) {
		std::cerr << "Ошибка выделения памяти для кодеков." << std::endl;
		return -1;
	}

	// Получаем информацию о кодеках
	status = Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
	if (status != Gdiplus::Ok) {
		std::cerr << "Ошибка получения кодеков: " << status << std::endl;
		free(pImageCodecInfo);  // Не забываем освобождать память
		return -1;
	}

	// Ищем кодек с нужным MIME типом
	for (UINT j = 0; j < num; ++j) {
		if (pImageCodecInfo[j].MimeType != NULL && wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Кодек найден
		}
	}

	std::cerr << "Кодек с MIME типом " << format << " не найден." << std::endl;
	free(pImageCodecInfo);  // Освобождаем память
	return -1;  // Кодек не найден
}

std::string ScreenScaner::makeText(bool eng, int x, int y, int width, int height, int R, int G, int B, int Rmax, int Gmax, int Bmax)
{
	// Получаем размеры экрана
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	// Создаем устройство для захвата экрана
	HDC hScreen = GetDC(NULL);
	if (!hScreen) {
		cout << "Ошибка получения HDC экрана" << endl;
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return "";
	}

	HDC hDC = CreateCompatibleDC(hScreen);
	if (!hDC) {
		cout << "Ошибка создания совместимого HDC" << endl;
		ReleaseDC(NULL, hScreen);
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return "";
	}

	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, screen_width, screen_height);
	if (!hBitmap) {
		cout << "Ошибка создания HBITMAP" << endl;
		DeleteDC(hDC);
		ReleaseDC(NULL, hScreen);
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return "";
	}

	SelectObject(hDC, hBitmap);

	// Копируем изображение с экрана
	if (!BitBlt(hDC, 0, 0, screen_width, screen_height, hScreen, 0, 0, SRCCOPY)) {
		cout << "Ошибка копирования изображения с экрана" << endl;
		DeleteObject(hBitmap);
		DeleteDC(hDC);
		ReleaseDC(NULL, hScreen);
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return "";
	}

	// Получаем пиксели изображения
	BITMAP bm;
	if (!GetObject(hBitmap, sizeof(BITMAP), &bm)) {
		cout << "Ошибка получения объекта BITMAP" << endl;
		DeleteObject(hBitmap);
		DeleteDC(hDC);
		ReleaseDC(NULL, hScreen);
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return "";
	}

	BITMAPINFOHEADER bi = { sizeof(BITMAPINFOHEADER), screen_width, -screen_height, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	HDC hMemDC = CreateCompatibleDC(hDC);
	if (!hMemDC) {
		cout << "Ошибка создания совместимого DC для памяти" << endl;
		DeleteObject(hBitmap);
		DeleteDC(hDC);
		ReleaseDC(NULL, hScreen);
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return "";
	}
	SelectObject(hMemDC, hBitmap);

	BYTE* pPixels = new BYTE[bm.bmWidth * bm.bmHeight * 4];
	if (!GetDIBits(hMemDC, hBitmap, 0, screen_height, pPixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS)) {
		cout << "Ошибка получения битов изображения" << endl;
		delete[] pPixels;
		DeleteDC(hMemDC);
		DeleteObject(hBitmap);
		DeleteDC(hDC);
		ReleaseDC(NULL, hScreen);
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return "";
	}

	// Обрабатываем пиксели (замена всех не-белых на черный, если они не входят в диапазон 200-255)
	for (int y = 0; y < screen_height; ++y) {
		for (int x = 0; x < screen_width; ++x) {
			BYTE* pixel = pPixels + (y * screen_width + x) * 4; // BGRA

			// Проверяем, входят ли компоненты пикселя в диапазон 200-255
			bool isWithinRange = (pixel[0] >= B) && (pixel[0] <= Bmax) && // B (Blue)
				(pixel[1] >= G) && (pixel[1] <= Gmax) && // G (Blue)
				(pixel[2] >= R) && (pixel[2] <= Rmax); // R (Blue)

			// Если хотя бы одна из компонент не в диапазоне, перекрашиваем пиксель в черный
			if (!isWithinRange) {
				pixel[0] = 0;  // Устанавливаем в черный (B)
				pixel[1] = 0;  // Устанавливаем в черный (G)
				pixel[2] = 0;  // Устанавливаем в черный (R)
			}
		}
	}


	// Записываем обратно пиксели
	if (!SetDIBits(hMemDC, hBitmap, 0, screen_height, pPixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS)) {
		cout << "Ошибка установки битов изображения" << endl;
	}

	// Сохраняем изображение как PNG
	Gdiplus::Bitmap bitmap(hBitmap, NULL);
	CLSID pngClsid;
	if (GetEncoderClsid(L"image/png", &pngClsid) == -1) {
		cout << "Ошибка получения кодека PNG" << endl;
	}
	else {
		Gdiplus::Status saveStatus = bitmap.Save(L"Screenshot.png", &pngClsid, NULL);
		if (saveStatus != Gdiplus::Ok) {
			cout << "Ошибка сохранения изображения как PNG" << endl;
		}
	}

	//Получаем текст
	string result;
	try
	{
		tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
		// Initialize tesseract-ocr with English, without specifying tessdata path
		if (!eng)
		{
			while (api->Init(NULL, "rus"))
			{
				Sleep(1000);
				cerr << "Failed to init RUS tesseract!" << endl;
			}
		}
		else
		{
			while (api->Init(NULL, "eng"))
			{
				Sleep(1000);
				cerr << "Failed to init ENG tesseract!" << endl;
			}
		}

		// Open input image with leptonica library
		Pix* image = pixRead("Screenshot.png");
		api->SetImage(image);
		// Restrict recognition to a sub-rectangle of the image
		// SetRectangle(left, top, width, height)
		if (width != 0 && height != 0)
		{
			api->SetRectangle(x, y, width, height);
		}
		// Get OCR result
		result = api->GetUTF8Text();
		pixDestroy(&image);
		api->End();
		delete api;
	}
	catch (const std::exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		result = (string)"Error: " + e.what();
	}
	
	// Очистка ресурсов
	delete[] pPixels;
	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);

	return result;
}

/*public: int tryRunLottery()
{
	int x1, y1, x2, y2, w, h;
	POINT cursor;
	HDC     hScreen = GetDC(NULL);
	// get screen dimensions
	x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
	y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
	x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	w = x2 - x1;
	h = y2 - y1;
	int timer = 0;
	cout << "w = " << w << '\n';
	cout << "h = " << h << '\n';
	Sleep(2000);
	while (timer < 10) {
		GetCursorPos(&cursor);
		COLORREF c = GetPixel(hScreen, cursor.x, cursor.y);
		cout << "\nx : " << cursor.x;
		cout << "\ny : " << cursor.y << "\n";
		cout << "(";
		cout << (int)GetRValue(c) << ", ";
		cout << (int)GetGValue(c) << ", ";
		cout << (int)GetBValue(c) << ")";
		timer++;
		Sleep(2000);
	}

	// copy screen to bitmap

	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);



	// save bitmap to clipboard
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);
	CloseClipboard();

	// clean up
	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
	DeleteObject(hBitmap);
}*/




