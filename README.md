软件安全课程设计

环境：

        qt6.1.2
        vs2019(带有qt插件)

程序功能：
        通过ui界面选择可执行程序并运行它，ui界面会显示出可执行程序调用的一些API的具体信息。

程序共四个部分（需要打开qt_gui的sln）

        Ui界面程序(qt_gui)：让用户选择被调用程序并调用注射器程序，之后ui界面会显示被注射程序的相关信息以及操作信息。

        注射器程序(injector)：对被注射的可执行程序注入dll文件并调用它。

        被注入的Dll文件(injected_dll)：包含了hook操作，对一些api进行了hook，会返回信息给ui界面。

        被注射的可执行程序(test_program)：被感染的目标。

被hook函数的api如下：
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
