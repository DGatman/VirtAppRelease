#pragma once
#define _SCREENSCANER_H_
#include <iostream>
#include <Windows.h>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <atlimage.h>
#include <atlsafe.h>
#include <ctime>
#include <tgbot/Bot.h>
#pragma comment(lib, "winmm.lib")

#include <Gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")


#include <vector>
class ScreenScaner {
	int x1, y1, x2, y2, w, h;
	HDC     hScreen;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::Status startupStatus;
public:
	ScreenScaner();
	bool checkPixel(int x, int y, int R, int G, int B);
	bool checkPixel(int x, int y, int R, int G, int B, int delta);
	void end();
	void makeScreenshot(bool onlywhite = false);
	std::string makeText(bool eng = true, int x = 0, int y = 0, int width = 0, int height = 0, int R = 200, int G = 200, int B = 200, int Rmax = 256, int Gmax = 256, int Bmax = 256);
};
