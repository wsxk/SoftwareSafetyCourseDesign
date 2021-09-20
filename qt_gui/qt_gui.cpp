#include "qt_gui.h"

qt_gui::qt_gui(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.input_file, SIGNAL(clicked()), this, SLOT(get_file()));
    connect(ui.run_file, SIGNAL(clicked()), this, SLOT(run_file()));
    //connect(ui.hook_info, SIGNAL(clicked()), this, SLOT(get_hook_info()));
}

void qt_gui::get_file() {
    QString file_path = QFileDialog::getOpenFileName(this, "input file!", /*"/"*/NULL,"(*.exe)");
    ui.text_show->setText(file_path);
}

void qt_gui::run_file() {
    QString file_path = ui.text_show->text();
    if (file_path.isEmpty()) {
        MessageBoxA(NULL, "no exe file!", "warning!", NULL);
    }
    else {
        QByteArray text = file_path.toUtf8();
        char* data = new char[text.size() + 2];
        data[0] = ' ';
        strcpy(data+1, text.data());
        STARTUPINFOA startupInfo = { 0 };
        PROCESS_INFORMATION  processInformation = { 0 };
        //char moduleFileName[MAX_PATH];
        //GetCurrentDirectoryA(MAX_PATH, moduleFileName);
        //printf("%s", moduleFileName);
        BOOL bSuccess = CreateProcessA("./x64/Debug/injector.exe", data, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInformation);
        if (bSuccess) {
            char buffer[1024];

            sprintf(buffer, "%d", processInformation.dwProcessId);
            ui.process_id->setText(QString(QLatin1String(buffer)));
            
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "0x%p", GetPriorityClass(processInformation.hProcess));
            ui.process_priority->setText(QString(QLatin1String(buffer)));
            
            memset(buffer, 0, sizeof(buffer));
            ui.process_name->setText(QString(QLatin1String("injector.exe")));
            
            Sleep(500);
            memset(buffer, 0, sizeof(buffer));
            HMODULE hMod[100];
            DWORD cbNeeded;
            int moduleNum;
            char moduleName[256];
            if (EnumProcessModulesEx(processInformation.hProcess, hMod, sizeof(hMod), &cbNeeded, LIST_MODULES_64BIT)) {
                moduleNum = cbNeeded / sizeof(HMODULE);
                for (int i = 0; i < moduleNum; i++) {
                    GetModuleBaseNameA(processInformation.hProcess, hMod[i], moduleName, sizeof(moduleName));
                    strcat(buffer, moduleName);
                    strcat(buffer, "\n");
                }
            }
            ui.process_modules->setText(QString(QLatin1String(buffer)));

            //创建线程
            std::thread t(get_hook_message, std::ref(*this));
            t.detach();
        }
        else {
            MessageBox(NULL, L"create process failed!", L"error", NULL);
        }
        //int d = GetLastError();
        //printf("CreateProcess failed (%d).\n", GetLastError());     
    }
}
/*
//创建线程
void qt_gui::get_hook_info() {
    std::thread t(get_hook_message, std::ref(*this));
    t.detach();
}*/
//线程执行的函数
void get_hook_message(qt_gui &w) {
    typedef void(*ptrGet)(char*, int);
    typedef void(*ptrNum)(int&);
    typedef void(*pHeapAllocNum)(int&);
    typedef void(*Reset)();
    HMODULE hMod_new = LoadLibraryA("./x64/Debug/injected_dll.dll");
    if (!hMod_new) {
        MessageBoxA(NULL, "no library", "error", NULL);
        return;
    }
    ptrGet GetSeg = (ptrGet)GetProcAddress(hMod_new, "GetSeg");
    ptrNum GetNum = (ptrNum)GetProcAddress(hMod_new, "GetNum");
    pHeapAllocNum GetAllocNum = (pHeapAllocNum)GetProcAddress(hMod_new, "GetAllocNum");
    Reset ResetCount = (Reset)GetProcAddress(hMod_new, "ResetCount");

    int end = 0;
    int start = 0;
    char show[2048+6]="info:\n";
    ResetCount();
    while (true)
    {
        Sleep(2000);
        char buffer[512];
        int AllocNum = 0;
        GetNum(end);
        if (end < start)
            start = 0;
        if (start == end)
            continue;
        if (end - start > 4)
            end = start + 4;
        memset(show + 6, 0, sizeof(show));
        for (int i = start + 1; i <= end; i++) {
            memset(buffer, 0, sizeof(buffer));
            GetSeg(buffer, i);
            strcat(show, buffer);
        }
        start = end;
        w.ui.info->setText(QString(QLatin1String(show)));
    }
}