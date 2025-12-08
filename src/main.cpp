#include "utils.h"
#include <conio.h>
#include <fstream>
#include <interception.h>
#include <iostream>
#include <regex>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wtsapi32.lib")
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <tgbot/tgbot.h>
#include <ctime>
#include <csignal>
#include <exception>
#include <cstdlib>
#include <cstdio>
#include <lmcons.h>
#include <WtsApi32.h>
#include <vector>
#include <Psapi.h>
// #include <sqlite_modern_cpp.h>  // Не используется в коде
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "VirtApp.h"
#include "ScreenScaner.h"
#include "DashboardUI.h"  // Dashboard UI v3.0
#include <string>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <mailio/imap.hpp>
#include <array>
#include <filesystem>
#include <algorithm>
#include <codecvt>


using namespace TgBot;
using namespace std;


enum scancode {
	v = 0x2F,
	n = 0x31,
	space = 0x39,
	a = 0x1E,
	g = 0x22,
	u = 0x16,
	i = 0x17,
	b = 0x30,
	d1 = 0x2,
	d3 = 0x4,
	d7 = 0x8,
	d2 = 0x3,
	d8 = 0x9,
	shift = 0x2A,
	f1 = 0x3B,
	f2 = 0x3C,
	F7 = 0x41,
	f10 = 0x44,
	w = 0x11,
	s = 0x1F,
	o = 0x18,
	esc = 0x01,
	ctrl = 0x1D,
	alt = 0x38,
	del = 0x53,
	capslock = 0x3A,
	upArrow = 0x48,
	leftButton = 1,
	bksp = 0x0E,
	d5 = 0x6,
	d0 = 0x0B,
	tab = 0x0F,
	enter = 0x1C,
	slash = 0x35
};
const char* userProfile = std::getenv("USERPROFILE");

vector<string> tgListLoging =
{
	"517005065"	//	Гатман @DGatman
};

vector<string> tgListSecurity =
{
	"517005065",	//	Гатман		@DGatman
	//"417320201",	//	EnoTiK11m	@Ferixis
	"6667819992",	//	LeetProxy	@leetproxy
	"7381075796",	//	VirtEmpire	@VirtEmpire_Support
	"627722189",	//	Владос		@VIaDDoS
	"700665502",	//	Nifilim		@Nifilim1337
	"6495114020",	//	Yuth		@Yuth_01
	"8200636752"	//	Артем		@xakwg
};

map<string, string> stickers =
{
	{"no_permission","CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE"},
	{"unknown_command","CAACAgEAAxkBAAExxZ1nsGErheezM-1Ok74esUVG6pUBNgAChQEAAnY3dj-p9DVU0dcpYzYE"}
};

map<string, string> servers =
{
	{"01.Downtown","v3-downtown.gta5rp.com"},
	{"02.Strawberry","v3-strawberry.gta5rp.com"},
	{"03.Vinewood","v3-vinewood.gta5rp.com"},
	{"04.Blackberry","v3-blackberry.gta5rp.com"},
	{"05.Insquad","v3-insquad.gta5rp.com"},
	{"06.Sunrise","v3-sunrise.gta5rp.com"},
	{"07.Rainbow","v3-rainbow.gta5rp.com"},
	{"08.Richman","v3-richman.gta5rp.com"},
	{"09.Eclipse","v3-eclipse.gta5rp.com"},
	{"10.LaMesa","v3-lamesa.gta5rp.com"},
	{"11.Burton","v3-burton.gta5rp.com"},
	{"12.Rockford","v3-rockford.gta5rp.com"},
	{"13.Alta","v3-alta.gta5rp.com"},
	{"14.DelPerro","v3-delperro.gta5rp.com"},
	{"15.Davis","v3-davis.gta5rp.com"},
	{"16.Harmony","v3-harmony.gta5rp.com"},
	{"17.Redwood","v3-redwood.gta5rp.com"},
	{"18.Hawick","v3-hawick.gta5rp.com"},
	{"19.Grapeseed","v3-grapeseed.gta5rp.com"},
	{"20.Murrieta","v3-murrieta.gta5rp.com"},
	{"21.Vespucci","v3-vespucci.gta5rp.com"},
	{"22.Milton","v3-milton.gta5rp.com"}
};

string logHistory;

InterceptionKeyStroke
enter_down = { scancode::enter,INTERCEPTION_KEY_DOWN,0 },
enter_up = { scancode::enter,INTERCEPTION_KEY_UP,0 },
tab_down = { scancode::tab,INTERCEPTION_KEY_DOWN,0 },
tab_up = { scancode::tab,INTERCEPTION_KEY_UP,0 },
d5_down = { scancode::d5,INTERCEPTION_KEY_DOWN,0 },
d5_up = { scancode::d5,INTERCEPTION_KEY_UP,0 },
d0_down = { scancode::d0,INTERCEPTION_KEY_DOWN,0 },
d0_up = { scancode::d0,INTERCEPTION_KEY_UP,0 },
bksp_down = { scancode::bksp,INTERCEPTION_KEY_DOWN,0 },
bksp_up = { scancode::bksp,INTERCEPTION_KEY_UP,0 },
del_down = { scancode::del,INTERCEPTION_KEY_DOWN,0 },
del_up = { scancode::del,INTERCEPTION_KEY_UP,0 },
alt_down = { scancode::alt,INTERCEPTION_KEY_DOWN,0 },
alt_up = { scancode::alt,INTERCEPTION_KEY_UP,0 },
v_down = { scancode::v,INTERCEPTION_KEY_DOWN,0 },
n_down = { scancode::n,INTERCEPTION_KEY_DOWN,0 },
space_down = { scancode::space,INTERCEPTION_KEY_DOWN,0 },
a_down = { scancode::a,INTERCEPTION_KEY_DOWN,0 },
g_down = { scancode::g,INTERCEPTION_KEY_DOWN,0 },
u_down = { scancode::u,INTERCEPTION_KEY_DOWN,0 },
i_down = { scancode::i,INTERCEPTION_KEY_DOWN,0 },
slash_down = { scancode::slash,INTERCEPTION_KEY_DOWN,0 },
b_down = { scancode::b,INTERCEPTION_KEY_DOWN,0 },
d1_down = { scancode::d1,INTERCEPTION_KEY_DOWN,0 },
d3_down = { scancode::d3,INTERCEPTION_KEY_DOWN,0 },
d7_down = { scancode::d7,INTERCEPTION_KEY_DOWN,0 },
d2_down = { scancode::d2,INTERCEPTION_KEY_DOWN,0 },
d8_down = { scancode::d8,INTERCEPTION_KEY_DOWN,0 },
shift_down = { scancode::shift,INTERCEPTION_KEY_DOWN,0 },
f1_down = { scancode::f1,INTERCEPTION_KEY_DOWN,0 },
f2_down = { scancode::f2,INTERCEPTION_KEY_DOWN,0 },
o_down = { scancode::o, INTERCEPTION_KEY_DOWN,0 },
upArrow_down = { scancode::upArrow, INTERCEPTION_KEY_DOWN, 0 },
s_down = { scancode::s      , INTERCEPTION_KEY_DOWN, 0 },
F7_down = { scancode::F7      , INTERCEPTION_KEY_DOWN, 0 },
f10_down = { scancode::f10,INTERCEPTION_KEY_DOWN,0 },
w_down = { scancode::w      , INTERCEPTION_KEY_DOWN, 0 },
esc_down = { scancode::esc     , INTERCEPTION_KEY_DOWN, 0 },
ctrl_down = { scancode::ctrl    , INTERCEPTION_KEY_DOWN, 0 },
capslock_down = { scancode::capslock, INTERCEPTION_KEY_DOWN, 0 },
o_up = { scancode::o, INTERCEPTION_KEY_UP,0 },
esc_up = { scancode::esc     , INTERCEPTION_KEY_UP  , 0 },
ctrl_up = { scancode::ctrl    , INTERCEPTION_KEY_UP  , 0 },
capslock_up = { scancode::capslock, INTERCEPTION_KEY_UP  , 0 },
F7_up = { scancode::F7, INTERCEPTION_KEY_UP  , 0 },
f10_up = { scancode::f10,INTERCEPTION_KEY_UP,0 },
f2_up = { scancode::f2,INTERCEPTION_KEY_UP,0 },
s_up = { scancode::s      , INTERCEPTION_KEY_UP, 0 },
upArrow_up = { scancode::upArrow      , INTERCEPTION_KEY_UP, 0 },
w_up = { scancode::w, INTERCEPTION_KEY_UP  , 0 },
n_up = { scancode::n,INTERCEPTION_KEY_UP,0 },
a_up = { scancode::a,INTERCEPTION_KEY_UP,0 },
g_up = { scancode::g,INTERCEPTION_KEY_UP,0 },
space_up = { scancode::space,INTERCEPTION_KEY_UP,0 },
u_up = { scancode::u,INTERCEPTION_KEY_UP,0 },
i_up = { scancode::i,INTERCEPTION_KEY_UP,0 },
slash_up = { scancode::slash,INTERCEPTION_KEY_UP,0 },
b_up = { scancode::b,INTERCEPTION_KEY_UP,0 },
d1_up = { scancode::d1,INTERCEPTION_KEY_UP,0 },
d3_up = { scancode::d3,INTERCEPTION_KEY_UP,0 },
d7_up = { scancode::d7,INTERCEPTION_KEY_UP,0 },
d2_up = { scancode::d2,INTERCEPTION_KEY_UP,0 },
d8_up = { scancode::d8,INTERCEPTION_KEY_UP,0 },
v_up = { scancode::v,INTERCEPTION_KEY_UP,0 },
shift_up = { scancode::shift,INTERCEPTION_KEY_UP,0 },
f1_up = { scancode::f1,INTERCEPTION_KEY_UP,0 };

bool operator==(const InterceptionKeyStroke& first,
	const InterceptionKeyStroke& second) {
	return first.code == second.code && first.state == second.state;
}

std::string readAmountFile(const std::string& filename) {
	std::ifstream inputFile(filename);
	std::string content;
	if (inputFile.is_open()) {
		inputFile >> content;
		inputFile.close();
	}
	else {
		content = "File was not founded";
	}
	return content;
}

void writeAmountFile(const std::string& filename, int value) {
	std::ofstream outputFile(filename);
	if (outputFile.is_open()) {
		outputFile << value;
		outputFile.close();
	}
}

void createOneColumnKeyboard(const vector<string>& buttonStrings, ReplyKeyboardMarkup::Ptr& kb)
{
	for (size_t i = 0; i < buttonStrings.size(); ++i) {
		vector<KeyboardButton::Ptr> row;
		KeyboardButton::Ptr button(new KeyboardButton);
		button->text = buttonStrings[i];
		row.push_back(button);
		kb->keyboard.push_back(row);
	}
}

string filterDigits(const std::string& input) {
	std::string result;
	for (char c : input) {
		if (std::isdigit(c)) {
			result += c;
		}
	}
	return result;
}






bool isRunningP(LPCWSTR process)
{
	WTS_PROCESS_INFOW* pProcessInfo;
	DWORD               NumProcesses;

	if (NULL == WTSEnumerateProcessesW(WTS_CURRENT_SERVER_HANDLE,
		0, // Reserved.
		1, // Version,
		&pProcessInfo,
		&NumProcesses))
	{
		printf("WTSEnumerateProcessesW failed with last error %lu.\r\n", GetLastError());
		return false;
	}

	bool fCalcFound = false;

	for (DWORD i = 0; i < NumProcesses; ++i)
	{
		if (0 == lstrcmpW(pProcessInfo[i].pProcessName, process))
		{
			fCalcFound = true;
			break;
		}
	}

	WTSFreeMemory(pProcessInfo);

	return fCalcFound;
}


void createKeyboard(const vector<vector<string>>& buttonLayout, ReplyKeyboardMarkup::Ptr& kb)
{
	for (size_t i = 0; i < buttonLayout.size(); ++i) {
		vector<KeyboardButton::Ptr> row;
		for (size_t j = 0; j < buttonLayout[i].size(); ++j) {
			KeyboardButton::Ptr button(new KeyboardButton);
			button->text = buttonLayout[i][j];
			row.push_back(button);
		}
		kb->keyboard.push_back(row);
	}
}

bool isRunning(LPCSTR pName)
{
	HWND hwnd;
	hwnd = FindWindowA(NULL, pName);
	if (hwnd != 0) {
		return true;
	}
	else {
		return false;
	}
}

std::list<std::string> splitStringByLength(const std::string& input, std::size_t partLength) {
	std::list<std::string> parts;
	std::size_t start = 0;

	while (start < input.size()) {
		// Получаем подстроку длиной не более partLength
		std::string part = input.substr(start, partLength);
		parts.push_back(part);
		start += partLength;
	}

	return parts;
}

string logprint(string loginfo, tm* currentTm)
{
	cout << '[' << put_time(currentTm, "%H:%M:%S") << "] " << loginfo << '\n';
	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", currentTm);
	std::string formattedTime(buffer);
	logHistory = logHistory + '[' + formattedTime + "] " + loginfo + '\n';
	std::ofstream logFile("logs.txt", std::ios::app);
	if (logFile.is_open()) {
		// Записываем лог в файл
		logFile << '[' << formattedTime << "] " << loginfo << '\n';
		logFile.close();
	}
	else {
		// Если файл не удалось открыть, обрабатываем ошибку
		cout << '[' << put_time(currentTm, "%H:%M:%S") << "] " << "Error: unable to open logs.txt" << endl;
		logHistory = logHistory + '[' + formattedTime + "] " + "Error: unable to open logs.txt" + '\n';
	}
	return logHistory;
}

void handleGetLogFile(TgBot::Bot& bot, const TgBot::Message::Ptr& message) {
	if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > 60)
	{
		cout << "Ignored command due the timeout!" << endl;
		return;
	}
	if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
	{
		cout << "Ignored command due security settings" << endl;
		bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
		bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
		return;
	}
	const std::string logFilePath = "logs.txt";

	// Проверяем, существует ли файл
	std::ifstream logFile(logFilePath);
	if (logFile.good()) {
		bot.getApi().sendDocument(message->chat->id, TgBot::InputFile::fromFile(logFilePath, "text/plain"));
	}
	else {
		// Если файла нет, отправляем сообщение об ошибке
		bot.getApi().sendMessage(message->chat->id, "Logs.txt wasn't founded.");
	}
}



void mouse_leftClick(InterceptionContext context, InterceptionDevice mouse, int x, int y, bool rightButton = false, int waitdelay = 300, int clickdelay = 150)
{
	InterceptionMouseStroke mstrokes;
	mstrokes.flags = INTERCEPTION_MOUSE_MOVE_ABSOLUTE;
	mstrokes.x = static_cast<int>((0xFFFF * x) / 1280);
	mstrokes.y = static_cast<int>((0xFFFF * y) / 720);
	interception_send(context, mouse, (InterceptionStroke*)&mstrokes, 1);
	this_thread::sleep_for(chrono::milliseconds(waitdelay));
	mstrokes.flags = INTERCEPTION_MOUSE_MOVE_ABSOLUTE;
	if (rightButton)
	{
		mstrokes.state = INTERCEPTION_MOUSE_RIGHT_BUTTON_DOWN;
	}
	else
	{
		mstrokes.state = INTERCEPTION_MOUSE_LEFT_BUTTON_DOWN;
	}
	mstrokes.x = static_cast<int>((0xFFFF * x) / 1280);
	mstrokes.y = static_cast<int>((0xFFFF * y) / 720);
	interception_send(context, mouse, (InterceptionStroke*)&mstrokes, 1);
	this_thread::sleep_for(chrono::milliseconds(clickdelay));
	mstrokes.flags = INTERCEPTION_MOUSE_MOVE_ABSOLUTE;
	if (rightButton)
	{
		mstrokes.state = INTERCEPTION_MOUSE_RIGHT_BUTTON_UP;
	}
	else
	{
		mstrokes.state = INTERCEPTION_MOUSE_LEFT_BUTTON_UP;
	}
	mstrokes.x = static_cast<int>((0xFFFF * x) / 1280);
	mstrokes.y = static_cast<int>((0xFFFF * y) / 720);
	interception_send(context, mouse, (InterceptionStroke*)&mstrokes, 1);
	mstrokes.flags = 0;
	mstrokes.state = 0;
	mstrokes.x = static_cast<int>((0xFFFF * 0) / 1280);
	mstrokes.y = static_cast<int>((0xFFFF * 0) / 720);
	interception_send(context, mouse, (InterceptionStroke*)&mstrokes, 1);

}

vector<InterceptionKeyStroke> caps2esc(const InterceptionKeyStroke& kstroke, bool ha, bool direction) {
	static bool capslock_is_down = true, esc_give_up = false;

	vector<InterceptionKeyStroke> kstrokes;

	while (true) {
		switch (direction)
		{
		case true:
			if (ha) {
				kstrokes.push_back(w_down);
			}
			else {
				kstrokes.push_back(w_up);
			}
			break;
		case false:
			if (ha) {
				kstrokes.push_back(s_down);
			}
			else
			{
				kstrokes.push_back(s_up);
			}
			break;
		}
		break;
	}

	return kstrokes;
}

time_t to_time_t(const boost::local_time::local_date_time& ldt) {
	// Получаем ptime из local_date_time
	boost::posix_time::ptime utc_ptime = ldt.utc_time();  // Преобразуем в UTC

	// Начало эпохи UNIX
	boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));

	// Вычисляем количество секунд с начала эпохи UNIX
	boost::posix_time::time_duration diff = utc_ptime - epoch;

	return diff.total_seconds();  // Преобразуем разницу в секунды
}

void addToClipboard(string str) {
	//Copying to clipboard
	if (OpenClipboard(0)) {
		// Очистить буфер обмена
		EmptyClipboard();

		// Выделить память для текста
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (str.size() + 1) * sizeof(char));

		if (hMem != NULL) {
			char* pMem = static_cast<char*>(GlobalLock(hMem));

			// Копирование текста в выделенную память
			strcpy_s(pMem, str.size() + 1, str.c_str());

			GlobalUnlock(hMem);

			// Поместить текст в буфер обмена
			SetClipboardData(CF_TEXT, hMem);

			// Закрыть буфер обмена
			CloseClipboard();
		}
	}
}

void onConfigCommand(const TgBot::Message::Ptr& message, TgBot::Bot& bot) {
	if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > 60)
	{
		cout << "Ignored command due the timeout!" << endl;
		return;
	}
	if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
	{
		cout << "Ignored command due security settings" << endl;
		bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
		bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
		return;
	}
	std::ifstream configFile("config.txt", std::ios::binary);
	if (configFile.is_open()) {
		std::stringstream buffer;
		buffer << configFile.rdbuf();
		configFile.close();

		// Преобразуем в std::string (UTF-8)
		std::string content = buffer.str();

		// Если содержимое файла содержит не-UTF-8 символы, можно их дополнительно обработать
		cout << "Sending config content" << endl;
		bot.getApi().sendMessage(message->chat->id, content.length() > 1 ? content : "Config is empty");
	}
	else {
		bot.getApi().sendMessage(message->chat->id, "Unable to open config file");
	}
}

//IP CHECK

struct IPRange {
	uint32_t start;
	uint32_t end;

	bool contains(uint32_t ip) const {
		return ip >= start && ip <= end;
	}
};

uint32_t ipToUint(const std::string& ip) {
	struct in_addr addr;
	inet_pton(AF_INET, ip.c_str(), &addr);
	return ntohl(addr.s_addr);
}

