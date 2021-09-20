// TestConsole.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <iostream>
#include <cstdio>
#include <Windows.h>
#include <detours.h>

#pragma comment(lib, "detours.lib")

int main(int argc, char* argv[])
{
	std::cout << "Hello World!\n"; //输出
	if (argc != 2) {
		MessageBox(NULL, L"too many or less args", NULL, NULL);
		return 0;
	}
	STARTUPINFO si;		//STARTUPINFO用于指定新进程的主窗口特性的一个结构体
	PROCESS_INFORMATION pi;	//PROCESS_INFORMATION 结构返回有关新进程及其主线程的信息
	ZeroMemory(&si, sizeof(STARTUPINFO)); //ZeroMemory用0填充一个区域的内存
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	//包含STARTUPINFO结构中的字节数.如果Microsoft将来扩展该结构
	//,它可用作版本控制手段，
	//应用程序必须将cb初始化为sizeof(STARTUPINFO)。
	//char moduleFileName[MAX_PATH];
	//GetCurrentDirectoryA(MAX_PATH, moduleFileName);
	//printf("%s", moduleFileName);
	WCHAR DirPath[MAX_PATH + 1];	//WCHAR拓展的字符类型 MAX_PATH为260
	wcscpy_s(DirPath, MAX_PATH, L".\\x64\\Debug"); //Dll的文件夹
			//wcscpy_s用于拷贝宽字符类型字符串（目的，目的大小，源指针）
	char DLLPath[MAX_PATH + 1] = "injected_dll.dll";//dll的地址
	WCHAR EXE[MAX_PATH + 1] = { 0 };
	//wcscpy_s(EXE, MAX_PATH, L".\\x64\\Debug\\test_program.exe
	wchar_t fileName[256] = L"";
	MultiByteToWideChar(CP_ACP, 0, argv[1], strlen(argv[1]), fileName, sizeof(fileName));
	//MessageBox(NULL, fileName, NULL, 0);
	wcscpy_s(EXE, MAX_PATH, fileName);
	//  C:\\Users\\fine\\Desktop\\softwaresafty\\InjectDll\\Debug
//DetourCreateProcessWithDllEx可以用来创建一个进程加载我们注入的dll

/*
	wcscpy_s(DirPath, MAX_PATH, L"C:\\softwareSecurity\\TestInitialNullForStudent\\InjectDll\\Debug"); //Dll的文件夹
	char DLLPath[MAX_PATH + 1] = "C:\\softwareSecurity\\TestInitialNullForStudent\\InjectDll\\Debug\\InnjectDll.dll";//Dll的地址 x64
	WCHAR EXE[MAX_PATH + 1] = { 0 };
	//wcscpy_s(EXE, MAX_PATH, L"C:\\Windows\\System32\\notepad.exe");//需要注入程序的地址System32
	//wcscpy_s(EXE, MAX_PATH, L"C:\\softwareSecurity\\APP\\Debug\\APP.exe");
	//wcscpy_s(EXE, MAX_PATH, L"C:\\softwareSecurity\\APP\\Debug\\FindFilesApp.exe");
	wcscpy_s(EXE, MAX_PATH,     L"C:\\softwareSecurity\\TestInitialNullForStudent\\InjectDll\\Debug\\TestApp.exe");//需要注入程序的地址System32
	*/

	if (DetourCreateProcessWithDllEx(EXE, NULL, NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, NULL, DirPath, &si, &pi, DLLPath, NULL))
	{
		//MessageBox& (NULL，”INJECT”，”INJECT”，NULL);
		ResumeThread(pi.hThread);//使线程的挂起时间计数减一
		WaitForSingleObject(pi.hProcess, INFINITE);
		//如果dwMilliseconds为INFINITE，对象被触发信号后，函数才会返回。
		//hHandle[in]对象句柄。可以指定一系列的对象，
	}
	else
	{
		char error[100];
		sprintf_s(error, "%d", GetLastError()); //MessageBoxA(NULL， error，NULL，NULL);
				//GetLastError调用线程最近的错误代码值
	}
	return 0;

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
