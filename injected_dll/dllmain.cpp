// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "framework.h"
#include "detours.h"
#include "stdio.h"
#include "stdarg.h"
#include "windows.h"
#include <iostream>
#include <WinSock2.h>
#include <mutex>

#pragma comment(lib,"detours.lib")
#pragma comment (lib, "ws2_32.lib")

#pragma warning(disable : 4996)

//共享缓冲区
#pragma data_seg("MySeg")
char seg[500][512] = {};
int count = 0;
volatile int HeapAllocNum = 0;
#pragma data_seg()
#pragma comment(linker,"/section:MySeg,RWS")

std::mutex mtx;
char info[256];
HANDLE heap_handle=0;
HANDLE file_handle = 0;
bool is_free = 0;

extern "C" __declspec(dllexport)void ChangeSeg(LPCSTR str) {
    mtx.lock();
    count++;
    if (count >= 500) {
        count=1;
    }
    else
        strcpy_s(seg[count],str);
    mtx.unlock();
}
extern "C" __declspec(dllexport)void GetSeg(char* str, int i) {
    mtx.lock();
    strcpy_s(str, 512, seg[i]);
    mtx.unlock();
}
extern "C" __declspec(dllexport)void GetNum(int& num) {
    mtx.lock();
    num = count;
    mtx.unlock();
}
extern "C" __declspec(dllexport)void GetAllocNum(int& num) {
    mtx.lock();
    num = HeapAllocNum;
    mtx.unlock();
}

extern "C" __declspec(dllexport)void ResetCount() {
    mtx.lock();
    count = 0;
    mtx.unlock();
}