IPRange parseRange(const std::string& rangeStr) {
	if (rangeStr.find('-') != std::string::npos) {
		// Диапазон вида: "79.142.197.0 - 79.142.197.255"
		std::regex rangeRegex(R"((\d+\.\d+\.\d+\.\d+)\s*-\s*(\d+\.\d+\.\d+\.\d+))");
		std::smatch match;
		if (std::regex_match(rangeStr, match, rangeRegex)) {
			return { ipToUint(match[1]), ipToUint(match[2]) };
		}
	}
	else if (rangeStr.find('/') != std::string::npos) {
		// Диапазон вида: "79.142.197.0/24"
		std::regex cidrRegex(R"((\d+\.\d+\.\d+\.\d+)/(\d+))");
		std::smatch match;
		if (std::regex_match(rangeStr, match, cidrRegex)) {
			uint32_t ip = ipToUint(match[1]);
			int maskBits = std::stoi(match[2]);
			uint32_t mask = (0xFFFFFFFF << (32 - maskBits)) & 0xFFFFFFFF;
			uint32_t start = ip & mask;
			uint32_t end = start | ~mask;
			return { start, end };
		}
	}
	else {
		// Одиночный IP
		uint32_t ip = ipToUint(rangeStr);
		return { ip, ip };
	}

	throw std::invalid_argument("Invalid range format: " + rangeStr);
}

std::vector<IPRange> parseIPList(const std::vector<std::string>& ipList) {
	std::vector<IPRange> ranges;
	for (const auto& ip : ipList) {
		try {
			ranges.push_back(parseRange(ip));
		}
		catch (const std::invalid_argument& e) {
			std::cerr << e.what() << "\n";
		}
	}
	return ranges;
}

bool getExternalIP(std::string& externalIP) {
	CURL* curl = curl_easy_init();
	if (!curl) {
		std::cerr << "Failed to initialize CURL\n";
		return false;
	}

	std::string apiUrl = "https://api.ipify.org";
	std::string response;
	curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](void* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
		auto* response = static_cast<std::string*>(userdata);
		response->append(static_cast<char*>(ptr), size * nmemb);
		return size * nmemb;
		});
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::cerr << "CURL error: " << curl_easy_strerror(res) << "\n";
		curl_easy_cleanup(curl);
		return false;
	}

	curl_easy_cleanup(curl);
	externalIP = response;
	return true;
}

bool checkIP(const std::vector<std::string>& ipList) {
	std::string externalIP;
	if (!getExternalIP(externalIP)) {
		std::cerr << "Failed to retrieve external IP\n";
		return false;
	}

	uint32_t externalIPInt = ipToUint(externalIP);
	auto ranges = parseIPList(ipList);

	for (const auto& range : ranges) {
		if (range.contains(externalIPInt)) {
			return true; // Найдено соответствие
		}
	}
	return false; // Нет соответствий
}

std::string exec(const char* cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

//IP CHECK

string sendToDB(string PCName, string column, string value, string mode = "plus")
{
	string cmd = "main.py " + PCName + " " + value + " " + column + " " + mode;
	if (std::filesystem::exists("credentials.json"))
	{
		return exec(cmd.c_str());
	}
	else
	{
		return "";
	}
}

void removeTrailingNewline(std::string& str) {
	// Удаление \r\n (Windows) или \n (Unix)
	if (!str.empty() && str.back() == '\n') {
		str.pop_back();
		if (!str.empty() && str.back() == '\r') {
			str.pop_back();
		}
	}
}

std::string getGPUNameFromRegistry() {
	HKEY hKey;
	const char* subKey = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\WinSAT";

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
		std::cerr << "Error: Unable to open registry!" << std::endl;
		return "";
	}

	char value[256];
	DWORD valueSize = sizeof(value);
	if (RegQueryValueExA(hKey, "PrimaryAdapterString", nullptr, nullptr, (LPBYTE)value, &valueSize) == ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return std::string(value);
	}

	RegCloseKey(hKey);
	return "";
}

bool checkBanInTable(string pc_name)
{
	string cmd = "main.py " + pc_name;
	return filterDigits(exec(cmd.c_str())) == "1";
}

string getServerIp(string pc_name) 
{
	string cmd = "scripts\\get_server_from_sheet.py " + pc_name;
	string server_name = exec(cmd.c_str());
	cout << "Server name is:" << server_name << '.' << endl;
	server_name.erase(std::remove(server_name.begin(), server_name.end(), ' '), server_name.end());
	removeTrailingNewline(server_name);
	cout << "Server ip is:" << servers[server_name] << endl;
	return servers[server_name];
}

void updateStorageJson(const std::string& SERVER_IP) {
	std::string filePath = R"(C:\Games\GTA5RP\RageMP\clientdata\cef\clientdata\storage.json)";

	

	std::string content =
		"{\"tab\":\"favorites\",\"favorites\":[{\"ip\":\"" + SERVER_IP +
		"\",\"name\":\"V3 SERVER\",\"port\":22005}],\"history\":["
		"{\"ip\":\"vinewood.gta5rp.com\",\"name\":\"vinewood.gta5rp.com:22005\",\"port\":\"22005\"},"
		"{\"ip\":\"vinewood.gta5rp.com\",\"name\":\"GTA5RP.COM | VineWood | gta5rp.com/discord\",\"port\":22005}]}";

	if (SERVER_IP.empty())
	{
		content =
			"{\"tab\":\"favorites\",\"favorites\":[{\"ip\":\"" + SERVER_IP +
			"\",\"name\":\"ERROR DURING GETTING SERVER IP!!!!\",\"port\":22005}],\"history\":["
			"{\"ip\":\"vinewood.gta5rp.com\",\"name\":\"vinewood.gta5rp.com:22005\",\"port\":\"22005\"},"
			"{\"ip\":\"vinewood.gta5rp.com\",\"name\":\"GTA5RP.COM | VineWood | gta5rp.com/discord\",\"port\":22005}]}";
	}
	std::ofstream file(filePath, std::ios::trunc);  // std::ios::trunc — очищает файл перед записью
	if (!file.is_open()) {
		MessageBoxA(nullptr, "Не удалось открыть файл для записи.", "Ошибка", MB_ICONERROR | MB_OK);
		return;
	}

	file << content;
	file.close();
}

bool checkSpin()
{
	ScreenScaner scan1 = ScreenScaner();
	string s = scan1.makeText(false);

	// Конвертация из UTF-8 (или CP1251) → wstring
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring ws = conv.from_bytes(s);

	// Регулярка по-русски
	std::wregex spin(L"Доступно колесо удачи");

	// Поиск в широких символах
	bool found = std::regex_search(ws, spin);
	return found;
}

bool checkQueue()
{
	ScreenScaner scan1 = ScreenScaner();
	//makeText(bool eng, int x, int y, int width, int height, int R, int G, int B, int Rmax, int Gmax, int Bmax)
	string s = scan1.makeText(false, 880, 515, 346,177, 60, 103, 3, 74, 232, 35);

	// Конвертация из UTF-8 (или CP1251) → wstring
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring ws = conv.from_bytes(s);

	// Регулярка по-русски
	std::wregex spin(L"подождите");

	// Поиск в широких символах
	bool found = std::regex_search(ws, spin);
	return found;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ENTERPRISE CONSOLE UI SYSTEM v2.0 - Windows Console API
// Профессиональный вывод для enterprise-приложений 2025
// ═══════════════════════════════════════════════════════════════════════════════

// Цветовая схема (Windows Console API)
namespace ConsoleColors {
	const int BLACK = 0;
	const int DARK_BLUE = 1;
	const int DARK_GREEN = 2;
	const int DARK_CYAN = 3;
	const int DARK_RED = 4;
	const int DARK_MAGENTA = 5;
	const int DARK_YELLOW = 6;
	const int GRAY = 7;
	const int DARK_GRAY = 8;
	const int BLUE = 9;
	const int GREEN = 10;
	const int CYAN = 11;
	const int RED = 12;
	const int MAGENTA = 13;
	const int YELLOW = 14;
	const int WHITE = 15;
	
	// Комбинации для фона
	const int BG_BLUE = 16;
	const int BG_GREEN = 32;
	const int BG_CYAN = 48;
	const int BG_RED = 64;
}

// Глобальный handle консоли для производительности
static HANDLE g_hConsole = NULL;

// Инициализация консоли с поддержкой расширенных цветов
void initConsole() {
	g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	// Установка кодовой страницы для корректного отображения
	SetConsoleOutputCP(866); // DOS Cyrillic
	
	// Установка размера буфера консоли
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(g_hConsole, &csbi);
	
	// Очистка консоли с черным фоном
	COORD topLeft = {0, 0};
	DWORD written;
	DWORD length = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(g_hConsole, ' ', length, topLeft, &written);
	FillConsoleOutputAttribute(g_hConsole, ConsoleColors::GRAY, length, topLeft, &written);
	SetConsoleCursorPosition(g_hConsole, topLeft);
}

// Установка цвета текста
inline void setColor(int color) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(g_hConsole, color);
}

// Сброс цвета на стандартный
inline void resetColor() {
	setColor(ConsoleColors::GRAY);
}

// Получение текущего времени в формате строки
string getCurrentTimeStr() {
	time_t now = time(nullptr);
	tm* ltm = localtime(&now);
	char buffer[12];
	strftime(buffer, sizeof(buffer), "%H:%M:%S", ltm);
	return string(buffer);
}

// ═══════════════════════════════════════════════════════════════════════════════
// БАННЕР И ЗАГОЛОВКИ
// ═══════════════════════════════════════════════════════════════════════════════

void printBanner(const string& version) {
	if (!g_hConsole) initConsole();
	
	cout << "\n";
	
	// Верхняя рамка
	setColor(ConsoleColors::DARK_CYAN);
	cout << "  ";
	for (int i = 0; i < 68; i++) cout << "=";
	cout << "\n";
	
	// Логотип VIRTAPP
	setColor(ConsoleColors::CYAN);
	cout << "  ||";
	setColor(ConsoleColors::YELLOW);
	cout << "  __      __ _        _      _                                  ";
	setColor(ConsoleColors::CYAN);
	cout << "||\n";
	
	cout << "  ||";
	setColor(ConsoleColors::YELLOW);
	cout << "  \\ \\    / /(_) _ __ | |_   / \\   _ __   _ __                   ";
	setColor(ConsoleColors::CYAN);
	cout << "||\n";
	
	cout << "  ||";
	setColor(ConsoleColors::YELLOW);
	cout << "   \\ \\  / / | || '__|| __| / _ \\ | '_ \\ | '_ \\                  ";
	setColor(ConsoleColors::CYAN);
	cout << "||\n";
	
	cout << "  ||";
	setColor(ConsoleColors::YELLOW);
	cout << "    \\ \\/ /  | || |   | |_ / ___ \\| |_) || |_) |                 ";
	setColor(ConsoleColors::CYAN);
	cout << "||\n";
	
	cout << "  ||";
	setColor(ConsoleColors::YELLOW);
	cout << "     \\__/   |_||_|    \\__/_/   \\_\\ .__/ | .__/                  ";
	setColor(ConsoleColors::CYAN);
	cout << "||\n";
	
	cout << "  ||";
	setColor(ConsoleColors::YELLOW);
	cout << "                                 |_|    |_|                     ";
	setColor(ConsoleColors::CYAN);
	cout << "||\n";
	
	// Разделитель
	setColor(ConsoleColors::DARK_CYAN);
	cout << "  ||";
	for (int i = 0; i < 64; i++) cout << "-";
	cout << "||\n";
	
	// Информация о приложении
	setColor(ConsoleColors::CYAN);
	cout << "  ||";
	setColor(ConsoleColors::GREEN);
	cout << "              GTA5RP Automation Bot - Enterprise Edition        ";
	setColor(ConsoleColors::CYAN);
	cout << "||\n";
	
	cout << "  ||";
	setColor(ConsoleColors::WHITE);
	cout << "                        Version: ";
	setColor(ConsoleColors::YELLOW);
	cout << version;
	setColor(ConsoleColors::WHITE);
	cout << "                             ";
	setColor(ConsoleColors::CYAN);
	cout << "||\n";
	
	cout << "  ||";
	setColor(ConsoleColors::DARK_GRAY);
	cout << "                   (c) 2024-2025 RootOne1337                    ";
	setColor(ConsoleColors::CYAN);
	cout << "||\n";
	
	cout << "  ||";
	setColor(ConsoleColors::DARK_GRAY);
	cout << "                    Build: " << __DATE__ << " " << __TIME__ << "                   ";
	setColor(ConsoleColors::CYAN);
	cout << "||\n";
	
	// Нижняя рамка
	setColor(ConsoleColors::DARK_CYAN);
	cout << "  ";
	for (int i = 0; i < 68; i++) cout << "=";
	cout << "\n\n";
	
	resetColor();
}

// ═══════════════════════════════════════════════════════════════════════════════
// СЕКЦИИ И БЛОКИ
// ═══════════════════════════════════════════════════════════════════════════════

void printSection(const string& title) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	cout << "\n";
	setColor(ConsoleColors::CYAN);
	cout << "  +";
	setColor(ConsoleColors::DARK_CYAN);
	for (int i = 0; i < 66; i++) cout << "-";
	setColor(ConsoleColors::CYAN);
	cout << "+\n";
	
	cout << "  |";
	setColor(ConsoleColors::YELLOW);
	cout << " >> " << title;
	
	// Заполнение пробелами до конца строки
	int padding = 61 - (int)title.length();
	for (int i = 0; i < padding; i++) cout << " ";
	
	setColor(ConsoleColors::CYAN);
	cout << "|\n";
	
	cout << "  +";
	setColor(ConsoleColors::DARK_CYAN);
	for (int i = 0; i < 66; i++) cout << "-";
	setColor(ConsoleColors::CYAN);
	cout << "+\n";
	
	resetColor();
}

void printSectionEnd() {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	setColor(ConsoleColors::CYAN);
	cout << "  +";
	setColor(ConsoleColors::DARK_CYAN);
	for (int i = 0; i < 66; i++) cout << "-";
	setColor(ConsoleColors::CYAN);
	cout << "+\n";
	resetColor();
}

void printSubSection(const string& title) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	setColor(ConsoleColors::DARK_GRAY);
	cout << "  |--- ";
	setColor(ConsoleColors::WHITE);
	cout << title << "\n";
	resetColor();
}

// ═══════════════════════════════════════════════════════════════════════════════
// СТАТУСЫ И СООБЩЕНИЯ
// ═══════════════════════════════════════════════════════════════════════════════

void printStatus(const string& key, const string& value, int color = ConsoleColors::GREEN) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	setColor(ConsoleColors::DARK_GRAY);
	cout << "  |  ";
	setColor(ConsoleColors::WHITE);
	cout << key;
	
	// Выравнивание по точкам
	int dots = 25 - (int)key.length();
	setColor(ConsoleColors::DARK_GRAY);
	for (int i = 0; i < dots; i++) cout << ".";
	cout << ": ";
	
	setColor(color);
	cout << value << "\n";
	resetColor();
}

void printStatusBool(const string& key, bool value, const string& trueText = "ON", const string& falseText = "OFF") {
	printStatus(key, value ? trueText : falseText, value ? ConsoleColors::GREEN : ConsoleColors::RED);
}

void printOK(const string& message) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	setColor(ConsoleColors::DARK_GRAY);
	cout << "  [";
	setColor(ConsoleColors::GREEN);
	cout << " OK ";
	setColor(ConsoleColors::DARK_GRAY);
	cout << "] ";
	setColor(ConsoleColors::WHITE);
	cout << message << "\n";
	resetColor();
}

void printWarn(const string& message) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	setColor(ConsoleColors::DARK_GRAY);
	cout << "  [";
	setColor(ConsoleColors::YELLOW);
	cout << "WARN";
	setColor(ConsoleColors::DARK_GRAY);
	cout << "] ";
	setColor(ConsoleColors::YELLOW);
	cout << message << "\n";
	resetColor();
}

void printError(const string& message) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	setColor(ConsoleColors::DARK_GRAY);
	cout << "  [";
	setColor(ConsoleColors::RED);
	cout << "FAIL";
	setColor(ConsoleColors::DARK_GRAY);
	cout << "] ";
	setColor(ConsoleColors::RED);
	cout << message << "\n";
	resetColor();
}

void printInfo(const string& message) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	setColor(ConsoleColors::DARK_GRAY);
	cout << "  [";
	setColor(ConsoleColors::CYAN);
	cout << "INFO";
	setColor(ConsoleColors::DARK_GRAY);
	cout << "] ";
	setColor(ConsoleColors::WHITE);
	cout << message << "\n";
	resetColor();
}

void printDebug(const string& message) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	setColor(ConsoleColors::DARK_GRAY);
	cout << "  [";
	setColor(ConsoleColors::MAGENTA);
	cout << "DBG ";
	setColor(ConsoleColors::DARK_GRAY);
	cout << "] " << message << "\n";
	resetColor();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ПРОГРЕСС-БАР
// ═══════════════════════════════════════════════════════════════════════════════

void printProgress(const string& label, int current, int total, int barWidth = 40) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	float progress = (float)current / total;
	int filled = (int)(progress * barWidth);
	
	setColor(ConsoleColors::WHITE);
	cout << "  " << label << " [";
	
	setColor(ConsoleColors::GREEN);
	for (int i = 0; i < filled; i++) cout << "#";
	
	setColor(ConsoleColors::DARK_GRAY);
	for (int i = filled; i < barWidth; i++) cout << "-";
	
	setColor(ConsoleColors::WHITE);
	cout << "] ";
	setColor(ConsoleColors::YELLOW);
	cout << (int)(progress * 100) << "%";
	setColor(ConsoleColors::DARK_GRAY);
	cout << " (" << current << "/" << total << ")\r";
	cout.flush();
	resetColor();
}

void printProgressDone(const string& label) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	setColor(ConsoleColors::WHITE);
	cout << "  " << label << " [";
	setColor(ConsoleColors::GREEN);
	for (int i = 0; i < 40; i++) cout << "#";
	setColor(ConsoleColors::WHITE);
	cout << "] ";
	setColor(ConsoleColors::GREEN);
	cout << "100% DONE                    \n";
	resetColor();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ТАБЛИЦЫ
// ═══════════════════════════════════════════════════════════════════════════════

void printTableHeader(const vector<string>& headers, const vector<int>& widths) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	// Верхняя линия
	setColor(ConsoleColors::CYAN);
	cout << "  +";
	for (size_t i = 0; i < headers.size(); i++) {
		for (int j = 0; j < widths[i] + 2; j++) cout << "-";
		cout << "+";
	}
	cout << "\n";
	
	// Заголовки
	cout << "  |";
	for (size_t i = 0; i < headers.size(); i++) {
		setColor(ConsoleColors::YELLOW);
		cout << " " << headers[i];
		int pad = widths[i] - (int)headers[i].length();
		for (int j = 0; j < pad; j++) cout << " ";
		cout << " ";
		setColor(ConsoleColors::CYAN);
		cout << "|";
	}
	cout << "\n";
	
	// Разделитель
	cout << "  +";
	for (size_t i = 0; i < headers.size(); i++) {
		for (int j = 0; j < widths[i] + 2; j++) cout << "=";
		cout << "+";
	}
	cout << "\n";
	resetColor();
}

void printTableRow(const vector<string>& cells, const vector<int>& widths, const vector<int>& colors) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	setColor(ConsoleColors::CYAN);
	cout << "  |";
	for (size_t i = 0; i < cells.size(); i++) {
		setColor(colors.size() > i ? colors[i] : ConsoleColors::WHITE);
		cout << " " << cells[i];
		int pad = widths[i] - (int)cells[i].length();
		for (int j = 0; j < pad; j++) cout << " ";
		cout << " ";
		setColor(ConsoleColors::CYAN);
		cout << "|";
	}
	cout << "\n";
	resetColor();
}

void printTableEnd(const vector<int>& widths) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	setColor(ConsoleColors::CYAN);
	cout << "  +";
	for (size_t i = 0; i < widths.size(); i++) {
		for (int j = 0; j < widths[i] + 2; j++) cout << "-";
		cout << "+";
	}
	cout << "\n";
	resetColor();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ЛОГИРОВАНИЕ С ЦВЕТАМИ И КАТЕГОРИЯМИ
// ═══════════════════════════════════════════════════════════════════════════════

enum LogLevel {
	LOG_DEBUG = 0,
	LOG_INFO = 1,
	LOG_SUCCESS = 2,
	LOG_WARNING = 3,
	LOG_ERROR = 4,
	LOG_CRITICAL = 5
};

void logPrint(const string& message, LogLevel level = LOG_INFO) {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	// Временная метка
	string timeStr = getCurrentTimeStr();
	
	setColor(ConsoleColors::DARK_GRAY);
	cout << "  [" << timeStr << "] ";
	
	// Уровень лога
	switch (level) {
		case LOG_DEBUG:
			setColor(ConsoleColors::MAGENTA);
			cout << "[DEBUG] ";
			setColor(ConsoleColors::DARK_GRAY);
			break;
		case LOG_INFO:
			setColor(ConsoleColors::CYAN);
			cout << "[INFO]  ";
			setColor(ConsoleColors::WHITE);
			break;
		case LOG_SUCCESS:
			setColor(ConsoleColors::GREEN);
			cout << "[OK]    ";
			setColor(ConsoleColors::WHITE);
			break;
		case LOG_WARNING:
			setColor(ConsoleColors::YELLOW);
			cout << "[WARN]  ";
			setColor(ConsoleColors::YELLOW);
			break;
		case LOG_ERROR:
			setColor(ConsoleColors::RED);
			cout << "[ERROR] ";
			setColor(ConsoleColors::RED);
			break;
		case LOG_CRITICAL:
			setColor(ConsoleColors::RED | ConsoleColors::BG_RED);
			cout << "[CRIT]  ";
			setColor(ConsoleColors::RED);
			break;
	}
	
	cout << message << "\n";
	resetColor();
}

// ═══════════════════════════════════════════════════════════════════════════════
// СПИННЕР ДЛЯ ОЖИДАНИЯ
// ═══════════════════════════════════════════════════════════════════════════════

class Spinner {
private:
	const char* frames[4] = {"|", "/", "-", "\\"};
	int currentFrame = 0;
	string message;
	bool running = false;
	
public:
	void start(const string& msg) {
		message = msg;
		running = true;
		currentFrame = 0;
	}
	
	void update() {
		if (!running) return;
		if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		
		setColor(ConsoleColors::CYAN);
		cout << "\r  [";
		setColor(ConsoleColors::YELLOW);
		cout << frames[currentFrame];
		setColor(ConsoleColors::CYAN);
		cout << "] ";
		setColor(ConsoleColors::WHITE);
		cout << message << "   ";
		cout.flush();
		
		currentFrame = (currentFrame + 1) % 4;
		resetColor();
	}
	
	void stop(bool success = true) {
		running = false;
		if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		
		cout << "\r  [";
		if (success) {
			setColor(ConsoleColors::GREEN);
			cout << "OK";
		} else {
			setColor(ConsoleColors::RED);
			cout << "XX";
		}
		setColor(ConsoleColors::DARK_GRAY);
		cout << "] ";
		setColor(ConsoleColors::WHITE);
		cout << message << "                    \n";
		resetColor();
	}
};

// Глобальный спиннер
static Spinner g_spinner;

// ═══════════════════════════════════════════════════════════════════════════════
// СТАРТОВАЯ ИНФОРМАЦИЯ О СИСТЕМЕ
// ═══════════════════════════════════════════════════════════════════════════════

void printSystemInfo() {
	if (!g_hConsole) g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	// Получение информации о системе
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	
	printSection("SYSTEM INFORMATION");
	printStatus("OS", "Windows", ConsoleColors::CYAN);
	printStatus("Architecture", sysInfo.wProcessorArchitecture == 9 ? "x64" : "x86", ConsoleColors::CYAN);
	printStatus("Processors", to_string(sysInfo.dwNumberOfProcessors) + " cores", ConsoleColors::GREEN);
	printStatus("Total RAM", to_string(memInfo.ullTotalPhys / (1024 * 1024)) + " MB", ConsoleColors::GREEN);
	printStatus("Available RAM", to_string(memInfo.ullAvailPhys / (1024 * 1024)) + " MB", ConsoleColors::YELLOW);
	printSectionEnd();
}

BOOL WINAPI ConsoleHandler(DWORD dwType)
{
	if (dwType == CTRL_CLOSE_EVENT || dwType == CTRL_C_EVENT)
	{
		printf("\n[VirtApp] Exiting... Performing emergency backup...\n");
		system("python scripts/epic_auth.py --backup-only");
		Sleep(2000);
	}
	return FALSE;
}