SYSTEMTIME st;
//messagebox
static int (WINAPI* OldMessageBoxW)(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType) = MessageBoxW;
static int (WINAPI* OldMessageBoxA)(_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_ UINT uType) = MessageBoxA;
extern "C" __declspec(dllexport)int WINAPI NewMessageBoxA(_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_ UINT uType) {
    GetLocalTime(&st);
    sprintf(info, "opration: MessageBoxA\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhWnd:%08p\nlpText:%s\nlpCaption:%s\nuType:%x\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, hWnd, lpText, lpCaption, uType);
    ChangeSeg(info);
    /*
    sprintf(info, "hWnd:%08p\n", hWnd);
    ChangeSeg(info);
    sprintf(info, "lpText:%s\n", lpText);
    ChangeSeg(info);
    sprintf(info, "lpCaption:%s\n", lpCaption);
    ChangeSeg(info);
    sprintf(info, "uType:%x\n\n", uType);
    ChangeSeg(info);
    */

    return OldMessageBoxA(NULL, "oh father!", "Hooked", MB_OK);
}
extern "C" __declspec(dllexport)int WINAPI NewMessageBoxW(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType) {
    GetLocalTime(&st);
    sprintf(info, "opration:MessageBoxW\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhWnd:%08p\nlpText:%ls\nlpCaption:%ls\nuType:%x\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, hWnd, lpText, lpCaption, uType);
    ChangeSeg(info);
    /*
    sprintf(info, "time:%d-%d-%d %02d:%02d:%02d:%03d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    ChangeSeg(info);
    sprintf(info, "hWnd:%08p\n", hWnd);
    ChangeSeg(info);
    sprintf(info, "lpText:%ls\n", lpText);
    ChangeSeg(info);
    sprintf(info, "lpCaption:%ls\n", lpCaption);
    ChangeSeg(info);
    sprintf(info, "uType:%x\n\n", uType);
    ChangeSeg(info);
    */

    return OldMessageBoxW(NULL, L"oh father!", L"Hooked", MB_OK);
}

//heap
static HANDLE(WINAPI* OldHeapCreate)(DWORD fIOoptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) = HeapCreate;
extern "C" __declspec(dllexport)HANDLE WINAPI NewHeapCreate(DWORD fIOoptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) {
    GetLocalTime(&st);
    heap_handle = OldHeapCreate(fIOoptions, dwInitialSize, dwMaximumSize);
    sprintf(info, "opration:HeapCreate\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nHeapHandle:%p\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, heap_handle);
    ChangeSeg(info);
    is_free = 0;
    /*
    sprintf(info, "time:%d-%d-%d %02d:%02d:%02d:%03d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    ChangeSeg(info);
    sprintf(info, "fIOoptions:%08x\n", fIOoptions);
    ChangeSeg(info);
    sprintf(info, "dwInitialSize:%x\n", dwInitialSize);
    ChangeSeg(info);
    sprintf(info, "dwMaximumSize:%x\n\n", dwMaximumSize);
    ChangeSeg(info);
    */
    return heap_handle;
    //return OldHeapCreate(fIOoptions, dwInitialSize, dwMaximumSize);
}

static BOOL(WINAPI* OldHeapDestroy)(HANDLE) = HeapDestroy;
extern "C" __declspec(dllexport)BOOL WINAPI NewHeapDestroy(HANDLE hHeap) {
    /*
    if (return_mark) {
        return OldHeapDestroy(hHeap);
    }
    return_mark = 1;
    */
    if(heap_handle==0||hHeap!=heap_handle)
        return OldHeapDestroy(hHeap);
    GetLocalTime(&st);
    sprintf(info, "opration:HeapDestroy\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhHeap:%08p\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, hHeap);
    ChangeSeg(info);
    /*
    sprintf(info, "time:%d-%d-%d %02d:%02d:%02d:%03d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    ChangeSeg(info);
    sprintf(info, "hHeap:%08p\n\n", hHeap);
    ChangeSeg(info);
    */
    return OldHeapDestroy(hHeap);
}

static BOOL(WINAPI* OldHeapFree)(HANDLE hHeap, DWORD dwFlags, _Frees_ptr_opt_ LPVOID lpMem) = HeapFree;
extern "C" __declspec(dllexport) BOOL WINAPI NewHeapFree(HANDLE hHeap, DWORD dwFlags, _Frees_ptr_opt_ LPVOID lpMem) {
    /*
    if (return_mark) {
        return OldHeapFree(hHeap, dwFlags, lpMem);
    }
    return_mark = 1;
    */
    if(heap_handle==0||heap_handle!=hHeap)
        return OldHeapFree(hHeap, dwFlags, lpMem);
    if (is_free) {
        GetLocalTime(&st);
        sprintf(info, "opration:HeapFree\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhHeap:%p\ndouble free detected!\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, hHeap);
        ChangeSeg(info);
    }
    else
    {
        GetLocalTime(&st);
        sprintf(info, "opration:HeapFree\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhHeap:%p\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, hHeap);
        ChangeSeg(info);
    }
    is_free = 1;
    /*
    sprintf(info, "time:%d-%d-%d %02d:%02d:%02d:%03d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    ChangeSeg(info);
    sprintf(info, "hHeap:%08p\n", hHeap);
    ChangeSeg(info);
    sprintf(info, "dwFlags:%08x\n", dwFlags);
    ChangeSeg(info);
    sprintf(info, "lpMem:%08p\n\n", lpMem);
    ChangeSeg(info);
    */
    //return_mark = 0;
    return OldHeapFree(hHeap, dwFlags, lpMem);
}

//file operation
static HANDLE(WINAPI* OldCreateFile)(
    LPCTSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    ) = CreateFile;

extern "C" __declspec(dllexport)HANDLE WINAPI NewCreateFile(
    LPCTSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
)
{
    if (wcsstr(lpFileName, L"FONTS")) {
        return OldCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }
    if (wcsstr(lpFileName, L"Fonts")) {
        return OldCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }
    if (wcsstr(lpFileName, L"Globalization")) {
        return OldCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    }
    file_handle = OldCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    
    if (wcsstr(lpFileName, L"exe") || wcsstr(lpFileName, L"dll") || wcsstr(lpFileName, L"ocx")) {
        GetLocalTime(&st);
        sprintf(info, "opration:CreateFile\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nlpFileName:%ls\nhFile:%p\nattempt to open exe,dll or ocx file\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, lpFileName, file_handle);
        ChangeSeg(info);
        return file_handle;
    }

    GetLocalTime(&st);
    sprintf(info,"opration:CreateFile\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nlpFileName:%ls\nhFile:%p\n\n",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, lpFileName,file_handle);
    ChangeSeg(info);
    
    return file_handle;
}

static BOOL(WINAPI* OldWriteFile)(
    HANDLE       hFile,
    LPCVOID      lpBuffer,
    DWORD        nNumberOfBytesToWrite,
    LPDWORD      lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
    ) = WriteFile;

extern "C" __declspec(dllexport)BOOL WINAPI NewWriteFile(
    HANDLE       hFile,
    LPCVOID      lpBuffer,
    DWORD        nNumberOfBytesToWrite,
    LPDWORD      lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
)
{
    if(file_handle==0||hFile!=file_handle)
        return OldWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
    GetLocalTime(&st);
    sprintf(info, "opration:WriteFile\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhFile:%p\nlpBuffer:%s\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, hFile,lpBuffer);
    ChangeSeg(info);

    return OldWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

static BOOL(WINAPI* OldReadFile)(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    ) = ReadFile;

extern "C" __declspec(dllexport)BOOL WINAPI NewReadFile(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
)
{
    if(file_handle==0||file_handle!=hFile)
        return OldReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    bool tmp = OldReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    GetLocalTime(&st);
    sprintf(info, "opration:ReadFile\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhFile:%p\nlbuffer:%s\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, hFile, lpBuffer);
    ChangeSeg(info);
    return tmp;
}

//register
static LSTATUS(WINAPI* OldRegCreateKeyEx)(
    HKEY                        hKey,
    LPCWSTR                     lpSubKey,
    DWORD                       Reserved,
    LPWSTR                      lpClass,
    DWORD                       dwOptions,
    REGSAM                      samDesired,
    const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY                       phkResult,
    LPDWORD                     lpdwDisposition) = RegCreateKeyEx;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegCreateKeyEx(
    HKEY                        hKey,
    LPCWSTR                     lpSubKey,
    DWORD                       Reserved,
    LPWSTR                      lpClass,
    DWORD                       dwOptions,
    REGSAM                      samDesired,
    const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY                       phkResult,
    LPDWORD                     lpdwDisposition
) {
    size_t val = OldRegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
    
    GetLocalTime(&st);
    sprintf(info, "opration:RegCreateKey\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhKey:%p\nlpSubKey:%ls\n%ls is attempted to create\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, *phkResult,lpSubKey, lpSubKey);
    ChangeSeg(info);
    
    return val;
}

static LSTATUS(WINAPI* OldRegSetValueEx)(
    HKEY       hKey,
    LPCWSTR    lpValueName,
    DWORD      Reserved,
    DWORD      dwType,
    const BYTE* lpData,
    DWORD      cbData
    ) = RegSetValueEx;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegSetValueEx(
    HKEY       hKey,
    LPCWSTR    lpValueName,
    DWORD      Reserved,
    DWORD      dwType,
    const BYTE * lpData,
    DWORD      cbData)
{
    GetLocalTime(&st);
    sprintf(info, "opration:RegSetValueEx\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhKey:%p\nlpValueName:%ls\nlpData:%ls\nIt changed the register\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, hKey, lpValueName, lpData);
    ChangeSeg(info);
    return OldRegSetValueEx(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

static LSTATUS(WINAPI* OldRegCloseKey)(HKEY hKey) = RegCloseKey;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegCloseKey(HKEY hKey)
{
    GetLocalTime(&st);
    sprintf(info, "opration:RegCloseKey\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhKey:%p\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, hKey);
    ChangeSeg(info);
    return OldRegCloseKey(hKey);
}

static LSTATUS(WINAPI* OldRegOpenKeyEx)(
    HKEY    hKey,
    LPCWSTR lpSubKey,
    DWORD   ulOptions,
    REGSAM  samDesired,
    PHKEY   phkResult
    ) = RegOpenKeyEx;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegOpenKeyEx(
    HKEY    hKey,
    LPCWSTR lpSubKey,
    DWORD   ulOptions,
    REGSAM  samDesired,
    PHKEY   phkResult)
{
    size_t val = OldRegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    GetLocalTime(&st);
    if (wcsstr(lpSubKey, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")) {
        sprintf(info, "opration:RegOpenKey\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhKey:%p\nlpSubKey:%ls\nattempt to boot automatically while opening computer\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, *phkResult, lpSubKey);
        return val;
    }
    sprintf(info, "opration:RegOpenKey\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhKey:%p\nlpSubKey:%ls\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, *phkResult, lpSubKey);
    ChangeSeg(info);
    return val;
}

static LSTATUS(WINAPI* OldRegDeleteValue)(
    HKEY    hKey,
    LPCWSTR lpValueName
    ) = RegDeleteValue;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegDeleteValue(
    HKEY    hKey,
    LPCWSTR lpValueName)
{
    GetLocalTime(&st);
    sprintf(info, "opration:RegDeleteValue\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nhKey:%p\nlpValueName:%ls\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,hKey,lpValueName);
    ChangeSeg(info);
    return OldRegDeleteValue(hKey, lpValueName);
}

//socket
static SOCKET(WINAPI* Oldsocket)(
    int af,
    int type,
    int protocol
    ) = socket;
extern "C" __declspec(dllexport)SOCKET WINAPI Newsocket(int af, int type, int protocol) {

    GetLocalTime(&st);
    sprintf(info, "opration:socket\ntime:%d-%d-%d %02d:%02d:%02d:%03d\naf:%x\ntype:%x\nprotocol:%x\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,af,type,protocol);
    ChangeSeg(info);
    return Oldsocket(af, type, protocol);
}

static int (WINAPI* Oldbind)(
    SOCKET         s,
    const sockaddr* name,
    int            namelen
    ) = bind;
extern "C" __declspec(dllexport)int WINAPI Newbind(SOCKET s, const sockaddr * name, int namelen) {
    sockaddr_in* sock =(sockaddr_in*) name;
    GetLocalTime(&st);
    sprintf(info, "opration:bind\ntime:%d-%d-%d %02d:%02d:%02d:%03d\naddr:%s\nport:%d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, inet_ntoa(sock->sin_addr), ntohs(sock->sin_port));
    ChangeSeg(info);
    return Oldbind(s, name, namelen);
}

static int (WINAPI* Oldsend)(
    SOCKET     s,
    const char* buf,
    int        len,
    int        flags
    ) = send;
extern "C" __declspec(dllexport)int WINAPI Newsend(SOCKET s, const char* buf, int len, int flags) {
    GetLocalTime(&st);
    sprintf(info, "opration:send\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nmessage:%s\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,buf);
    ChangeSeg(info);
    return Oldsend(s, buf, len, flags);
}

static int (WINAPI* Oldconnect)(
    SOCKET         s,
    const sockaddr* name,
    int            namelen
    ) = connect;
extern "C" __declspec(dllexport)int WINAPI Newconnect(SOCKET s, const sockaddr * name, int namelen) {
    sockaddr_in* sock = (sockaddr_in*)name;
    GetLocalTime(&st);
    sprintf(info, "opration:connect\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nserverIP:%s\nport:%d\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, inet_ntoa(sock->sin_addr), ntohs(sock->sin_port));
    ChangeSeg(info);
    return Oldconnect(s, name, namelen);
}

static int (WINAPI* Oldrecv)(
    SOCKET     s,
    char* buf,
    int        len,
    int        flags
    ) = recv;
extern "C" __declspec(dllexport)int WINAPI Newrecv(SOCKET s, char* buf, int len, int flags) {
    GetLocalTime(&st);
    int recv = Oldrecv(s, buf, len, flags);
    sprintf(info, "opration:recv\ntime:%d-%d-%d %02d:%02d:%02d:%03d\nrecv_messages:%s\n\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,buf);
    ChangeSeg(info);
    return recv;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);//实现禁用指定的DLL的DLL_THREAD_ATTACH和DLL_THREAD_DETACH通知，减小某些程序的工作集大小。
        DetourTransactionBegin();//开始一次截获或者解除截获过程
        DetourUpdateThread(GetCurrentThread());//列入一个在DetourTransaction过程中要进行update的线程
        
        DetourAttach(&(PVOID&)OldMessageBoxW, NewMessageBoxW);
        DetourAttach(&(PVOID&)OldMessageBoxA, NewMessageBoxA);
        
        DetourAttach(&(PVOID&)OldHeapCreate, NewHeapCreate);
        DetourAttach(&(PVOID&)OldHeapFree, NewHeapFree);
        DetourAttach(&(PVOID&)OldHeapDestroy, NewHeapDestroy);
        
        DetourAttach(&(PVOID&)OldCreateFile, NewCreateFile);
        DetourAttach(&(PVOID&)OldWriteFile, NewWriteFile);
        DetourAttach(&(PVOID&)OldReadFile, NewReadFile);
        
        DetourAttach(&(PVOID&)OldRegCreateKeyEx, NewRegCreateKeyEx);
        DetourAttach(&(PVOID&)OldRegSetValueEx, NewRegSetValueEx);
        DetourAttach(&(PVOID&)OldRegDeleteValue, NewRegDeleteValue);
        DetourAttach(&(PVOID&)OldRegCloseKey, NewRegCloseKey);
        DetourAttach(&(PVOID&)OldRegOpenKeyEx, NewRegOpenKeyEx);
        
        DetourAttach(&(PVOID&)Oldsocket, Newsocket);
        DetourAttach(&(PVOID&)Oldbind, Newbind);
        DetourAttach(&(PVOID&)Oldsend, Newsend);
        DetourAttach(&(PVOID&)Oldconnect, Newconnect);
        DetourAttach(&(PVOID&)Oldrecv, Newrecv);
        
        DetourTransactionCommit();//实际拦截操作
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
       
        DetourDetach(&(PVOID&)OldMessageBoxW, NewMessageBoxW);
        DetourDetach(&(PVOID&)OldMessageBoxA, NewMessageBoxA);
        
        DetourDetach(&(PVOID&)OldHeapCreate, NewHeapCreate);
        DetourDetach(&(PVOID&)OldHeapFree, NewHeapFree);
        DetourDetach(&(PVOID&)OldHeapDestroy, NewHeapDestroy);
        
        DetourDetach(&(PVOID&)OldCreateFile, NewCreateFile);
        DetourDetach(&(PVOID&)OldWriteFile, NewWriteFile);
        DetourDetach(&(PVOID&)OldReadFile, NewReadFile);
        
        DetourDetach(&(PVOID&)OldRegCreateKeyEx, NewRegCreateKeyEx);
        DetourDetach(&(PVOID&)OldRegSetValueEx, NewRegSetValueEx);
        DetourDetach(&(PVOID&)OldRegDeleteValue, NewRegDeleteValue);
        DetourDetach(&(PVOID&)OldRegCloseKey, NewRegCloseKey);
        DetourDetach(&(PVOID&)OldRegOpenKeyEx, NewRegOpenKeyEx);
        
        DetourDetach(&(PVOID&)Oldsocket, Newsocket);
        DetourDetach(&(PVOID&)Oldbind, Newbind);
        DetourDetach(&(PVOID&)Oldsend, Newsend);
        DetourDetach(&(PVOID&)Oldconnect, Newconnect);
        DetourDetach(&(PVOID&)Oldrecv, Newrecv);
        
        DetourTransactionCommit();
        break;
    }
    return TRUE;
}