int main() {
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);
	
	// ═══════════════════════════════════════════════════════════════════════════
	// САМЫЙ РАННИЙ ВЫВОД - до любой инициализации
	// Если это не появится - проблема в DLL или загрузке
	// ═══════════════════════════════════════════════════════════════════════════
	
	// Убеждаемся что консоль работает
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE || hStdOut == NULL) {
		// Попробуем выделить консоль
		AllocConsole();
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	
	printf("=== VirtApp STARTING ===\n");
	printf("Version: v3.2 (Build: %s %s)\n", __DATE__, __TIME__);
	printf("Initializing...\n");
	fflush(stdout);
	
	try {
		// ═══════════════════════════════════════════════════════════════════════════
		// Инициализация Dashboard UI v3.0
		// ═══════════════════════════════════════════════════════════════════════════
		printf("[1/5] Console init...\n"); fflush(stdout);
		initConsole();
		
		printf("[2/5] Dashboard init...\n"); fflush(stdout);
		initDashboard();
		
		string version = "v3.5.1";  // Dashboard UI. Enterprise-level console interface.
		string status = "Baldeet";
		
		printf("[3/5] CURL init...\n"); fflush(stdout);
		curl_global_init(CURL_GLOBAL_DEFAULT);
		
		printf("[4/5] Single instance check...\n"); fflush(stdout);
		void* program_instance = try_open_single_program("407637B6-98D3-4EFC-A838-40BBB5204CF1");
		if (!program_instance) {
			printf("ERROR: Another instance is running!\n");
			printf("Press any key to exit...\n");
			_getch();
			return 0;
		}
		
		printf("[5/5] Ready!\n"); fflush(stdout);


	std::vector<std::string> ipList = {
		"212.220.204.72",
		"217.73.89.128",
		"79.142.197.0 - 79.142.197.255",
		"217.73.88.0 - 217.73.91.255",
		"185.70.0.0 - 185.70.255.255"
	};

	InterceptionContext context;
	InterceptionDevice device;
	InterceptionDevice mouseDevice;
	InterceptionKeyStroke kstroke;
	InterceptionMouseStroke mstroke;
	ScreenScaner scan = ScreenScaner();



	raise_process_priority();

	// ═══════════════════════════════════════════════════════════════════════════
	// TIME SYNC - Must complete BEFORE any network operations (SSL requires correct time)
	// ═══════════════════════════════════════════════════════════════════════════
	printSection("TIME SYNCHRONIZATION");
	printInfo("Syncing system time (required for SSL)...");
	{
		// Используем быстрый скрипт с уменьшенным таймаутом
		string set_time_command = "scripts\\set_local_time.py";
		string result = exec(set_time_command.c_str());
		if (result.find("successfully") != string::npos || result.find("SUCCESS") != string::npos) {
			printOK("Time synchronized successfully");
		} else {
			printWarn("Time sync may have issues, continuing...");
		}
	}
	printSectionEnd();
	
	context = interception_create_context();

	// FIX: Initialize devices explicitly since we removed the input filter thread
	// We need to find the mouse and keyboard devices to send events to them.
	for (int i = 1; i <= INTERCEPTION_MAX_DEVICE; ++i) {
		char hardware_id[512];
		if (interception_get_hardware_id(context, i, hardware_id, sizeof(hardware_id)) > 0) {
			if (interception_is_keyboard(i)) {
				device = i;
			}
			else if (interception_is_mouse(i)) {
				mouseDevice = i;
			}
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// MOUSE PASS-THROUGH THREAD (REMOVED)
	// ═══════════════════════════════════════════════════════════════════════════
	// REMOVED: This thread was causing mouse blocking issues (dead mouse) because
	// it set a filter but sometimes failed to forward events correctly.
	// Since we only need INJECTION (not interception of user input), we don't need
	// a filter at all. The main thread initializes 'device' and 'mouseDevice'
	// by iterating hardware IDs, which works without a filter.
	// This ensures the user's physical mouse/keyboard work natively without lag.
	/*
	thread input_passthrough([&]() {
		InterceptionContext ctx = interception_create_context();
		interception_set_filter(ctx, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);
		interception_set_filter(ctx, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_ALL);
		
		InterceptionDevice dev;
		InterceptionStroke stroke;
		
		while (interception_receive(ctx, dev = interception_wait(ctx), (InterceptionStroke*)&stroke, 1) > 0) {
			// Просто пересылаем всё, что пришло от физических устройств
			interception_send(ctx, dev, (InterceptionStroke*)&stroke, 1);
		}
		interception_destroy_context(ctx);
	});
	input_passthrough.detach(); // Отпускаем поток в свободное плавание
	*/

	Sleep(1000);

	tm targetTimeMin = {};
	targetTimeMin.tm_hour = 7;    // Час
	targetTimeMin.tm_min = 0;      // Минута

	std::tm targetTimeMax = {};
	targetTimeMax.tm_hour = 7;    // Час
	targetTimeMax.tm_min = 5;      // Минута

	time_t currentTime;
	tm* currentTm;


	Sleep(2000);
	thread timecheck([&]()
		{
			while (true)
			{
				currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				currentTm = std::localtime(&currentTime);
				Sleep(350);
			}
		});
	timecheck.detach();

	// ═══════════════════════════════════════════════════════════════════════════
	// Dashboard UI - Главный баннер
	// ═══════════════════════════════════════════════════════════════════════════
	string extIP;
	getExternalIP(extIP);
	
	// Выводим полный Dashboard
	dashPrintHeader(version);
	dashPrintSystemPanel();
	
	logprint("Current IP:\t" + extIP, currentTm);

	Sleep(1000);
	logprint("Started", currentTm);

	string password = "Naguibs1337228";

	logprint("Current version:\t" + version, currentTm);
	string token(getenv("TOKEN"));
	
	// Dashboard Log Entry для Telegram
	dashPrintLogHeader();
	dashPrintLogEntry("INFO", "Telegram bot connecting...", "WAIT");
	
	logprint("Token:\t " + token, currentTm);


	string gpuname = getGPUNameFromRegistry();
	string gta5Settings = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><Settings><version value=\"27\" /><configSource>SMC_AUTO</configSource><graphics><Tessellation value=\"0\" /><LodScale value=\"0.000000\" /><PedLodBias value=\"0.000000\" /><VehicleLodBias value=\"0.000000\" /><ShadowQuality value=\"1\" /><ReflectionQuality value=\"0\" /><ReflectionMSAA value=\"0\" /><SSAO value=\"0\" /><AnisotropicFiltering value=\"0\" /><MSAA value=\"0\" /><MSAAFragments value=\"0\" /><MSAAQuality value=\"0\" /><SamplingMode value=\"1\" /><TextureQuality value=\"0\" /><ParticleQuality value=\"0\" /><WaterQuality value=\"0\" /><GrassQuality value=\"0\" /><ShaderQuality value=\"0\" /><Shadow_SoftShadows value=\"0\" /><UltraShadows_Enabled value=\"false\" /><Shadow_ParticleShadows value=\"true\" /><Shadow_Distance value=\"1.000000\" /><Shadow_LongShadows value=\"false\" /><Shadow_SplitZStart value=\"0.930000\" /><Shadow_SplitZEnd value=\"0.890000\" /><Shadow_aircraftExpWeight value=\"0.990000\" /><Shadow_DisableScreenSizeCheck value=\"false\" /><Reflection_MipBlur value=\"true\" /><FXAA_Enabled value=\"false\" /><TXAA_Enabled value=\"false\" /><Lighting_FogVolumes value=\"true\" /><Shader_SSA value=\"false\" /><DX_Version value=\"2\" /><CityDensity value=\"0.000000\" /><PedVarietyMultiplier value=\"0.000000\" /><VehicleVarietyMultiplier value=\"0.000000\" /><PostFX value=\"0\" /><DoF value=\"false\" /><HdStreamingInFlight value=\"false\" /><MaxLodScale value=\"0.000000\" /><MotionBlurStrength value=\"0.000000\" /></graphics><system><numBytesPerReplayBlock value=\"9000000\" /><numReplayBlocks value=\"30\" /><maxSizeOfStreamingReplay value=\"1024\" /><maxFileStoreSize value=\"65536\" /></system><audio><Audio3d value=\"false\" /></audio><video><AdapterIndex value=\"0\" /><OutputIndex value=\"0\" /><ScreenWidth value=\"1280\" /><ScreenHeight value=\"720\" /><RefreshRate value=\"59\" /><Windowed value=\"2\" /><VSync value=\"2\" /><Stereo value=\"0\" /><Convergence value=\"0.100000\" /><Separation value=\"1.000000\" /><PauseOnFocusLoss value=\"1\" /><AspectRatio value=\"0\" /></video><VideoCardDescription>" + getGPUNameFromRegistry() + (string)"</VideoCardDescription></Settings>";
	const string photoFilePath = "./Screenshot.png";
	const string photoMimeType = "image/png";
	ifstream startup_config;
	startup_config.open("config.txt");
	string empty;
	int ActiveCharacter;
	string mail;
	string mpass;
	string imapip;
	string socmail;
	string socpass;
	string login;
	bool ha = true;
	bool present_activity = true;
	bool direction = true;
	bool afk = false;
	bool ruletka = true;
	bool ruletka_user_disabled = false; // Флаг: пользователь вручную выключил рулетку
	int previous_lvl = 0; // Предыдущий уровень для отслеживания достижения 3 лвл
	int current_lvl_global = 0; // Глобальная переменная для хранения текущего уровня
	int max_lvl_int = 5; // Максимальный уровень из таблицы (по умолчанию 5)
	bool relogin = true;
	bool forceRelogin = false;
	bool cmd = false;
	bool awaitingConfigContent = false;
	bool awaitingCredentialsContent = false;
	bool rename = false;
	bool validation = true;
	bool updating = false;
	int imapport = 993;
	bool altafk = false;
	bool donotrestart = false;
	int timeout = 60;
	bool afkonly = false;
	bool banned = false;
	string PCName = "TEST_4_S7_32";
	
	// Epic Games credentials (для автоматической авторизации)
	string epicLogin = "";
	string epicPassword = "";
	bool epicAuthEnabled = false;  // Флаг: запускать ли скрипт авторизации Epic

	map<string, string> config;
	while (!startup_config.eof())
	{
		string key;
		string value;
		getline(startup_config, key, '=');
		getline(startup_config, value);
		if (value[value.length() - 1] == ';')
		{
			value = value.substr(0, value.length() - 1);
		}
		config.insert({ key, value });
	}


	//CONFIG - Загрузка конфигурации
	string configLogin = "";
	string configPCName = "Unknown";
	
	if (!config["Active Character"].empty())
	{
		try
		{
			ActiveCharacter = stoi(config["Active Character"]);
			logprint("Active character #" + to_string(ActiveCharacter), currentTm);
		}
		catch (const std::exception&)
		{
			ActiveCharacter = 404;
			logprint("Active character #" + to_string(ActiveCharacter), currentTm);
		}
	}
	if (!config["Email"].empty())
	{
		mail = config["Email"];
		logprint("Mail for Rockstar: " + mail, currentTm);
	}
	if (!config["Password"].empty())
	{
		mpass = config["Password"];
		logprint("Mail password for Rockstar: " + mpass, currentTm);
	}
	if (!config["IMAP"].empty())
	{
		imapip = config["IMAP"];
		logprint("IMAP IP for Rockstar: " + imapip, currentTm);
	}
	if (!config["SocialLogin"].empty())
	{
		socmail = config["SocialLogin"];
		logprint("Rockstar login: " + socmail, currentTm);
	}
	if (!config["SocialPassword"].empty())
	{
		socpass = config["SocialPassword"];
		logprint("Rockstar password: " + mpass, currentTm);
	}
	if (!config["AFK"].empty())
	{
		if (config["AFK"] == "ON" || config["AFK"] == "on")
		{
			afk = true;
			ruletka = false;
			logprint("AFK: ON, Ruletka: OFF", currentTm);
		}
		else
		{
			afk = false;
		}
	}
	if (!config["PCNAME"].empty())
	{
		PCName = config["PCNAME"];
		configPCName = PCName;
		logprint("PCNAME: " + PCName, currentTm);
	}
	if (!config["Login"].empty())
	{
		login = config["Login"];
		configLogin = login;
		logprint("Login: " + login, currentTm);
	}
	if (!config["afkonly"].empty())
	{
		afkonly = true;
	}
	
	// Epic Games credentials - чтение из конфига
	if (!config["EpicLogin"].empty())
	{
		epicLogin = config["EpicLogin"];
		logprint("Epic Login: " + epicLogin.substr(0, 3) + "***", currentTm);
	}
	if (!config["EpicPassword"].empty())
	{
		epicPassword = config["EpicPassword"];
		logprint("Epic Password: ***", currentTm);
	}
	
	// Проверяем заполнены ли Epic credentials
	if (!epicLogin.empty() && !epicPassword.empty())
	{
		epicAuthEnabled = true;
		logprint("Epic Auth: ENABLED", currentTm);
	}
	else
	{
		epicAuthEnabled = false;
		logprint("Epic Auth: DISABLED (credentials not set)", currentTm);
	}
	
	// Автогенерация полей Epic в config.txt если их нет
	if (config.find("EpicLogin") == config.end() || config.find("EpicPassword") == config.end())
	{
		logprint("Epic fields missing in config, adding...", currentTm);
		std::ifstream configReadFile("config.txt");
		std::string configContent;
		if (configReadFile.is_open())
		{
			std::stringstream buffer;
			buffer << configReadFile.rdbuf();
			configContent = buffer.str();
			configReadFile.close();
			
			// Добавляем поля Epic если их нет
			bool needsEpicLogin = (configContent.find("EpicLogin=") == std::string::npos);
			bool needsEpicPassword = (configContent.find("EpicPassword=") == std::string::npos);
			
			if (needsEpicLogin || needsEpicPassword)
			{
				std::ofstream configWriteFile("config.txt", std::ios::app);
				if (configWriteFile.is_open())
				{
					configWriteFile << "\n# Epic Games (auto-auth)\n";
					if (needsEpicLogin)
						configWriteFile << "EpicLogin=;\n";
					if (needsEpicPassword)
						configWriteFile << "EpicPassword=;\n";
					configWriteFile.close();
					logprint("Epic fields added to config.txt", currentTm);
				}
			}
		}
	}
	
	// Dashboard - Панель сессии с данными из конфига
	dashPrintSessionPanel(configLogin.empty() ? "Unknown" : configLogin, configPCName, extIP);
	
	// Dashboard - Лог записи инициализации
	dashPrintLogEntry("OK", "Configuration loaded", "OK");
	dashPrintLogEntry("OK", "Interception driver ready", "OK");
	dashPrintLogEntry("INFO", "Waiting for game process...", "WAIT");
	dashPrintLogFooter();
	dashPrintHotkeys();
	dashPrintFooter();


	thread update([&updating]()
		{
			while (true)
			{
				Sleep(1000);
				if (updating)
				{
					Sleep(5000);
					string path = ("update.bat");
					char buf[255];
					strcpy(buf, path.c_str());
					system(buf);
				}
			}
		});
	update.detach();
	logprint("Active character:\t" + '#' + ActiveCharacter, currentTm);
	Bot bot(token);

	if (validation)
	{
		try
		{
			bot.getApi().sendMessage(517005065, "Verified", false, 0, nullptr, "", true);
		}
		catch (const std::exception&)
		{
			return 0;
		}
	}




	// ═══════════════════════════════════════════════════════════════════════════
	// TELEGRAM BOT - Улучшенная клавиатура с эмодзи (2025)
	// ═══════════════════════════════════════════════════════════════════════════
	ReplyKeyboardMarkup::Ptr keyboardOneCol(new ReplyKeyboardMarkup);
	createOneColumnKeyboard({ "Photo", "AFK", "Ruletka" }, keyboardOneCol);

	ReplyKeyboardMarkup::Ptr keyboardWithLayout(new ReplyKeyboardMarkup);
	createKeyboard({
		{"/photo", "/status"},
		{"/afk", "/ruletka"},
		{"/vip", "/lvl"},
		{"/restart", "/help"},
		{"/backup"}
		}, keyboardWithLayout);
	
	// Время запуска для uptime
	auto botStartTime = chrono::steady_clock::now();

	bot.getEvents().onCommand("start", [&bot, &keyboardWithLayout, &timeout, &afk, &ruletka, &max_lvl_int](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		string welcomeMsg = 
			"VIRTAPP v3.1 STARTED!\n"
			"========================\n"
			"Bot: @" + bot.getApi().getMe()->username + "\n\n"
			"CURRENT SETTINGS:\n"
			"- AFK: " + string(afk ? "ON" : "OFF") + "\n"
			"- Ruletka: " + string(ruletka ? "ON" : "OFF") + "\n"
			"- Target: " + to_string(max_lvl_int) + " lvl\n\n"
			"/help - commands list\n"
			"/status - full status\n\n"
			"Welcome!";
		bot.getApi().sendMessage(message->chat->id, welcomeMsg, false, 0, keyboardWithLayout);
		});
	bot.getEvents().onCommand("gpu", [&bot, &keyboardWithLayout, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		bot.getApi().sendMessage(message->chat->id, getGPUNameFromRegistry());
		});
	bot.getEvents().onCommand("config", std::bind(onConfigCommand, std::placeholders::_1, std::ref(bot)));
	bot.getEvents().onCommand("getlogfile", [&bot, &timeout](TgBot::Message::Ptr message) {

		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		handleGetLogFile(bot, message);
		});
	bot.getEvents().onCommand("hwid", [&bot, &timeout](TgBot::Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			string motherboard_ = exec("wmic baseboard get product,Manufacturer,serialnumber");
			string bios_ = exec("wmic bios get manufacturer,serialnumber,version,smbiosbiosversion,releasedate && wmic csproduct get name, identifyingnumber, uuid");
			string ram_ = exec("wmic memorychip get partnumber, serialnumber, sku");
			string mac_ = exec("wmic nic get macaddress, description");
			string gpu_ = exec("wmic path win32_videocontroller get name | findstr /V \"Parsec\"");
			string cpu_ = exec("wmic cpu get caption, processorid, socketdesignation, Name, Caption");
			string hardDrives_ = exec("wmic logicaldisk where drivetype=3 get name,volumeserialnumber && wmic diskdrive get Model, SerialNumber, name");
			string windows_ = exec("wmic OS GET Caption,SerialNumber,CSName && wmic computersystem get PrimaryOwnerName");
			cout << ("MOTHERBOARD\n" + motherboard_ + "\nBIOS\n" + bios_ + "\nRAM\n" + ram_ + "\nMAC\n" + mac_ + "\nGPU\n" + gpu_ + "\nCPU\n" + cpu_ + "\nHARD DRIVES\n" + hardDrives_ + "\nWINDOWS\n" + windows_) << endl;
			bot.getApi().sendMessage(message->chat->id, ("MOTHERBOARD\n" + motherboard_ + "\nBIOS\n" + bios_ + "\nRAM\n" + ram_ + "\nMAC\n" + mac_ + "\nGPU\n" + gpu_ + "\nCPU\n" + cpu_ + "\nHARD DRIVES\n" + hardDrives_ + "\nWINDOWS\n" + windows_));
		});
	bot.getEvents().onCommand("pycheck", [&bot, &timeout](TgBot::Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			string py_ = exec("where python");
			if (py_.empty())
			{
				bot.getApi().sendMessage(message->chat->id, "NO PYTHON");
			}
			else
			{
				bot.getApi().sendMessage(message->chat->id, py_);
			}
		});
	bot.getEvents().onCommand("profiles", [&bot, &timeout, &login](TgBot::Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			if (login.empty())
			{
				bot.getApi().sendMessage(message->chat->id, "Empty login in config");
				return;
			}
			string command = ("main.py " + login + " Naguibs1337228");
			string py_ = exec(command.c_str());
			if (py_.empty())
			{
				bot.getApi().sendMessage(message->chat->id, "No result. Entered command:\n" + command);
			}
			else
			{
				bot.getApi().sendMessage(message->chat->id, py_);
			}
		});
	bot.getEvents().onCommand("user", [&bot, &timeout, &login, &PCName](TgBot::Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			if (login.empty())
			{
				bot.getApi().sendMessage(message->chat->id, "Empty login in config");
				return;
			}
			string command = ("main.py " + login + " Naguibs1337228 " + PCName);
			string py_ = exec(command.c_str());
			if (py_.empty())
			{
				bot.getApi().sendMessage(message->chat->id, "No result. Entered command:\n" + command);
			}
			else
			{
				bot.getApi().sendMessage(message->chat->id, py_);
			}
		});
	bot.getEvents().onCommand("pyinstall", [&bot, &timeout](TgBot::Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			string py_ = exec("python-3.10.0-amd64.exe /passive InstallAllUsers=1 PrependPath=1 Include_test=0");
			if (py_.empty())
			{
				bot.getApi().sendMessage(message->chat->id, "Too long or empty");
			}
			else
			{
				bot.getApi().sendMessage(message->chat->id, py_);
			}
		});
	bot.getEvents().onCommand("setconfig", [&bot, &awaitingConfigContent, &keyboardWithLayout, &timeout](Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			bot.getApi().sendMessage(message->chat->id, "Send me new config.", false, 0, keyboardWithLayout);
			awaitingConfigContent = true;
		});
	bot.getEvents().onCommand("creds", [&bot, &awaitingCredentialsContent, &timeout](Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			bot.getApi().sendMessage(message->chat->id, "Send me new credentials.json.");
			awaitingCredentialsContent = true;
		});
	bot.getEvents().onCommand("log", [&bot, &keyboardWithLayout, &currentTm, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		for each(string part in splitStringByLength(logprint("Logs was sended to telegram", currentTm), 2048))
		{
			bot.getApi().sendMessage(message->chat->id, "..." + part, false, 0, keyboardWithLayout);
			Sleep(50);
		}
		});
	bot.getEvents().onCommand("update", [&bot, &updating, &keyboardWithLayout, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		bot.getApi().sendMessage(message->chat->id, "Update!", false, 0, keyboardWithLayout);
		updating = true;
		});
	bot.getEvents().onCommand("fps", [&bot, &updating, &keyboardWithLayout, &device, &context, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		bot.getApi().sendMessage(message->chat->id, "fps", false, 0, keyboardWithLayout);
		interception_send(context, device, (InterceptionStroke*)&slash_down, 1);
		Sleep(230);
		interception_send(context, device, (InterceptionStroke*)&slash_up, 1);
		});
	bot.getEvents().onCommand("vip", [&bot, &context, &device, &status, &photoFilePath, &photoMimeType, &scan, &afk, &ruletka, &mouseDevice, &keyboardWithLayout, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		interception_send(context, device, (InterceptionStroke*)&f10_down, 1);
		Sleep(400);
		interception_send(context, device, (InterceptionStroke*)&f10_up, 1);
		Sleep(3000);
		mouse_leftClick(context, mouseDevice, 591, 96);
		Sleep(5000);
		scan.makeScreenshot();
		bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType), bot.getApi().getMe()->username.c_str() + (string)"\n" + status + (string)"\nAFK: " + (afk ? "ON" : "OFF") + (string)"\nRuletka: " + (ruletka ? "ON" : "OFF"), 0, keyboardWithLayout);
		Sleep(3000);
		mouse_leftClick(context, mouseDevice, 1063, 96);
		Sleep(5000);
		scan.makeScreenshot();
		bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType), bot.getApi().getMe()->username.c_str() + (string)"\n" + status + (string)"\nAFK: " + (afk ? "ON" : "OFF") + (string)"\nRuletka: " + (ruletka ? "ON" : "OFF"), 0, keyboardWithLayout);
		interception_send(context, device, (InterceptionStroke*)&esc_down, 1);
		Sleep(500);
		interception_send(context, device, (InterceptionStroke*)&esc_up, 1);
		});
	bot.getEvents().onCommand("bp", [&bot, &context, &device, &photoFilePath, &status, &photoMimeType, &scan, &afk, &ruletka, &mouseDevice, &keyboardWithLayout, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		interception_send(context, device, (InterceptionStroke*)&f10_down, 1);
		Sleep(400);
		interception_send(context, device, (InterceptionStroke*)&f10_up, 1);
		Sleep(3000);
		mouse_leftClick(context, mouseDevice, 1063, 96);
		Sleep(5000);
		scan.makeScreenshot();
		interception_send(context, device, (InterceptionStroke*)&esc_down, 1);
		Sleep(500);
		interception_send(context, device, (InterceptionStroke*)&esc_up, 1);
		bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType), bot.getApi().getMe()->username.c_str() + (string)"\n" + status + (string)"\nAFK: " + (afk ? "ON" : "OFF") + (string)"\nRuletka: " + (ruletka ? "ON" : "OFF"), 0, keyboardWithLayout);
		});
	bot.getEvents().onCommand("dp", [&bot, &context, &device, &photoFilePath, &status, &photoMimeType, &scan, &afk, &ruletka, &mouseDevice, &keyboardWithLayout, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		interception_send(context, device, (InterceptionStroke*)&f10_down, 1);
		Sleep(400);
		interception_send(context, device, (InterceptionStroke*)&f10_up, 1);
		Sleep(3000);
		mouse_leftClick(context, mouseDevice, 956, 96);
		Sleep(4000);
		Sleep(1000);
		scan.makeScreenshot();
		interception_send(context, device, (InterceptionStroke*)&esc_down, 1);
		Sleep(500);
		interception_send(context, device, (InterceptionStroke*)&esc_up, 1);
		bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType), bot.getApi().getMe()->username.c_str() + (string)"\n" + status + (string)"\nAFK: " + (afk ? "ON" : "OFF") + (string)"\nRuletka: " + (ruletka ? "ON" : "OFF"), 0, keyboardWithLayout);
		});
	bot.getEvents().onCommand("cad", [&bot, &context, &device, &mouseDevice, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
		interception_send(context, device, (InterceptionStroke*)&alt_down, 1);
		interception_send(context, device, (InterceptionStroke*)&del_down, 1);
		Sleep(400);
		interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
		interception_send(context, device, (InterceptionStroke*)&alt_up, 1);
		interception_send(context, device, (InterceptionStroke*)&del_up, 1);
		Sleep(5000);
		interception_send(context, device, (InterceptionStroke*)&esc_down, 1);
		Sleep(220);
		interception_send(context, device, (InterceptionStroke*)&esc_up, 1);
		});
	bot.getEvents().onCommand("photo", [&bot, &photoFilePath, &photoMimeType, &scan, &afk, &status, &ruletka, &keyboardWithLayout, &currentTm, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		scan.makeScreenshot();
		string photoCaption = 
			"SCREENSHOT\n"
			"----------------\n"
			"Bot: @" + bot.getApi().getMe()->username + "\n"
			"Status: " + status + "\n"
			"AFK: " + (afk ? "[ON]" : "[OFF]") + "\n"
			"Ruletka: " + (ruletka ? "[ON]" : "[OFF]");
		try
		{
			bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType), photoCaption, 0, keyboardWithLayout);
		}
		catch (const std::exception& e)
		{
			logprint("Failed to send screenshot. Error: " + (string)(e.what()), currentTm);
			bot.getApi().sendMessage(message->chat->id, "Failed to send screenshot. Error: " + (string)(e.what()));
		}
		});
	bot.getEvents().onCommand("photoText", [&bot, &photoFilePath, &photoMimeType, &scan, &currentTm, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		try
		{
			string result = "Result: " + scan.makeText();
			bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType), result);
			bot.getApi().sendDocument(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType));
		}
		catch (const std::exception& e)
		{
			logprint("Failed to send screenshot. Error: " + (string)(e.what()), currentTm);
			bot.getApi().sendMessage(message->chat->id, "Failed to send screenshot. Error: " + (string)(e.what()));
		}
		});
	bot.getEvents().onCommand("checkSpin", [&bot, &photoFilePath, &photoMimeType, &scan, &currentTm, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		try
		{
			string result = "Result: " + scan.makeText(false) + (checkSpin()?"TRUE":"FALSE");
			bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType), result);
			bot.getApi().sendDocument(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType));
		}
		catch (const std::exception& e)
		{
			logprint("Failed to send screenshot. Error: " + (string)(e.what()), currentTm);
			bot.getApi().sendMessage(message->chat->id, "Failed to send screenshot. Error: " + (string)(e.what()));
		}
		});
	bot.getEvents().onCommand("checkQueue", [&bot, &photoFilePath, &photoMimeType, &scan, &currentTm, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}
		try
		{
			string result = "Result: " + scan.makeText(false, 880, 515, 346, 177, 0, 100, 0, 255, 255, 255); + (checkQueue() ? "TRUE" : "FALSE");
			bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType), result);
			bot.getApi().sendDocument(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType));
		}
		catch (const std::exception& e)
		{
			logprint("Failed to send screenshot. Error: " + (string)(e.what()), currentTm);
			bot.getApi().sendMessage(message->chat->id, "Failed to send screenshot. Error: " + (string)(e.what()));
		}
		});

	bot.getEvents().onCommand("backup", [&bot, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			return;
		}
		
		bot.getApi().sendMessage(message->chat->id, "⏳ Creating Golden Session Backup...");
		
		// Execute Python script with --backup flag
		string result = exec("python scripts/epic_auth.py --backup");
		
		if (result.find("BACKUP_OK") != string::npos) {
			bot.getApi().sendMessage(message->chat->id, "✅ Backup Created Successfully!\nSaved to: Documents/EpicSessionBackup");
		} else {
			bot.getApi().sendMessage(message->chat->id, "❌ Backup Failed!\nCheck logs or try manually.");
		}
	});

	bot.getEvents().onCommand("photofile", [&bot, &photoFilePath, &photoMimeType, &scan, &afk, &status, &ruletka, &keyboardWithLayout, &currentTm, &timeout](Message::Ptr message) {
		// Проверяем, что сообщение не слишком старое (например, старше 120 секунд)
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout * 2) {
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			cout << "Ignored command due security settings" << endl;
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
			return;
		}

		// Делаем скриншот
		scan.makeScreenshot();

		try {
			// Отправляем файл как документ, чтобы избежать сжатия
			bot.getApi().sendDocument(message->chat->id,
				InputFile::fromFile(photoFilePath, photoMimeType));
		}
		catch (const std::exception& e) {
			// Логируем ошибку и отправляем сообщение об ошибке в чат
			logprint("Failed to send screenshot as document. Error: " + (string)(e.what()), currentTm);
			bot.getApi().sendMessage(message->chat->id, "Failed to send screenshot as document. Error: " + (string)(e.what()));
		}
		});
	bot.getEvents().onCommand("afk", [&bot, &afk, &keyboardWithLayout, &timeout](Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			string afkStatus = afk ? 
				"AFK: OFF\n================\nAFK mode disabled" : 
				"AFK: ON\n================\nAFK mode enabled";
			bot.getApi().sendMessage(message->chat->id, afkStatus, false, 0, keyboardWithLayout);
			afk = !afk;
		});
	bot.getEvents().onCommand("ruletka", [&bot, &ruletka, &ruletka_user_disabled, &keyboardWithLayout, &timeout](Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			string ruletkaStatus = ruletka ? 
				"Ruletka: OFF\n==================\nRuletka disabled manually" : 
				"Ruletka: ON\n==================\nRuletka enabled!";
			bot.getApi().sendMessage(message->chat->id, ruletkaStatus, false, 0, keyboardWithLayout);
			ruletka = !ruletka;
			// Запоминаем что пользователь выключил рулетку вручную
			ruletka_user_disabled = !ruletka;
		});
	bot.getEvents().onCommand("forcerelogin", [&bot, &forceRelogin, &keyboardWithLayout, &timeout](Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			bot.getApi().sendMessage(message->chat->id, "Force relogin...\n=================\nRelogging now", false, 0, keyboardWithLayout);
			forceRelogin = true;
		});
	bot.getEvents().onCommand("restart", [&bot, &forceRelogin, &keyboardWithLayout, &timeout](Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			bot.getApi().sendMessage(message->chat->id, "PC RESTART\n=================\nPC will restart in 5 seconds...", false, 0, keyboardWithLayout);
			system("shutdown /r /t 5");
		});
	bot.getEvents().onCommand("sysinfo", [&bot, &timeout](Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			PROCESS_MEMORY_COUNTERS_EX pmc;
			MEMORYSTATUSEX statex;
			statex.dwLength = sizeof(statex);
			ostringstream oss;
			if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
			{
				oss << "Process memory information\n";
				oss << "WorkingSetSize: " << pmc.WorkingSetSize / 1024 / 1024 << " MB\n";
				oss << "PrivateUsage: " << pmc.PrivateUsage / 1024 / 1024 << " MB\n";
			}
			else
			{
				oss << "Fail during getting info about memory usage\n";
			}
			if (GlobalMemoryStatusEx(&statex))
			{
				oss << "System memory information\n";
				oss << "Total system memory: " << statex.ullTotalPhys / 1024 / 1024 << " MB\n";
				oss << "Available system memory: " << statex.ullAvailPhys / 1024 / 1024 << " MB";
			}
			else
			{
				oss << "Fail during getting system info";
			}
			bot.getApi().sendMessage(message->from->id, oss.str());
		});
	bot.getEvents().onCommand("cmd", [&bot, &cmd, &keyboardWithLayout, &timeout](Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			bot.getApi().sendMessage(message->chat->id, "Please, enter your command: ", false, 0, keyboardWithLayout);
			cmd = true;
		});
	bot.getEvents().onCommand("lvl", [&bot, &cmd, &keyboardWithLayout, &timeout, &login, &currentTm](Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			string command = "scripts\\getlvl.py " + login + " Naguibs1337228";
			string lvl = exec(command.c_str());
			if (!filterDigits(lvl).empty())
			{
				bot.getApi().sendMessage(message->chat->id, "Level: " + lvl, false, 0, keyboardWithLayout);
			}
			else
			{
				bot.getApi().sendMessage(message->chat->id, "Level not detected (" + lvl + ")", false, 0, keyboardWithLayout);
			}
		});

	// ═══════════════════════════════════════════════════════════════════════════
	// НОВЫЕ КОМАНДЫ 2025 - /help, /status, /menu, /ping, /uptime
	// ═══════════════════════════════════════════════════════════════════════════
	
	// Команда /help - справка по всем командам
	bot.getEvents().onCommand("help", [&bot, &keyboardWithLayout, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			return;
		}
		string helpText = 
			"======================================\n"
			"       VIRTAPP HELP v3.0             \n"
			"======================================\n"
			"\n"
			"INFO:\n"
			"  /status  - Full status\n"
			"  /photo   - Screenshot\n"
			"  /lvl     - Current level\n"
			"  /log     - Recent logs\n"
			"  /sysinfo - System info\n"
			"\n"
			"CONTROL:\n"
			"  /afk     - Toggle AFK mode\n"
			"  /ruletka - Toggle ruletka\n"
			"  /vip     - VIP menu\n"
			"  /bp      - Battle Pass\n"
			"  /dp      - Donator Pass\n"
			"\n"
			"SYSTEM:\n"
			"  /restart - Restart PC\n"
			"  /update  - Update bot\n"
			"  /config  - Show config\n"
			"  /ping    - Check connection\n"
			"  /uptime  - Bot uptime\n"
			"\n"
			"FILES:\n"
			"  /getlogfile - Download logs\n"
			"  /photofile  - Photo as file\n"
			"  /setconfig  - Change config\n"
			"  /creds      - Change creds\n"
			"\n"
			"DEBUG:\n"
			"  /cmd  - Execute command\n"
			"  /hwid - Show HWID\n"
			"  /gpu  - GPU info\n"
			"  /cad  - Ctrl+Alt+Del\n"
			"  /fps  - Current FPS\n"
			"======================================";
		bot.getApi().sendMessage(message->chat->id, helpText, false, 0, keyboardWithLayout);
	});

	// Команда /status - полная статус-карточка
	bot.getEvents().onCommand("status", [&bot, &keyboardWithLayout, &timeout, &afk, &ruletka, &status, &login, &PCName, &botStartTime, &currentTm, &max_lvl_int](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			return;
		}
		
		// Получаем uptime
		auto now = chrono::steady_clock::now();
		auto uptime = chrono::duration_cast<chrono::minutes>(now - botStartTime).count();
		int hours = uptime / 60;
		int mins = uptime % 60;
		
		// Получаем уровень
		string command = "scripts\\getlvl.py " + login + " Naguibs1337228";
		string lvl = exec(command.c_str());
		int current_lvl = 0;
		if (!lvl.empty() && !filterDigits(lvl).empty()) {
			current_lvl = stoi(filterDigits(lvl));
		}
		
		// Получаем память
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		int memPercent = (int)memInfo.dwMemoryLoad;
		
		// Прогресс до max_lvl
		int progress = (max_lvl_int > 0) ? min(100, (current_lvl * 100) / max_lvl_int) : 0;
		string progressBar = "";
		int filled = progress / 10;
		for (int i = 0; i < 10; i++) {
			progressBar += (i < filled) ? "#" : "-";
		}
		
		ostringstream oss;
		oss << "====================================\n";
		oss << "     VIRTAPP STATUS v3.1           \n";
		oss << "====================================\n";
		oss << "PC: " << PCName << "\n";
		oss << "Account: " << login << "\n";
		oss << "Status: " << status << "\n";
		oss << "------------------------------------\n";
		oss << "Level: " << current_lvl << " / " << max_lvl_int << "\n";
		oss << "Progress: [" << progressBar << "] " << progress << "%\n";
		oss << "------------------------------------\n";
		oss << "AFK:      " << (afk ? "[ON] " : "[OFF]");
		if (current_lvl < max_lvl_int) oss << " (auto to " << max_lvl_int << " lvl)";
		oss << "\n";
		oss << "Ruletka: " << (ruletka ? "[ON] " : "[OFF]");
		if (current_lvl < 3) oss << " (from 3 lvl)";
		oss << "\n";
		oss << "------------------------------------\n";
		oss << "Uptime: " << hours << "h " << mins << "m\n";
		oss << "RAM: " << memPercent << "%\n";
		oss << "====================================";
		
		bot.getApi().sendMessage(message->chat->id, oss.str(), false, 0, keyboardWithLayout);
	});

	// Команда /menu - главное меню с категориями
	bot.getEvents().onCommand("menu", [&bot, &keyboardWithLayout, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			return;
		}
		string menuText = 
			"MAIN MENU VIRTAPP\\n"
			"========================\\n\\n"
			"STATUS:\\n"
			"- /status - Full status\\n"
			"- /photo  - Screenshot\\n"
			"- /lvl    - Level\\n\\n"
			"CONTROL:\\n"
			"- /afk     - AFK mode\\n"
			"- /ruletka - Ruletka\\n"
			"- /vip     - VIP menu\\n\\n"
			"SYSTEM:\\n"
			"- /restart - Restart\\n"
			"- /sysinfo - System info\\n"
			"- /config  - Config\\n\\n"
			"/help - All commands";
		bot.getApi().sendMessage(message->chat->id, menuText, false, 0, keyboardWithLayout);
	});

	// Команда /ping - проверка связи
	bot.getEvents().onCommand("ping", [&bot, &keyboardWithLayout, &timeout](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			return;
		}
		auto now = chrono::system_clock::now();
		auto nowSec = chrono::duration_cast<chrono::seconds>(now.time_since_epoch()).count();
		int latency = nowSec - message->date;
		
		ostringstream oss;
		oss << "PONG!\n";
		oss << "================\n";
		oss << "Bot is active\n";
		oss << "Latency: " << latency << " sec";
		bot.getApi().sendMessage(message->chat->id, oss.str(), false, 0, keyboardWithLayout);
	});

	// Команда /uptime - время работы
	bot.getEvents().onCommand("uptime", [&bot, &keyboardWithLayout, &timeout, &botStartTime](Message::Ptr message) {
		if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
		{
			cout << "Ignored command due the timeout!" << endl;
			return;
		}
		if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
		{
			bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
			return;
		}
		auto now = chrono::steady_clock::now();
		auto uptimeMin = chrono::duration_cast<chrono::minutes>(now - botStartTime).count();
		int days = uptimeMin / 1440;
		int hours = (uptimeMin % 1440) / 60;
		int mins = uptimeMin % 60;
		
		ostringstream oss;
		oss << "UPTIME\n";
		oss << "================\n";
		if (days > 0) oss << "Days: " << days << "\n";
		oss << "Hours: " << hours << "\n";
		oss << "Minutes: " << mins;
		bot.getApi().sendMessage(message->chat->id, oss.str(), false, 0, keyboardWithLayout);
	});

	bot.getEvents().onAnyMessage([&bot, &cmd, &awaitingConfigContent, &awaitingCredentialsContent, &rename, &timeout](Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			if (message->from->id == 517005065 && cmd)
			{
				string cmd_result = exec(message->text.c_str());
				try
				{
					bot.getApi().sendMessage(message->chat->id, cmd_result, false, 0);
				}
				catch (const std::exception&)
				{
					bot.getApi().sendMessage(message->chat->id, "Result is too long", false, 0);
				}
				cmd = false;
			}
			if (awaitingConfigContent) {
				if (message->text.empty()) {
					bot.getApi().sendMessage(message->chat->id, "Message is empty, config wasn't updated.");
				}
				else {
					std::ofstream configFile("config.txt");
					if (configFile.is_open()) {
						configFile << message->text;
						configFile.close();
						bot.getApi().sendMessage(message->chat->id, "File config.txt was updated.");
					}
					else {
						bot.getApi().sendMessage(message->chat->id, "Unable to open file config.txt. Config wasn't updated.");
					}
				}
				string test;
				awaitingConfigContent = false;  // Отключаем режим ожидания после обновления файла
			}
			if (awaitingCredentialsContent)
			{
				if (message->text.empty()) {
					bot.getApi().sendMessage(message->chat->id, "Message is empty, credentials.json wasn't updated.");
				}
				else {
					std::ofstream configFile("credentials.json");
					if (configFile.is_open()) {
						configFile << message->text;
						configFile.close();
						bot.getApi().sendMessage(message->chat->id, "File credentials.json was updated.");
					}
					else {
						bot.getApi().sendMessage(message->chat->id, "Unable to open file credentials.json. credentials.json wasn't updated.");
					}
				}
				awaitingCredentialsContent = false;
			}
		});
	bot.getEvents().onUnknownCommand([&bot, &timeout](Message::Ptr message)
		{
			if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() - message->date > timeout)
			{
				cout << "Ignored command due the timeout!" << endl;
				return;
			}
			if (find(tgListSecurity.begin(), tgListSecurity.end(), to_string(message->from->id)) == tgListSecurity.end())
			{
				cout << "Ignored command due security settings" << endl;
				bot.getApi().sendMessage(message->chat->id, "You don't have permission to use this bot");
				bot.getApi().sendSticker(message->chat->id, "CAACAgEAAxkBAAExWJRnnoCj6G2IwoOoeyqX4CRurvrfLwACkQEAAt96sUfMX1uymPvn9jYE");
				return;
			}
			else
			{
				bot.getApi().sendMessage(message->chat->id, "Unknown command");
				bot.getApi().sendSticker(message->chat->id, stickers["unknown_command"]);
			}
		});



	signal(SIGINT, [](int s) {
		printf("SIGINT got\n");
		exit(0);
		});
	thread tgBot([&]()
		{
			while (true)
			{
				Sleep(5000);
				try {
					string botName = bot.getApi().getMe()->username;
					printSection("TELEGRAM BOT CONNECTED");
					printStatus("Bot Username", "@" + botName, 10);
					printStatus("Status", "ONLINE", 10);
					printSectionEnd();
					printf("Bot username: %s\n", botName.c_str());
					bot.getApi().deleteWebhook();
					TgLongPoll longPoll(bot);
					while (true) {
						longPoll.start();
					}
				}
				catch (exception& e) {
					printError("Telegram Bot Error: " + (string)e.what());
					logprint("error: " + (string)e.what(), currentTm);
					//printf("error: %s\n", e.what());
				}
			}
		});
	tgBot.detach();
	//Server Init
	logprint("Downloading python...", currentTm);
	system("python-3.10.0-amd64.exe /quiet InstallAllUsers=1 PrependPath=1 SimpleInstall=1");
	logprint("Downloading libs", currentTm);
	system("main.py");
	system("command_cmd.bat");
	


	while (checkIP(ipList))
	{
		logprint("YOU\'RE USING REAL IP!", currentTm);
		status = "USING REAL IP";
		Sleep(5000);
	}
	/*
	std::ofstream settingsFile(userProfile + (string)"\\Documents\\Rockstar Games\\GTA V\\settings.xml");
	if (settingsFile.is_open() && !gpuname.empty()) {
		settingsFile << gta5Settings;
		settingsFile.close();
		logprint("Setting replaced!", currentTm);
	}
	else
	{
		settingsFile.close();
		logprint("SETTINGS WASN'T REPLACED", currentTm);
	}*/
	thread server([&currentTm]()
		{
			while (true)
			{
				Sleep(60 * 1000);
				WSADATA wsaData;
				if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
				{
					logprint("Server startup failed!", currentTm);
					continue;
				}

				SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
				if (client_socket == INVALID_SOCKET)
				{
					logprint("Socket creation failed!", currentTm);
					WSACleanup();
					continue;
				}

				sockaddr_in server_adress;
				server_adress.sin_family = AF_INET;
				server_adress.sin_port = htons(5555);
				inet_pton(AF_INET, "193.168.227.189", &server_adress.sin_addr);//193.168.227.189

				if ((connect(client_socket, (sockaddr*)&server_adress, sizeof(server_adress)) == SOCKET_ERROR))
				{
					logprint("Connection to server failed!", currentTm);
					closesocket(client_socket);
					WSACleanup();
					continue;
				}
				string message(getenv("TOKEN"));
				send(client_socket, message.c_str(), message.size(), 0);
				closesocket(client_socket);
				WSACleanup();
			}
		}
	);
	server.detach();



	wchar_t hard_id[512];
	wchar_t hard_mid[512];
	for (size_t i = 0; i < INTERCEPTION_MAX_KEYBOARD; i++)
	{
		size_t keyboard = INTERCEPTION_KEYBOARD(i);
		logprint(interception_is_keyboard(keyboard) ? "Keyboard initialized" : "Keyboard error", currentTm);
		//cout << (interception_is_keyboard(keyboard) ? "Keyboard initialized\n" : "Keyboard error\n");
		if (interception_is_keyboard(keyboard))
		{
			size_t size = interception_get_hardware_id(context, keyboard, hard_id, 512);
			if (size != 0)
			{
				device = keyboard;
				break;
			}
		}
	}
	for (size_t i = 0; i < INTERCEPTION_MAX_MOUSE; i++)
	{
		size_t mouse = INTERCEPTION_MOUSE(i);
		logprint(interception_is_mouse(mouse) ? "Mouse initialized" : "Mouse error", currentTm);
		//cout << (interception_is_mouse(mouse) ? "Mouse initialized\n" : "Mouse error\n");
		if (interception_is_mouse(mouse))
		{
			size_t size = interception_get_hardware_id(context, mouse, hard_mid, 512);
			if (size != 0)
			{
				mouseDevice = mouse;
				break;
			}
		}
	}

	while (afkonly)
	{
		Sleep(40000);
		interception_send(context, device, (InterceptionStroke*)&w_down, 1);
		Sleep(400);
		interception_send(context, device, (InterceptionStroke*)&w_up, 1);
		Sleep(400);
		interception_send(context, device, (InterceptionStroke*)&s_down, 1);
		Sleep(400);
		interception_send(context, device, (InterceptionStroke*)&s_up, 1);
		Sleep(400);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// FPS LIMIT CONTROL (Bandicam)
	// ═══════════════════════════════════════════════════════════════════════════
	auto setFpsLimit = [&](bool enable) {
		HKEY hKey;
		LPCSTR subKey = "Software\\BANDISOFT\\BANDICAM\\OPTION";
		LPCSTR valueName = "bFpsLimit";
		DWORD value;
		DWORD dataSize = sizeof(value);
		DWORD valueType;

		if (RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			if (RegQueryValueExA(hKey, valueName, NULL, &valueType, (LPBYTE)&value, &dataSize) == ERROR_SUCCESS) {
				// value == 1 (Limit ON), value == 0 (Limit OFF)
				bool isEnabled = (value == 1);
				
				if (isEnabled != enable) {
					logprint(enable ? "Enabling FPS limit (Low FPS)..." : "Disabling FPS limit (High FPS for loading)...", currentTm);
					interception_send(context, device, (InterceptionStroke*)&slash_down, 1);
					Sleep(100);
					interception_send(context, device, (InterceptionStroke*)&slash_up, 1);
					Sleep(500); // Wait for Bandicam to react
				} else {
					// logprint(enable ? "FPS limit already ENABLED" : "FPS limit already DISABLED", currentTm);
				}
			}
			RegCloseKey(hKey);
		}
	};

	thread hotkeys([&]()
		{
			string relogin_is;
			string afk_is;
			string ruletka_is;

			while (true)
			{
				Sleep(100);
				if (GetKeyState(VK_F7) & 0x8000)
				{
					ruletka = !ruletka;
					// Запоминаем что пользователь выключил рулетку вручную
					ruletka_user_disabled = !ruletka;
					ruletka_is = ruletka ? "is on" : "is off";
					logprint("Ruletka " + ruletka_is, currentTm);
					//cout << "Ruletka " << ruletka_is << ".\n";
					Sleep(350);
				}
				if (GetKeyState(VK_F8) & 0x8000)
				{
					afk = !afk;
					afk_is = afk ? "is on" : "is off";
					logprint("AFK " + afk_is, currentTm);
					//cout << "AFK " << afk_is << ".\n";
					Sleep(350);
				}
				if (GetKeyState(VK_F6) & 0x8000)
				{
					relogin = !relogin;
					relogin_is = relogin ? "is on" : "is off";
					logprint("Relogin " + relogin_is, currentTm);
					//cout << "Relogin " << relogin_is << ".\n";
					Sleep(350);
				}
			}
		});
	hotkeys.detach();

	// ═══════════════════════════════════════════════════════════════════════════
	// SESSION GUARD (Backup Epic Games tokens every 5 mins)
	// ═══════════════════════════════════════════════════════════════════════════
	thread sessionGuard([&currentTm]() {
		logprint("Session Guard: STARTED (Interval: 5 min)", currentTm);
		while (true) {
			// Sleep 5 minutes (300,000 ms)
			Sleep(300000);

			logprint("Session Guard: Creating backup...", currentTm);
			// Run backup script
			system("python scripts/epic_auth.py --backup-only");
		}
		});
	sessionGuard.detach();

	HWND Console = GetConsoleWindow();
	PostMessage(Console, WM_SYSCOMMAND, SC_MINIMIZE, 0);
	if (checkBanInTable(PCName))
	{
		banned = true;
	}
	else
	{
		banned = false;
	}
	while (banned)
	{
		status = "BANNED\\BLOCKED";
		Sleep(10000);
	}
	updateStorageJson(getServerIp(PCName));
	
	// ═══════════════════════════════════════════════════════════════════════════
	// Вывод текущего состояния бота
	// ═══════════════════════════════════════════════════════════════════════════
	printSection("BOT STATUS");
	printStatus("Ruletka", ruletka ? "ON" : "OFF", ruletka ? 10 : 12);
	printStatus("AFK Mode", afk ? "ON" : "OFF", afk ? 10 : 8);
	printStatus("Presents", present_activity ? "ENABLED" : "DISABLED", present_activity ? 10 : 8);
	printSectionEnd();
	
	//AutoStart GTA
	string RESTART = "0";
	printSection("GAME LAUNCHER");
	printOK("Starting GTA5RP...");
	printSectionEnd();
	
	logprint("Restart init", currentTm);
	while (!isRunningP(L"GTA5.exe") && !checkIP(ipList))
	{
		status = "Restart";
		int waiting = 0;
		logprint("Restart", currentTm);
		int attemp = 1;
		while (!isRunningP(L"GTA5.exe")) {
			Sleep(3000);
			ShellExecuteA(nullptr, "open", "C:\\Program Files (x86)\\Epic Games\\Launcher\\Portal\\Binaries\\Win32\\EpicGamesLauncher.exe", nullptr, nullptr, SW_SHOWNORMAL);
			Sleep(3000);
			
			// ═══════════════════════════════════════════════════════════════════════════
			// EPIC GAMES AUTO-AUTH - Синхронный запуск (C++ ждёт завершения Python)
			// Скрипт работает до 5 минут, держит окно Epic активным на переднем плане
			// После завершения скрипта C++ продолжает работу
			// ═══════════════════════════════════════════════════════════════════════════
			if (epicAuthEnabled)
			{
				logprint("=== EPIC AUTH START ===", currentTm);
				logprint("Running epic_auth.py (up to 5 min)...", currentTm);
				logprint("Script will keep Epic window active and handle login if needed", currentTm);
				
				// Синхронный запуск - C++ ждёт пока Python завершится!
				// Используем system() без start - это блокирующий вызов
				int result = system("python scripts/epic_auth.py");
				
				if (result == 0)
				{
					logprint("=== EPIC AUTH COMPLETE (success) ===", currentTm);
				}
				else
				{
					logprint("=== EPIC AUTH COMPLETE (code: " + to_string(result) + ") ===", currentTm);
				}
			}
			else
			{
				logprint("Epic auth script SKIPPED (credentials not set in config.txt)", currentTm);
			}
			
			Sleep(2000);
			system("start C:\\Users\\%USERNAME%\\Desktop\\GTA5.lnk");
			logprint("Started launcher", currentTm);
			
			// DISABLE FPS LIMIT FOR FASTER LOADING
			setFpsLimit(false);
			
			while (!isRunning("RAGE Multiplayer"))
			{
				logprint("Waiting launcher", currentTm);
				waiting++;
				if (waiting % 270 == 0)
				{
					system("taskkill /F /IM ragemp_v.exe");
					system("taskkill /F /IM updater.exe");
					system("taskkill /F /IM Epic*");
					system("taskkill /F /IM Rockstar*");
					logprint("Killed all updaters", currentTm);
				}
				if (waiting % 90 == 0)
				{
					Sleep(5000);
					ShellExecuteA(nullptr, "open", "C:\\Program Files (x86)\\Epic Games\\Launcher\\Portal\\Binaries\\Win32\\EpicGamesLauncher.exe", nullptr, nullptr, SW_SHOWNORMAL);
					Sleep(5000);
					system("start C:\\Users\\%USERNAME%\\Desktop\\GTA5.lnk");
					logprint("Started launcher", currentTm);
				}
				Sleep(1000);
			}
			Sleep(5000);
			logprint("Trying to find launcher window", currentTm);
			HWND Launcher(FindWindowA(NULL, "RAGE Multiplayer"));
			logprint("Making launcher foreground", currentTm);
			SetForegroundWindow(Launcher);
			int restartwait = 0;
			for (size_t i = 0; i < 10; i++)
			{
				logprint("Checking that launcher is fully loaded", currentTm);
				for (int x = 618; x < 664; x++) {
					for (int y = 15; y < 64; y++) {
						if (scan.checkPixel(x, y, 255, 206, 52))
						{
							i--;
							break;
						}
						i = i + 3;
					}
				}
				restartwait++;
				if (restartwait >= 100)
				{
					try
					{
						bot.getApi().sendMessage(517005065, "Long restart, restarting", false, 0, nullptr, "", true);
					}
					catch (const std::exception&)
					{
						logprint("Restarting pc", currentTm);
					}
					Sleep(5000);
					if (!donotrestart)
					{
						system("shutdown /r /t 5");
					}

				}
				Sleep(1000);
			}
			Sleep(10000);
			logprint("Clicking server", currentTm);
			mouse_leftClick(context, mouseDevice, 272, 146);
			Sleep(1000);
			logprint("Clicking server", currentTm);
			mouse_leftClick(context, mouseDevice, 272, 146);
			logprint("Checking bandicam", currentTm);
			if (!isRunningP(L"bdcam.exe"))
			{
				logprint("Starting bandicam", currentTm);
				thread bandicam([] {
					system("\"C:\\Program Files\\Bandicam\\bdcam.exe\"");
					});
				bandicam.detach();
			}
			logprint("Bandicam is running", currentTm);


			int waitinggta5 = 0;
			logprint("Checking gta is running", currentTm);
			while (!isRunningP(L"GTA5.exe"))
			{
				logprint("Waiting for GTA (" + to_string(waitinggta5) + (string)"s)", currentTm);
				Sleep(1000);
				waitinggta5++;
				if (waitinggta5 == 300)
				{
					status = "Failed to start game. Attemp #" +to_string(attemp);
					attemp++;
				}
				if (waitinggta5 == 300)
				{
					system("taskkill /F /IM rage*");
					system("taskkill /F /IM updater.exe");
					system("taskkill /F /IM Epic*");
					system("taskkill /F /IM Rockstar*");
					logprint("Killed all gta processes", currentTm);
					break;
				}
				if (waitinggta5 >= 60 && waitinggta5 < 240 && waitinggta5%10 == 0 && !mail.empty() && !password.empty() && !imapip.empty() && !socpass.empty() && !socmail.empty())
				{
					logprint("Trying to find rockstar login window", currentTm);
					int redpix = 0;
					for (int x = 0; x < 1280; x = x + 100)
					{
						for (int y = 0; y < 720; y = y + 23)
						{
							redpix = scan.checkPixel(x, y, 189, 8, 8) ? redpix + 1 : redpix;
						}
					}
					logprint("Redpix count: " + to_string(redpix), currentTm);
					if (redpix >= 6)
					{
						// ENABLE FPS LIMIT FOR STABILITY (Login Screen)
						setFpsLimit(true);
						
						//Rockstar sign in

						//Copying mail to clipboard
						addToClipboard(socmail);

						logprint("Entering login", currentTm);
						Sleep(5000);
						interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
						interception_send(context, device, (InterceptionStroke*)&a_down, 1);
						Sleep(400);
						interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
						interception_send(context, device, (InterceptionStroke*)&a_up, 1);
						Sleep(400);
						interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
						interception_send(context, device, (InterceptionStroke*)&v_down, 1);
						Sleep(400);
						interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
						interception_send(context, device, (InterceptionStroke*)&v_up, 1);
						Sleep(400);
						logprint("Switching to password", currentTm);
						interception_send(context, device, (InterceptionStroke*)&tab_down, 1);
						Sleep(400);
						interception_send(context, device, (InterceptionStroke*)&tab_up, 1);
						Sleep(2000);
						addToClipboard(socpass);
						Sleep(1000);
						logprint("Entering password", currentTm);
						interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
						interception_send(context, device, (InterceptionStroke*)&v_down, 1);
						Sleep(400);
						interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
						interception_send(context, device, (InterceptionStroke*)&v_up, 1);
						Sleep(400);
						logprint("Connecting to imap", currentTm);
						try
						{
							mailio::imaps conn(imapip, imapport);
							logprint("IMAP aunthification", currentTm);
							conn.authenticate(mail, mpass, mailio::imaps::auth_method_t::LOGIN);
							mailio::message msg;
							msg.line_policy(mailio::codec::line_len_policy_t::NONE, mailio::codec::line_len_policy_t::NONE);
							conn.select("inbox");
							logprint("Collecting information about last message", currentTm);

							mailio::imaps::mailbox_stat_t stat = conn.statistics("inbox");
							conn.fetch(stat.messages_no, msg);
							boost::local_time::local_date_time queryTime(boost::posix_time::second_clock::universal_time(),
								boost::local_time::time_zone_ptr(new boost::local_time::posix_time_zone("UTC")));

							// Проверка условия и присваивание значения переменной queryTime
							if (msg.date_time().is_not_a_date_time())
							{
								boost::posix_time::ptime utcTime = boost::posix_time::second_clock::universal_time();
								queryTime = boost::local_time::local_date_time(utcTime,
									boost::local_time::time_zone_ptr(new boost::local_time::posix_time_zone("UTC")));
								logprint("Last message datetime: " + queryTime.to_string(), currentTm);
							}
							else
							{
								queryTime = boost::local_time::local_date_time(msg.date_time().utc_time(),
									boost::local_time::time_zone_ptr(new boost::local_time::posix_time_zone("UTC")));
								logprint("Last message datetime: " + queryTime.to_string(), currentTm);
							}




							logprint("Pressing enter", currentTm);
							Sleep(3000);
							interception_send(context, device, (InterceptionStroke*)&enter_down, 1);
							Sleep(330);
							interception_send(context, device, (InterceptionStroke*)&enter_up, 1);



							Sleep(10000);

							regex r(R"(\s(\d{6})\s)");
							smatch match;
							string code;

							while (code.empty())
							{
								logprint("Searching for rockstar code", currentTm);
								list<unsigned long> msg_ids;
								list<mailio::imaps::search_condition_t> criterias;
								criterias.push_back(mailio::imaps::search_condition_t(mailio::imaps::search_condition_t::SUBJECT, std::string("Your Rockstar Games verification code")));
								conn.select("inbox");
								conn.search(criterias, msg_ids);
								logprint("Number of founded messages : " + msg_ids.size(), currentTm);
								logprint("Filtering messages", currentTm);
								for each(unsigned long message in msg_ids)
								{
									conn.fetch(message, msg);
									if (msg.date_time() > queryTime)
									{
										string msgcontent = msg.parts()[0].content();
										if (regex_search(msgcontent, match, r))
										{
											code = match[1];
											logprint("Your code is : " + code, currentTm);
										}
										else
										{
											logprint("Code wasn't founded", currentTm);
										}
										logprint("Datetime : " + msg.date_time().to_string(), currentTm);
									}
									else
									{
										logprint("Waiting for code", currentTm);
									}
								}
								Sleep(60000);
							}
							addToClipboard(code);
							logprint("Switching to code holder", currentTm);
							for (int i = 0; i < 10; i++)
							{
								interception_send(context, device, (InterceptionStroke*)&tab_down, 1);
								Sleep(300);
								interception_send(context, device, (InterceptionStroke*)&tab_up, 1);
								Sleep(2000);
							}
							logprint("Entering code", currentTm);
							interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
							interception_send(context, device, (InterceptionStroke*)&v_down, 1);
							Sleep(400);
							interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
							interception_send(context, device, (InterceptionStroke*)&v_up, 1);
							Sleep(400);
							scan.makeScreenshot();
							for (string id : tgListLoging)
							{
								int wait = 0;
								while (wait < 5)
								{
									try
									{
										bot.getApi().sendPhoto(id, InputFile::fromFile(photoFilePath, photoMimeType), "Entered rockstar code", 0, nullptr, "", true);
										break;
									}
									catch (const std::exception& e)
									{
										logprint("Failed to send screenshot. Error: " + (string)(e.what()), currentTm);
										try
										{
											bot.getApi().sendMessage(517005065, "Failed to send screenshot. Error: " + (string)(e.what()));
										}
										catch (const std::exception&)
										{
											logprint("ERROR: Failed to send error message to Telegram: " + (string)e.what(), currentTm);
										}
									}
									wait++;
									Sleep(1000);
								}
							}
							interception_send(context, device, (InterceptionStroke*)&enter_down, 1);
							Sleep(330);
							interception_send(context, device, (InterceptionStroke*)&enter_up, 1);


							waitinggta5 = 0;

							//Rockstar sign in
						}
						catch (const std::exception& e)
						{
							logprint("Error during signin to Social: " + (string)e.what(), currentTm);
						}
					}
					else
					{
						try
						{
							redpix = 0;
							string screenText = scan.makeText();
							regex codeScreen(R"(Verify your email)");
							smatch match;
							if (regex_search(screenText, match, codeScreen))
							{
								mailio::imaps conn(imapip, imapport);
								logprint("IMAP aunthification", currentTm);
								conn.authenticate(mail, mpass, mailio::imaps::auth_method_t::LOGIN);
								mailio::message msg;
								msg.line_policy(mailio::codec::line_len_policy_t::NONE, mailio::codec::line_len_policy_t::NONE);
								conn.select("inbox");
								logprint("Collecting information about last message", currentTm);

								mailio::imaps::mailbox_stat_t stat = conn.statistics("inbox");
								conn.fetch(stat.messages_no, msg);

								regex r(R"(\s(\d{6})\s)");
								smatch match1;
								string code;
								string msgcontent = msg.parts()[0].content();
								if (regex_search(msgcontent, match1, r))
								{
									code = match1[1];
									logprint("Your code is : " + code, currentTm);
								}
								else
								{
									logprint("Code wasn't founded", currentTm);
								}
								logprint("Datetime : " + msg.date_time().to_string(), currentTm);
								addToClipboard(code);
								Sleep(2000);
								logprint("Switching to code holder", currentTm);
								for (size_t i = 0; i < 11; i++)
								{
									interception_send(context, device, (InterceptionStroke*)&tab_down, 1);
									Sleep(300);
									interception_send(context, device, (InterceptionStroke*)&tab_up, 1);
									Sleep(2000);
									scan.makeScreenshot();
									try
									{
										bot.getApi().sendPhoto(517005065, InputFile::fromFile(photoFilePath, photoMimeType), "i: " + i, 0, keyboardWithLayout);
									}
									catch (const std::exception& e)
									{
										logprint("Failed to send screenshot. Error: " + (string)(e.what()), currentTm);
										bot.getApi().sendMessage(517005065, "Failed to send screenshot. Error: " + (string)(e.what()));
									}
								}
								logprint("Entering code", currentTm);
								interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
								interception_send(context, device, (InterceptionStroke*)&v_down, 1);
								Sleep(400);
								interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
								interception_send(context, device, (InterceptionStroke*)&v_up, 1);
								Sleep(400);
								interception_send(context, device, (InterceptionStroke*)&enter_down, 1);
								Sleep(330);
								interception_send(context, device, (InterceptionStroke*)&enter_up, 1);

							}
						}
						catch (const std::exception& e)
						{
							logprint("Error: " + (string)e.what(), currentTm);
						}


					}
				}
			}
		}
		Sleep(30000);
		system("taskkill /F /IM AutoHotkey*");
		logprint("Trying to find window", currentTm);
		HWND lHwnd = FindWindow("Shell_TrayWnd", NULL);
		SendMessage(lHwnd, WM_COMMAND, 419, 0);
		Sleep(5000);
		HWND Gta5(FindWindowA(NULL, "RAGE Multiplayer"));
		if (Gta5 == NULL)
		{
			logprint("Failed to find window.", currentTm);
		}
		logprint("Window was founded", currentTm);
		PostMessage(Gta5, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		SetForegroundWindow(Gta5);
		logprint("Making window active", currentTm);
		Sleep(10000);
		system("taskkill /F /IM updater.exe");
		system("taskkill /F /IM ragemp_v.exe");
		logprint("Killed all updaters", currentTm);
		mouse_leftClick(context, mouseDevice, 5, 5);
		Sleep(1000);
		mouse_leftClick(context, mouseDevice, 5, 5);
		logprint("Waiting for login screen", currentTm);
		int waitinglogin = 0;
		while (!scan.checkPixel(995, 252, 255, 255, 255) && !scan.checkPixel(1180, 251, 255, 255, 255) && !scan.checkPixel(1117, 327, 245, 244, 245) && !forceRelogin)
		{
			Sleep(30 * 1000);
			cout << " ...";
			waitinglogin++;
			if (waitinglogin >= 30)
			{
				if (!donotrestart)
				{
					system("shutdown /r /t 5");
				}
			}
		}
		Sleep(60000);
		while (!scan.checkPixel(995, 252, 255, 255, 255) && !scan.checkPixel(1180, 251, 255, 255, 255) && !scan.checkPixel(1117, 327, 245, 244, 245) && !forceRelogin) { Sleep(30 * 1000); cout << " ..."; }
		Sleep(10 * 1000);
		//Logining
		//cad
		//Character choosing

		while (scan.checkPixel(995, 252, 255, 255, 255) && scan.checkPixel(1180, 251, 255, 255, 255) && !scan.checkPixel(188, 107, 73, 87, 121, 5))
		{

			logprint("Ctrl+Alt+Del", currentTm);
			interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
			interception_send(context, device, (InterceptionStroke*)&alt_down, 1);
			interception_send(context, device, (InterceptionStroke*)&del_down, 1);
			Sleep(400);
			interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
			interception_send(context, device, (InterceptionStroke*)&alt_up, 1);
			interception_send(context, device, (InterceptionStroke*)&del_up, 1);
			Sleep(5000);
			interception_send(context, device, (InterceptionStroke*)&esc_down, 1);
			Sleep(220);
			interception_send(context, device, (InterceptionStroke*)&esc_up, 1);
			Sleep(10000);
			cout << " ...";
			HWND Gta5(FindWindowA(NULL, "RAGE Multiplayer"));
			if (Gta5 == NULL)
			{
				logprint("Failed to find window.", currentTm);
			}
			else
			{
				logprint("Window was founded", currentTm);
				PostMessage(Gta5, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				SetForegroundWindow(Gta5);
				logprint("Making window active", currentTm);
			}
			Sleep(5000);
			mouse_leftClick(context, mouseDevice, 10, 10);
			Sleep(3000);
			logprint("Pressing tab", currentTm);
			Sleep(1000);
			interception_send(context, device, (InterceptionStroke*)&tab_down, 1);
			Sleep(270);
			interception_send(context, device, (InterceptionStroke*)&tab_up, 1);
			Sleep(3000);
			logprint("Typing login: " + login, currentTm);
			//Copying login to clipboard
			addToClipboard(login);

			//Ctrl+A
			interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
			Sleep(32);
			interception_send(context, device, (InterceptionStroke*)&a_down, 1);
			Sleep(112);
			interception_send(context, device, (InterceptionStroke*)&a_up, 1);
			Sleep(45);
			interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
			Sleep(2000);
			//Ctrl+V
			interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
			Sleep(32);
			interception_send(context, device, (InterceptionStroke*)&v_down, 1);
			Sleep(112);
			interception_send(context, device, (InterceptionStroke*)&v_up, 1);
			Sleep(45);
			interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
			logprint("Pressing tab", currentTm);
			Sleep(1000);
			interception_send(context, device, (InterceptionStroke*)&tab_down, 1);
			Sleep(270);
			interception_send(context, device, (InterceptionStroke*)&tab_up, 1);
			Sleep(3000);
			logprint("Typing password: " + password, currentTm);
			//Copying login to clipboard
			addToClipboard(password);

			//Ctrl+A
			interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
			Sleep(32);
			interception_send(context, device, (InterceptionStroke*)&a_down, 1);
			Sleep(112);
			interception_send(context, device, (InterceptionStroke*)&a_up, 1);
			Sleep(45);
			interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
			Sleep(2000);
			//Ctrl+V
			interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
			Sleep(32);
			interception_send(context, device, (InterceptionStroke*)&v_down, 1);
			Sleep(112);
			interception_send(context, device, (InterceptionStroke*)&v_up, 1);
			Sleep(45);
			interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
			Sleep(10000);
			interception_send(context, device, (InterceptionStroke*)&enter_down, 1);
			Sleep(300);
			interception_send(context, device, (InterceptionStroke*)&enter_up, 1);
			mouse_leftClick(context, mouseDevice, 1032, 513);
			logprint("Click login button", currentTm);
			Sleep(60000);
			/*int counter = 0;
			while (!scan.checkPixel(928,374,9,104,216,20))
			{
				counter++;
				if (counter % 6 == 0)
				{
					mouse_leftClick(context, mouseDevice, 10, 10);
					Sleep(3000);
				}
				logprint("Pressing tab",currentTm);
				Sleep(1000);
				interception_send(context, device, (InterceptionStroke*)&tab_down, 1);
				Sleep(270);
				interception_send(context, device, (InterceptionStroke*)&tab_up, 1);
				Sleep(3000);
				if (scan.checkPixel(16,697,255,255,255))
				{
					while (scan.checkPixel(16, 697, 255, 255, 255) || scan.checkPixel(637,25,57,57,57))
					{
						interception_send(context, device, (InterceptionStroke*)&alt_down, 1);
						Sleep(100);
						interception_send(context, device, (InterceptionStroke*)&tab_down, 1);
						Sleep(270);
						interception_send(context, device, (InterceptionStroke*)&tab_up, 1);
						Sleep(150);
						interception_send(context, device, (InterceptionStroke*)&alt_up, 1);
						Sleep(5000);
					}
				}
			}
			interception_send(context, device, (InterceptionStroke*)&tab_down, 1);
			Sleep(270);
			interception_send(context, device, (InterceptionStroke*)&tab_up, 1);
			Sleep(5000);
			//password
			logprint("Typing password: " + password, currentTm);
			//Copying password to clipboard
			addToClipboard(password);

			//Ctrl+A
			interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
			Sleep(32);
			interception_send(context, device, (InterceptionStroke*)&a_down, 1);
			Sleep(112);
			interception_send(context, device, (InterceptionStroke*)&a_up, 1);
			Sleep(45);
			interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
			Sleep(2000);
			//Ctrl+V
			interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
			Sleep(32);
			interception_send(context, device, (InterceptionStroke*)&v_down, 1);
			Sleep(112);
			interception_send(context, device, (InterceptionStroke*)&v_up, 1);
			Sleep(45);
			interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
			Sleep(10000);
			interception_send(context, device, (InterceptionStroke*)&enter_down, 1);
			Sleep(300);
			interception_send(context, device, (InterceptionStroke*)&enter_up, 1);
			mouse_leftClick(context, mouseDevice, 1032, 513);
			logprint("Click login button", currentTm);
			Sleep(60000);*/
		}
		while (!scan.checkPixel(149, 122, 73, 87, 121,5) && !scan.checkPixel(159, 99, 73, 87, 121,5))
		{
			Sleep(2000);
			logprint("Waiting for character choosing screen", currentTm);
		}
		while (!scan.checkPixel(744, 607, 255, 178, 0, 5))
		{
			switch (ActiveCharacter)
			{
			case(1):
				Sleep(5000);
				mouse_leftClick(context, mouseDevice, 962, 250);
				logprint("Click first character", currentTm);
				Sleep(5000);
				break;
			case(2):
				Sleep(5000);
				mouse_leftClick(context, mouseDevice, 962, 350);
				logprint("Click second character", currentTm);
				Sleep(5000);
				break;
			case(3):
				Sleep(5000);
				mouse_leftClick(context, mouseDevice, 962, 520);
				logprint("Click second character", currentTm);
				Sleep(5000);
				break;
			default:
				Sleep(5000);
				mouse_leftClick(context, mouseDevice, 894, 351);
				logprint("Click base character", currentTm);
				Sleep(5000);
				break;
			}
			if (!scan.checkPixel(744, 607, 255, 178, 0, 5))
			{
				interception_send(context, device, (InterceptionStroke*)&f2_down, 1);
				Sleep(250);
				interception_send(context, device, (InterceptionStroke*)&f2_up, 1);
			}
		}
		




		Sleep(10000);
		mouse_leftClick(context, mouseDevice, 626, 620);
		logprint("Click choose button", currentTm);
		Sleep(30000);
		//Spawn place choosing
		mouse_leftClick(context, mouseDevice, 714, 635);
		logprint("Click last place button", currentTm);
		Sleep(30000);
		waiting = 0;
		while (!scan.checkPixel(694, 442, 255, 255, 255) && !scan.checkPixel(760, 442, 255, 182, 0) && waiting < 180)
		{
			waiting++;
			Sleep(1000);
			logprint("Waiting for button", currentTm);
		}
		Sleep(1000);
		mouse_leftClick(context, mouseDevice, 646, 450);
		Sleep(1000);
		mouse_leftClick(context, mouseDevice, 646, 450);
		logprint("Click choose button", currentTm);
		Sleep(20000);
		status = "Paying for all";
		//Paying for phone
		string apps_command = "scripts\\get_appartments.py " + login + " Naguibs1337228";
		string apps = exec(apps_command.c_str());
		if (!apps.empty() && stoi(filterDigits(apps)) == 1)
		{
			logprint("Waiting for phone", currentTm);
			int waitingphone = 0;
			while (!scan.checkPixel(1182, 660, 255, 255, 255) && !scan.checkPixel(1072, 144, 255, 255, 255))
			{
				InterceptionKeyStroke  phone;
				phone.code = 0x48;
				phone.state = INTERCEPTION_KEY_DOWN | INTERCEPTION_KEY_E0;
				phone.information = 0;
				interception_send(context, device, (InterceptionStroke*)&phone, 1);
				Sleep(350);
				phone.code = 0x48;
				phone.state = INTERCEPTION_KEY_UP | INTERCEPTION_KEY_E0;
				phone.information = 0;
				interception_send(context, device, (InterceptionStroke*)&phone, 1);
				logprint("Phone opened", currentTm);
				Sleep(5000);
				waitingphone++;
				if (waitingphone >= 20)
				{
					forceRelogin = true;
					break;
				}
			}
			Sleep(2000);
			waitingphone = 0;
			while ((!scan.checkPixel(986, 173, 0, 0, 0) || !scan.checkPixel(1013, 292, 52, 199, 89)) && waitingphone <= 20)//Waiting bank
			{
				logprint("Click Bank", currentTm);
				mouse_leftClick(context, mouseDevice, 1211, 161);//Bank
				Sleep(15000);
				waitingphone++;
			}
			waitingphone = 0;
			while ((!scan.checkPixel(1021, 218, 28, 28, 30) || !scan.checkPixel(1193, 405, 28, 28, 30)) && waitingphone <= 20)//Waiting phone pay screen
			{
				mouse_leftClick(context, mouseDevice, 1200, 380);//phone
				Sleep(15000);
				waitingphone++;
			}
			interception_send(context, device, (InterceptionStroke*)&d5_down, 1);
			Sleep(400);
			interception_send(context, device, (InterceptionStroke*)&d5_up, 1);
			Sleep(1000);
			interception_send(context, device, (InterceptionStroke*)&d0_down, 1);
			Sleep(400);
			interception_send(context, device, (InterceptionStroke*)&d0_up, 1);
			Sleep(1000);
			interception_send(context, device, (InterceptionStroke*)&d0_down, 1);
			Sleep(400);
			interception_send(context, device, (InterceptionStroke*)&d0_up, 1);
			Sleep(5000);
			mouse_leftClick(context, mouseDevice, 1100, 620);//Pay all
			Sleep(15000);
			mouse_leftClick(context, mouseDevice, 1100, 585);//Anyways do
			Sleep(15000);
			mouse_leftClick(context, mouseDevice, 1100, 585);//Make another opperation
			Sleep(15000);
			mouse_leftClick(context, mouseDevice, 1106, 678);//Main menu button
			Sleep(15000);
			mouse_leftClick(context, mouseDevice, 1106, 678);//Main menu button
			Sleep(5000);
			mouse_leftClick(context, mouseDevice, 1106, 678);//Main menu button
			Sleep(5000);
			interception_send(context, device, (InterceptionStroke*)&bksp_down, 1);
			Sleep(400);
			interception_send(context, device, (InterceptionStroke*)&bksp_up, 1);
			Sleep(1000);
			interception_send(context, device, (InterceptionStroke*)&bksp_down, 1);
			Sleep(400);
			interception_send(context, device, (InterceptionStroke*)&bksp_up, 1);
			Sleep(1000);
			interception_send(context, device, (InterceptionStroke*)&bksp_down, 1);
			Sleep(400);
			interception_send(context, device, (InterceptionStroke*)&bksp_up, 1);
			Sleep(1000);

			//Ended paying for phone



			//Paying for flat
			Sleep(15000);
			logprint("Waiting for phone", currentTm);
			waitingphone = 0;
			while (!scan.checkPixel(1182, 660, 255, 255, 255) && !scan.checkPixel(1072, 144, 255, 255, 255))
			{
				InterceptionKeyStroke  phone;
				phone.code = 0x48;
				phone.state = INTERCEPTION_KEY_DOWN | INTERCEPTION_KEY_E0;
				phone.information = 0;
				interception_send(context, device, (InterceptionStroke*)&phone, 1);
				Sleep(350);
				phone.code = 0x48;
				phone.state = INTERCEPTION_KEY_UP | INTERCEPTION_KEY_E0;
				phone.information = 0;
				interception_send(context, device, (InterceptionStroke*)&phone, 1);
				logprint("Phone opened", currentTm);
				Sleep(5000);
				waitingphone++;
				if (waitingphone >= 20)
				{
					forceRelogin = true;
					break;
				}
			}
			Sleep(15000);
			waitingphone = 0;
			while ((!scan.checkPixel(986, 173, 0, 0, 0) || !scan.checkPixel(1013, 292, 52, 199, 89)) && waitingphone <= 20)//Waiting bank
			{
				logprint("Click Bank", currentTm);
				mouse_leftClick(context, mouseDevice, 1211, 161);//Bank
				Sleep(15000);
				waitingphone++;
			}
			mouse_leftClick(context, mouseDevice, 1020, 385);//House
			Sleep(15000);
			mouse_leftClick(context, mouseDevice, 1100, 385);//Flat
			Sleep(8000);
			mouse_leftClick(context, mouseDevice, 1100, 620);//Pay all
			Sleep(15000);
			mouse_leftClick(context, mouseDevice, 1100, 585);//Anyways do
			Sleep(15000);
			mouse_leftClick(context, mouseDevice, 1100, 585);//Make another opperation
			Sleep(15000);
			mouse_leftClick(context, mouseDevice, 1106, 678);//Main menu button
			Sleep(15000);
			mouse_leftClick(context, mouseDevice, 1106, 678);//Main menu button
			Sleep(3000);
			mouse_leftClick(context, mouseDevice, 1106, 678);//Main menu button
			Sleep(3000);
			interception_send(context, device, (InterceptionStroke*)&bksp_down, 1);
			Sleep(400);
			interception_send(context, device, (InterceptionStroke*)&bksp_up, 1);
			Sleep(1000);
			interception_send(context, device, (InterceptionStroke*)&bksp_down, 1);
			Sleep(400);
			interception_send(context, device, (InterceptionStroke*)&bksp_up, 1);
			Sleep(1000);
			interception_send(context, device, (InterceptionStroke*)&bksp_down, 1);
			Sleep(400);
			interception_send(context, device, (InterceptionStroke*)&bksp_up, 1);
			Sleep(1000);
			//Ended paying for flat
		}
		
		
		status = "Baldeet";
	}
	string get_max_lvl_cmd = "scripts\\getmaxlvl.py " + PCName;
	string max_lvl = exec(get_max_lvl_cmd.c_str());
	
	// Парсим max_lvl из таблицы (колонка T), если пусто - используем значение по умолчанию 5
	if (!max_lvl.empty() && !filterDigits(max_lvl).empty())
	{
		max_lvl_int = stoi(filterDigits(max_lvl));
		logprint("Max level from table: " + to_string(max_lvl_int), currentTm);
	}
	else
	{
		max_lvl_int = 5; // Значение по умолчанию если таблица не вернула данные
		logprint("Max level not found in table, using default: 5", currentTm);
	}
	
	// ═══════════════════════════════════════════════════════════════
	// ВЫВОД НАСТРОЕК AFK В КОНСОЛЬ
	// ═══════════════════════════════════════════════════════════════
	printSection("AFK SETTINGS");
	printStatus("Target Level", to_string(max_lvl_int) + " (from " + ((!max_lvl.empty() && !filterDigits(max_lvl).empty()) ? "table" : "default") + ")", ConsoleColors::CYAN);
	printInfo("AFK will be ENABLED until level " + to_string(max_lvl_int));
	printInfo("After reaching target - AFK will be DISABLED");
	printSectionEnd();
	
	string command = "scripts\\getlvl.py " + login + " Naguibs1337228";
	string lvl = exec(command.c_str());
	if (!lvl.empty() && !filterDigits(lvl).empty())
	{
		int current_lvl = stoi(filterDigits(lvl));
		current_lvl_global = current_lvl; // Сохраняем в глобальную переменную
		bot.getApi().sendMessage(517005065, "Lvl is " + lvl, false, 0, keyboardWithLayout);
		
		// Вывод информации об уровне
		printSection("LEVEL STATUS");
		printStatus("Current Level", to_string(current_lvl), ConsoleColors::YELLOW);
		printStatus("Max Level", to_string(max_lvl_int), ConsoleColors::CYAN);
		
		// Логика рулетки: при lvl >= 3 ВКЛ, при lvl < 3 ВЫКЛ
		if (current_lvl >= 3)
		{
			ruletka = true;
			ruletka_user_disabled = false; // Сбрасываем флаг т.к. уровень достигнут
			printStatusBool("Ruletka", true);
			printOK("Level >= 3: Ruletka ENABLED");
			logprint("Lvl >= 3, ruletka enabled", currentTm);
		}
		else
		{
			ruletka = false;
			printStatusBool("Ruletka", false);
			printWarn("Level < 3: Ruletka DISABLED");
			logprint("Lvl < 3, ruletka disabled", currentTm);
		}
		
		// ═══════════════════════════════════════════════════════════════
		// ЛОГИКА AFK: ПРИНУДИТЕЛЬНО ВКЛ если lvl < max_lvl
		// ═══════════════════════════════════════════════════════════════
		if (current_lvl >= max_lvl_int)
		{
			// Достигли целевого уровня - можно выключить AFK
			afk = false;
			printStatusBool("AFK Mode", false);
			printOK("TARGET REACHED! Level " + to_string(current_lvl) + " >= " + to_string(max_lvl_int));
			printInfo("AFK DISABLED - target level reached");
			logprint("Lvl >= max_lvl (" + to_string(max_lvl_int) + "), afk DISABLED", currentTm);
			bot.getApi().sendMessage(517005065, "TARGET REACHED!\nLevel: " + to_string(current_lvl) + " / " + to_string(max_lvl_int) + "\nAFK: DISABLED", false, 0, keyboardWithLayout);
		}
		else
		{
			// Ещё не достигли - AFK ОБЯЗАТЕЛЬНО ВКЛЮЧЁН
			afk = true;
			printStatusBool("AFK Mode", true);
			printWarn("AFK FORCED ON until level " + to_string(max_lvl_int));
			printInfo("Progress: " + to_string(current_lvl) + " / " + to_string(max_lvl_int) + " (" + to_string((current_lvl * 100) / max_lvl_int) + "%)");
			logprint("Lvl < max_lvl (" + to_string(max_lvl_int) + "), afk FORCED ON", currentTm);
		}
		printSectionEnd();
	}
	else
	{
		// При ошибке определения уровня - НЕ трогаем ruletka (она по умолчанию true)
		// Большинство аккаунтов lvl 3+, поэтому рулетка должна работать
		printError("Level detection failed: " + lvl);
		printWarn("Ruletka kept ENABLED (default behavior)");
		printInfo("API might be temporarily unavailable");
		bot.getApi().sendMessage(517005065, "Lvl wasn't detected(" + lvl + ")\nRuletka: ENABLED (default)\nAPI may be unavailable", false, 0, keyboardWithLayout);
	}
	logprint("Restart end", currentTm);

	HWND Gta5Window(FindWindowA(NULL, "RAGE Multiplayer"));
	
	// Статус готовности к работе
	printSection("BOT STATUS");
	printOK("Bot is now ACTIVE");
	printInfo("Monitoring game...");
	printSectionEnd();
	cout << "\n";
	
	while (true) {

		//Character choose window
		if (scan.checkPixel(149,122,73,87,121,5) && scan.checkPixel(159,99,73,87,121,5))
		{
			status = "Choosing character";
			while (!scan.checkPixel(744, 607, 255, 178, 0, 5))
			{
				switch (ActiveCharacter)
				{
				case(1):
					Sleep(5000);
					mouse_leftClick(context, mouseDevice, 962, 250);
					logprint("Click first character", currentTm);
					Sleep(5000);
					break;
				case(2):
					Sleep(5000);
					mouse_leftClick(context, mouseDevice, 962, 350);
					logprint("Click second character", currentTm);
					Sleep(5000);
					break;
				case(3):
					Sleep(5000);
					mouse_leftClick(context, mouseDevice, 962, 520);
					logprint("Click second character", currentTm);
					Sleep(5000);
					break;
				default:
					Sleep(5000);
					mouse_leftClick(context, mouseDevice, 894, 351);
					logprint("Click base character", currentTm);
					Sleep(5000);
					break;
				}
				if (!scan.checkPixel(744, 607, 255, 178, 0, 5))
				{
					interception_send(context, device, (InterceptionStroke*)&f2_down, 1);
					Sleep(250);
					interception_send(context, device, (InterceptionStroke*)&f2_up, 1);
				}
			}





			Sleep(10000);
			mouse_leftClick(context, mouseDevice, 626, 620);
			logprint("Click choose button", currentTm);
			Sleep(30000);
			//Spawn place choosing
			mouse_leftClick(context, mouseDevice, 714, 635);
			logprint("Click last place button", currentTm);
			Sleep(40000);
			int waiting = 0;
			while (!scan.checkPixel(694, 442, 255, 255, 255) && !scan.checkPixel(760, 442, 255, 182, 0) && waiting < 180)
			{
				waiting++;
				Sleep(1000);
				logprint("Waiting for button", currentTm);
			}
			Sleep(1000);
			mouse_leftClick(context, mouseDevice, 646, 450);
			Sleep(1000);
			mouse_leftClick(context, mouseDevice, 646, 450);
			logprint("Click choose button", currentTm);
			Sleep(20000);
			string command = "scripts\\getlvl.py " + login + " Naguibs1337228";
			string lvl = exec(command.c_str());
			if (!lvl.empty() && !filterDigits(lvl).empty())
			{
				int current_lvl = stoi(filterDigits(lvl));
				current_lvl_global = current_lvl; // Сохраняем в глобальную переменную
				bot.getApi().sendMessage(517005065, "Lvl is " + lvl, false, 0, keyboardWithLayout);
				
				// Логика рулетки: при lvl >= 3 ВКЛ, при lvl < 3 ВЫКЛ
				if (current_lvl >= 3)
				{
					ruletka = true;
					ruletka_user_disabled = false; // Сбрасываем флаг т.к. уровень достигнут
					logprint("Lvl >= 3, ruletka enabled", currentTm);
				}
				else
				{
					ruletka = false;
					logprint("Lvl < 3, ruletka disabled", currentTm);
				}
				
				// ═══════════════════════════════════════════════════════════════
				// ЛОГИКА AFK: ПРИНУДИТЕЛЬНО ВКЛ если lvl < max_lvl
				// ═══════════════════════════════════════════════════════════════
				if (current_lvl >= max_lvl_int)
				{
					// Достигли цели - AFK можно выключить
					afk = false;
					logprint("TARGET REACHED! Lvl " + to_string(current_lvl) + " >= max_lvl (" + to_string(max_lvl_int) + "), afk DISABLED", currentTm);
					bot.getApi().sendMessage(517005065, "TARGET REACHED!\nLevel: " + to_string(current_lvl) + " / " + to_string(max_lvl_int) + "\nAFK: DISABLED", false, 0, keyboardWithLayout);
				}
				else
				{
					// Ещё не достигли - AFK ОБЯЗАТЕЛЬНО ВКЛЮЧЁН
					afk = true;
					logprint("Progress: Lvl " + to_string(current_lvl) + " < max_lvl (" + to_string(max_lvl_int) + "), afk FORCED ON", currentTm);
					bot.getApi().sendMessage(517005065, "Progress: " + to_string(current_lvl) + " / " + to_string(max_lvl_int) + " lvl\nAFK: FORCED ON", false, 0, keyboardWithLayout);
				}
			}
			else
			{
				// При ошибке определения уровня - НЕ трогаем ruletka (она по умолчанию true)
				// Большинство аккаунтов lvl 3+, поэтому рулетка должна работать
				logprint("Level detection failed, ruletka kept enabled (default)", currentTm);
				bot.getApi().sendMessage(517005065, "Lvl wasn't detected(" + lvl + ")\nRuletka: ENABLED (default)", false, 0, keyboardWithLayout);
			}
			status = "Baldeet";
		}

		//Check GTA is running
		if (!isRunningP(L"GTA5.exe") && !checkIP(ipList))
		{
			logprint("Gta crushed, updating bot...", currentTm);
			updating = true;
		}
		//Check for disconnection
		if ((currentTm->tm_hour == targetTimeMin.tm_hour &&
			(currentTm->tm_min >= targetTimeMin.tm_min &&
				currentTm->tm_min <= targetTimeMax.tm_min && relogin))
			||
			(scan.checkPixel(121, 593, 255, 255, 255) &&
				relogin && scan.checkPixel(153, 639, 35, 31, 32) &&
				scan.checkPixel(1279, 719, 24, 24, 24) &&
				scan.checkPixel(1175, 665, 24, 65, 101) && !(currentTm->tm_hour == targetTimeMin.tm_hour &&
					!(currentTm->tm_min >= targetTimeMin.tm_min &&
						currentTm->tm_min <= targetTimeMax.tm_min && relogin))) ||
			forceRelogin)
		{

			if (!donotrestart)
			{
				curl_global_cleanup();
				system("shutdown /r /t 5");
			}
			logprint("Disconnected. Restarting", currentTm);
		}
		/*if ((!scan.checkPixel(1198, 102, 255, 255, 255) || !scan.checkPixel(1198, 102 + 60, 255, 255, 255)) &&
			(scan.checkPixel(1112, 93, 255, 255, 255) ||
				scan.checkPixel(1164, 96, 255, 255, 255) ||
				scan.checkPixel(1112, 93 + 60, 255, 255, 255) ||
				scan.checkPixel(1164, 96 + 60, 255, 255, 255))
			&& ruletka)*/
		
		// ═══════════════════════════════════════════════════════════════════════════
		// PRESENT TAKING - Подарки работают ТОЛЬКО вместе с рулеткой!
		// Если рулетка выключена (lvl < 3), подарки тоже не забираем
		// ═══════════════════════════════════════════════════════════════════════════
		// УБРАНО: Отдельный блок подарков когда рулетка выключена
		// Подарки теперь забираются ТОЛЬКО в блоке рулетки (когда ruletka=true и lvl >= 3)
		// ═══════════════════════════════════════════════════════════════════════════
		
		//Ruletka
		if (checkSpin() && ruletka)
		{
			status = "Krutit";
			//Checking fpslimit value
			HKEY hKey;
			// Путь к ключу реестра
			LPCSTR subKey = "Software\\BANDISOFT\\BANDICAM\\OPTION";
			// Имя параметра реестра
			LPCSTR valueName = "bFpsLimit";
			DWORD value;
			DWORD valueType = REG_DWORD;
			DWORD dataSize = sizeof(value);

			// Открываем ключ реестра
			if (RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
				// Читаем значение параметра реестра
				if (RegQueryValueEx(hKey, valueName, NULL, &valueType, (LPBYTE)&value, &dataSize) == ERROR_SUCCESS) {
					// Проверяем, равняется ли значение 1
					if (value == 1)
					{
						interception_send(context, device, (InterceptionStroke*)&slash_down, 1);
						Sleep(400);
						interception_send(context, device, (InterceptionStroke*)&slash_up, 1);
						Sleep(5000);
						logprint("FPS limit is turned off", currentTm);
					}
					else
					{
						logprint("FPS limit is OK", currentTm);
					}

				}
				else
				{
					logprint("Unable to read bFpsLimit", currentTm);
				}

			}
			else {
				logprint("Unable to open regedit", currentTm);
			}

			Sleep(5000);

			//PRESENT TAKING
			if (present_activity)
			{
				logprint("Checking present", currentTm);
				if (true) //!scan.checkPixel(1028, 100, 253, 117, 7)
				{
					interception_send(context, device, (InterceptionStroke*)&f10_down, 1);
					Sleep(300);
					interception_send(context, device, (InterceptionStroke*)&f10_up, 1);
					Sleep(10000);
					mouse_leftClick(context, mouseDevice, 458, 94);
					Sleep(10000);
					
					// 1. Попытка забрать (если уже куплен и выполнен)
					if (scan.checkPixel(720, 500, 193, 26, 33))
					{
						mouse_leftClick(context, mouseDevice, 720, 500);
						Sleep(10000);
						mouse_leftClick(context, mouseDevice, 720, 500);
						Sleep(3000);
						scan.makeScreenshot();
						for (string id : tgListLoging)
						{
							int wait = 0;
							while (wait < 5)
							{
								try
								{
									bot.getApi().sendPhoto(id, InputFile::fromFile(photoFilePath, photoMimeType), "Present was taken", 0, nullptr, "", true);
									break;
								}
								catch (const std::exception& e)
								{
									logprint("\nNO CONNECTION WITH TELEGRAM.SCREENSHOT WASN'T SENDED\nError: " + (string)e.what(), currentTm);
								}
								wait++;
								Sleep(1000);
							}
						}
					}
					else 
					{
						// 2. BLIND CLICK (Попытка купить БП)
						// Если кнопки "Забрать" нет - возможно БП не куплен.
						// Пробуем кликнуть в центр (400, 338) где обычно кнопка покупки.
						// Если денег нет - ничего не произойдет. Если куплен - тоже.
						logprint("Present button not found. Trying BLIND BUY (400, 338)...", currentTm);
						mouse_leftClick(context, mouseDevice, 400, 338); 
						Sleep(2000);
						// Подтверждение покупки (на всякий случай, если есть диалог)
						mouse_leftClick(context, mouseDevice, 400, 338);
						Sleep(2000);
					}

					// 3. SAFE EXIT (Гарантированный выход)
					// Вместо клика по крестику (1158, 100), который может не сработать если окно другое,
					// используем F10, так как мы открывали меню через F10.
					// ESC опасен тем, что открывает карту (меню паузы).
					logprint("Exiting present menu (Safe Exit)...", currentTm);
					
					// Закрываем через F10
					interception_send(context, device, (InterceptionStroke*)&f10_down, 1);
					Sleep(300);
					interception_send(context, device, (InterceptionStroke*)&f10_up, 1);
					Sleep(2000);
				}
			}
			

			logprint("Ruletka detected, opening phone...", currentTm);



			//Phone opening
			int waitingphone = 0;
			while (!scan.checkPixel(1182, 660, 255, 255, 255) && !scan.checkPixel(1072, 144, 255, 255, 255))
			{
				InterceptionKeyStroke  phone;
				phone.code = 0x48;
				phone.state = INTERCEPTION_KEY_DOWN | INTERCEPTION_KEY_E0;
				phone.information = 0;
				interception_send(context, device, (InterceptionStroke*)&phone, 1);
				Sleep(350);
				phone.code = 0x48;
				phone.state = INTERCEPTION_KEY_UP | INTERCEPTION_KEY_E0;
				phone.information = 0;
				interception_send(context, device, (InterceptionStroke*)&phone, 1);
				logprint("Phone opened", currentTm);
				Sleep(2000);
				waitingphone++;
				if (waitingphone >= 10)
				{
					break;
				}
			}
			if (waitingphone >= 10)
			{
				logprint("Phone not opened after 10 attempts, skipping ruletka this cycle", currentTm);
				continue;
			}

			//CLick phone
			Sleep(10000);
			while (!scan.checkPixel(390, 347, 255, 255, 255) && !scan.checkPixel(741, 374, 255, 255, 255) && !scan.checkPixel(656, 135, 255, 255, 255))
			{
				mouse_leftClick(context, mouseDevice, 1210, 470, false, 350);
				logprint("Clicked,waiting for casino app.", currentTm);
				Sleep(5000);
			}

			//Click time roulet button
			Sleep(15000);
			waitingphone = 0;
			while (!scan.checkPixel(626, 273, 255, 253, 250) && !scan.checkPixel(624, 710, 255, 255, 255) && waitingphone < 10)
			{
				mouse_leftClick(context, mouseDevice, 390, 347, false, 8000, 3000);
				logprint("Trying to open ruletka page", currentTm);
				Sleep(5000);
				waitingphone++;
			}
			if (waitingphone >= 10)
			{
				logprint("Ruletka page not opened after 10 attempts, skipping ruletka this cycle", currentTm);
				continue;
			}

			//Click spin button
			Sleep(10000);
			mouse_leftClick(context, mouseDevice, 645, 710, false, 1000);
			Sleep(2000);
			mouse_leftClick(context, mouseDevice, 645, 710, false, 1000);
			Sleep(2000);

			//waiting for result
			logprint("Waiting for spin result", currentTm);
			int spinwait = 0;
			while (scan.checkPixel(626, 273, 255, 253, 250, 50))
			{
				Sleep(100);
				spinwait++;
				if (spinwait >= 6000)
				{
					break;
				}
			}
			Sleep(2000);
			scan.makeScreenshot();
			logprint("Result was screenshoted, trying to detect amount", currentTm);
			string outText;
			if (spinwait < 6000)
			{
				//OCR
				tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
				logprint("TessOCR init", currentTm);
				try
				{
					// Initialize tesseract-ocr with English, without specifying tessdata path
					int wait = 0;
					while (wait < 40)
					{
						if (api->Init(NULL, "eng"))
						{
							break;
						}
						else
						{
							Sleep(1000);
							wait++;
							logprint("Failed to init tesseract", currentTm);
						}
					}
					// Open input image with leptonica library
					Pix* image = pixRead("Screenshot.png");
					api->SetImage(image);
					// Restrict recognition to a sub-rectangle of the image
					// SetRectangle(left, top, width, height)
					api->SetRectangle(345, 126, 598, 404);
					// Get OCR result
					outText = api->GetUTF8Text();
					string OCRResult = outText;

					//Regex
					regex cash(R"(\b(\d{5})(5|9|8|S|\$)(!|1|I|i|l|S|\$|9))");
					regex tips(R"(\b(\d{2,3}) [^\s]+)");
					regex dp(R"(\b(\d{2,3}) (DP|OP)\b)");
					smatch match;

					if (regex_search(outText, match, cash)) {
						string digits = match[1]; // Извлекаем цифры
						string dblog = sendToDB(PCName, "7", digits);
						if (dblog.empty())
						{
							dblog = "WASN\'T SENT TO GOOGLE SHEET";
						}
						outText = (string)"Cash dropped\n+ " + digits + (string)" $\n" + dblog + "\nOCR result :\n" + OCRResult;
					}
					// Проверяем соответствие выражению dp
					else if (regex_search(outText, match, dp)) {
						string digits = match[1]; // Извлекаем цифры
						string dblog = sendToDB(PCName, "10", digits);
						if (dblog.empty())
						{
							dblog = "WASN\'T SENT TO GOOGLE SHEET";
						}
						outText = (string)"Donate Points dropped\n+ " + digits + (string)" DP\n" + dblog + "\nOCR result :\n" + OCRResult;
					}
					// Проверяем соответствие выражению tips
					else if (regex_search(outText, match, tips)) {
						string digits = match[1]; // Извлекаем цифры
						if (digits.length() < 3)
						{
							digits = "1" + digits;
						}
						string dblog = sendToDB(PCName, "8", digits);
						if (dblog.empty())
						{
							dblog = "WASN\'T SENT TO GOOGLE SHEET";
						}
						outText = (string)"Tips dropped\n+ " + digits + (string)" tips\n" + dblog + "\nOCR result :\n" + OCRResult;
					}
					else {
						outText = (string)"IDK what is dropped\n OutText: " + outText;
					}

				}
				catch (const std::exception& e)
				{
					logprint("Error during text recognition: " + (string)e.what(), currentTm);
				}
				printf("OCR output:\n%s", outText);
			}
			else
			{
				outText = "SPIN BUG, RESULT UNKNOWN";
			}

			//Sending result to TG
			//ToDo: Make query queue and thred for checking and pushing this queries

			for (string id : tgListLoging)
			{
				int wait = 0;
				while (wait < 5)
				{
					try
					{
						bot.getApi().sendPhoto(id, InputFile::fromFile(photoFilePath, photoMimeType), outText, 0, nullptr, "", true);
						break;
					}
					catch (const std::exception& e)
					{
						logprint("\nNO CONNECTION WITH TELEGRAM.SCREENSHOT WASN'T SENDED\nError: " + (string)e.what(), currentTm);
					}
					wait++;
					Sleep(1000);
				}
			}

			//Exit to main screen
			int wait = 0;
			interception_send(context, device, (InterceptionStroke*)&esc_down, 1);
			Sleep(175);
			interception_send(context, device, (InterceptionStroke*)&esc_up, 1);
			Sleep(5000);
			interception_send(context, device, (InterceptionStroke*)&esc_down, 1);
			Sleep(175);
			interception_send(context, device, (InterceptionStroke*)&esc_up, 1);
			Sleep(5000);
			interception_send(context, device, (InterceptionStroke*)&bksp_down, 1);
			Sleep(175);
			interception_send(context, device, (InterceptionStroke*)&bksp_up, 1);
			Sleep(5000);
			interception_send(context, device, (InterceptionStroke*)&bksp_down, 1);
			Sleep(175);
			interception_send(context, device, (InterceptionStroke*)&bksp_up, 1);
			Sleep(5000);

			// Открываем ключ реестра
			if (RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
				// Читаем значение параметра реестра
				if (RegQueryValueEx(hKey, valueName, NULL, &valueType, (LPBYTE)&value, &dataSize) == ERROR_SUCCESS) {
					// Проверяем, равняется ли значение 1
					if (value == 0)
					{
						interception_send(context, device, (InterceptionStroke*)&slash_down, 1);
						Sleep(400);
						interception_send(context, device, (InterceptionStroke*)&slash_up, 1);
						Sleep(5000);
						logprint("FPS limit is turned on", currentTm);
					}
					else
					{
						logprint("FPS limit is OK", currentTm);
					}

				}
				else
				{
					logprint("Unable to read bFpsLimit", currentTm);
				}

			}
			else {
				logprint("Unable to open regedit", currentTm);
			}

			if (!login.empty())
			{

				string command_user_update = ("main.py " + login + " Naguibs1337228 " + PCName);
				string user_update = exec(command_user_update.c_str());
				logprint("User update.\n" + user_update, currentTm);
				if (user_update.empty())
				{
					bot.getApi().sendMessage(517005065, "No result during user update. Entered command:\n" + command_user_update);
				}
				else
				{
					try
					{
						bot.getApi().sendMessage(517005065, user_update);
					}
					catch (const std::exception e)
					{
						bot.getApi().sendMessage(517005065, "Result is too long. " + (string)e.what());
					}
				}
				string command_profile_update = ("main.py " + login + " Naguibs1337228");
				string profile_update = exec(command_profile_update.c_str());
				logprint("Profile update.\n" + profile_update, currentTm);
				if (user_update.empty())
				{
					bot.getApi().sendMessage(517005065, "No result during profile update. Entered command:\n" + command_profile_update);
				}
				else
				{
					try
					{
						bot.getApi().sendMessage(517005065, profile_update);
					}
					catch (const std::exception e)
					{
						bot.getApi().sendMessage(517005065, "Result is too long. " + (string)e.what());
					}
				}
			}
			else
			{
				bot.getApi().sendMessage(517005065, "Skipped sending to table. Login is empty", false, 0, nullptr, "", true);
			}


			status = "Baldeet";
		}
		
		
		
		//Checking if responding
		if (Gta5Window != NULL && IsHungAppWindow(Gta5Window))
		{
			logprint("GTA window is not responding.", currentTm);
			status = "IS NOT RESPONDING!!!";
		}

		//Disconnected relogin
		if (scan.checkPixel(995, 252, 255, 255, 255) && scan.checkPixel(1180, 251, 255, 255, 255))
		{
			status = "Disconected, trying to relogin";
			logprint("Disconected, trying to relogin",currentTm);
			while (scan.checkPixel(995, 252, 255, 255, 255) && scan.checkPixel(1180, 251, 255, 255, 255) && !scan.checkPixel(188, 107, 73, 87, 121, 5))
			{

				logprint("Ctrl+Alt+Del", currentTm);
				interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
				interception_send(context, device, (InterceptionStroke*)&alt_down, 1);
				interception_send(context, device, (InterceptionStroke*)&del_down, 1);
				Sleep(400);
				interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
				interception_send(context, device, (InterceptionStroke*)&alt_up, 1);
				interception_send(context, device, (InterceptionStroke*)&del_up, 1);
				Sleep(5000);
				interception_send(context, device, (InterceptionStroke*)&esc_down, 1);
				Sleep(220);
				interception_send(context, device, (InterceptionStroke*)&esc_up, 1);
				Sleep(10000);
				cout << " ...";
				HWND Gta5(FindWindowA(NULL, "RAGE Multiplayer"));
				if (Gta5 == NULL)
				{
					logprint("Failed to find window.", currentTm);
				}
				else
				{
					logprint("Window was founded", currentTm);
					PostMessage(Gta5, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
					SetForegroundWindow(Gta5);
					logprint("Making window active", currentTm);
				}
				Sleep(5000);
				mouse_leftClick(context, mouseDevice, 10, 10);
				Sleep(3000);
				logprint("Pressing tab", currentTm);
				Sleep(1000);
				interception_send(context, device, (InterceptionStroke*)&tab_down, 1);
				Sleep(270);
				interception_send(context, device, (InterceptionStroke*)&tab_up, 1);
				Sleep(3000);
				logprint("Typing login: " + login, currentTm);
				//Copying login to clipboard
				addToClipboard(login);

				//Ctrl+A
				interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
				Sleep(32);
				interception_send(context, device, (InterceptionStroke*)&a_down, 1);
				Sleep(112);
				interception_send(context, device, (InterceptionStroke*)&a_up, 1);
				Sleep(45);
				interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
				Sleep(2000);
				//Ctrl+V
				interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
				Sleep(32);
				interception_send(context, device, (InterceptionStroke*)&v_down, 1);
				Sleep(112);
				interception_send(context, device, (InterceptionStroke*)&v_up, 1);
				Sleep(45);
				interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
				logprint("Pressing tab", currentTm);
				Sleep(1000);
				interception_send(context, device, (InterceptionStroke*)&tab_down, 1);
				Sleep(270);
				interception_send(context, device, (InterceptionStroke*)&tab_up, 1);
				Sleep(3000);
				logprint("Typing password: " + password, currentTm);
				//Copying login to clipboard
				addToClipboard(password);

				//Ctrl+A
				interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
				Sleep(32);
				interception_send(context, device, (InterceptionStroke*)&a_down, 1);
				Sleep(112);
				interception_send(context, device, (InterceptionStroke*)&a_up, 1);
				Sleep(45);
				interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
				Sleep(2000);
				//Ctrl+V
				interception_send(context, device, (InterceptionStroke*)&ctrl_down, 1);
				Sleep(32);
				interception_send(context, device, (InterceptionStroke*)&v_down, 1);
				Sleep(112);
				interception_send(context, device, (InterceptionStroke*)&v_up, 1);
				Sleep(45);
				interception_send(context, device, (InterceptionStroke*)&ctrl_up, 1);
				Sleep(10000);
				interception_send(context, device, (InterceptionStroke*)&enter_down, 1);
				Sleep(300);
				interception_send(context, device, (InterceptionStroke*)&enter_up, 1);
				mouse_leftClick(context, mouseDevice, 1032, 513);
				logprint("Click login button", currentTm);
				Sleep(60000);
			}

			switch (ActiveCharacter)
			{
			case(1):
				mouse_leftClick(context, mouseDevice, 962, 250);
				logprint("Click first character", currentTm);
				break;
			case(2):
				mouse_leftClick(context, mouseDevice, 962, 350);
				logprint("Click second character", currentTm);
				break;
			case(3):
				mouse_leftClick(context, mouseDevice, 962, 520);
				logprint("Click second character", currentTm);
				break;
			default:
				mouse_leftClick(context, mouseDevice, 894, 351);
				logprint("Click base character", currentTm);
				break;
			}




			Sleep(10000);
			mouse_leftClick(context, mouseDevice, 626, 620);
			logprint("Click choose button", currentTm);
			Sleep(30000);
			//Spawn place choosing
			mouse_leftClick(context, mouseDevice, 714, 635);
			logprint("Click last place button", currentTm);
			Sleep(40000);
			int waiting = 0;
			while (!scan.checkPixel(694, 442, 255, 255, 255) && !scan.checkPixel(760, 442, 255, 182, 0) && waiting < 180)
			{
				waiting++;
				Sleep(1000);
				logprint("Waiting for button", currentTm);
			}
			Sleep(1000);
			mouse_leftClick(context, mouseDevice, 646, 450);
			Sleep(1000);
			mouse_leftClick(context, mouseDevice, 646, 450);
			logprint("Click choose button", currentTm);
			Sleep(20000);
			status = "Baldeet";
		}

		//AFK
		while (afk) {
			//mouse_leftClick(context, mouseDevice, 100, 100, true);
			interception_send(context, device, (InterceptionStroke*)&s_down, 1);
			Sleep(100);
			interception_send(context, device, (InterceptionStroke*)&s_up, 1);
			Sleep(40000);
			break;
		}
	}
	interception_destroy_context(context);
	scan.end();
	close_single_program(program_instance);
	
	} catch (const std::exception& e) {
		printf("\n\n=== CRITICAL ERROR ===\n");
		printf("Exception: %s\n", e.what());
		printf("Press any key to exit...\n");
		fflush(stdout);
		_getch();
		return 1;
	} catch (...) {
		printf("\n\n=== UNKNOWN CRITICAL ERROR ===\n");
		printf("Press any key to exit...\n");
		fflush(stdout);
		_getch();
		return 1;
	}
	
	return 0;
}
