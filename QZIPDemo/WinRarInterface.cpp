
#include "WinRarInterface.h"
using namespace Pan;

#include <windows.h>
#include <strsafe.h>

bool WinRarInterface::is_winrar_directory(const wchar_t *path)
{
    // 排除空指针
    if (path == nullptr)
        return false;

    // 不是一个存在的文件夹
    if (!WinRarInterface::is_existing_directory(path))
    {
        return false;
    }

    bool ret(false);            // 返回值
    wchar_t *fullpath(nullptr); // 保存绝对路径
    DWORD len(0);               // 保存 fullpath 字符串的长度

    fullpath = new wchar_t[512];

    if (fullpath == nullptr)
    {
        return false;
    }

    // 转换为绝对路径
    len = ::GetFullPathNameW(path, 512, fullpath, NULL);

    // 函数调用出错
    if (len <= 0){
        // 释放内存
        if (fullpath != nullptr)
        {
            delete[] fullpath;
            fullpath = nullptr;
        }
        return ret;
    }
    // 去除末尾的斜杠
    if (len > 0 && fullpath[len - 1] == '\\')
    {
        // C:\\ ?
        if (len > 1 && fullpath[len - 2] == ':')
        {
        }
        else
        {
            fullpath[--len] = '\0';
        }
    }

    // 取得字符串的末尾位置, 方便在后边拼接上一点东西
    wchar_t *end(&fullpath[len]);

    // 检查Rar.exe是否存在
    Pan::WinRarInterface::_string_copy(end, L"\\Rar.exe");
    if (!WinRarInterface::is_existing_file(fullpath))
    {
        ret = false;
        // 释放内存
        if (fullpath != nullptr)
        {
            delete[] fullpath;
            fullpath = nullptr;
        }

        return ret;
    }

    // 检查WinRar.exe是否存在
    Pan::WinRarInterface::_string_copy(end, L"\\WinRar.exe");
    if (!WinRarInterface::is_existing_file(fullpath))
    {
        ret = false;
        // 释放内存
        if (fullpath != nullptr)
        {
            delete[] fullpath;
            fullpath = nullptr;
        }
        return ret;
    }
    // 全部检测通过
    ret = true;
    // 释放内存
    if (fullpath != nullptr)
    {
        delete[] fullpath;
        fullpath = nullptr;
    }
    return ret;
}

WinRarInterface *Pan::WinRarInterface::autoDetect()
{

    // 当前目录
    WinRarInterface *wi = new WinRarInterface(L"WinRAR");

    if (WinRarInterface::is_winrar_directory(wi->m_winRarDirectory))
    {
        return wi;
    }
    else
    {
        // 表示路径不可用
        wi->m_winRarDirectory[0] = 0;
    }

    // 通过环境变量(ProgramW6432)获取程序目录
    if (wi->m_winRarDirectory[0] == 0)
    {
        DWORD w(0);
        w = ::GetEnvironmentVariableW(
            L"ProgramW6432",
            wi->m_winRarDirectory,
            WinRarInterface::bufferSize);

        if (w > 0) // 取得该变量成功, 说明这应该是64位的操作系统
        {
            wchar_t *end(wi->m_winRarDirectory);
            while (*end)
                end++;
            WinRarInterface::_string_copy(end, L"\\WinRAR");
            if (WinRarInterface::is_winrar_directory(wi->m_winRarDirectory))
            {
                return wi;
            }
            else
            {
                // 表示该路径不可用
                wi->m_winRarDirectory[0] = 0;
            }
        }
    }

    // 通过环境变量(ProgramFiles)获取程序目录
    if (wi->m_winRarDirectory[0] == 0)
    {
        DWORD w(0);
        w = ::GetEnvironmentVariableW(
            L"ProgramFiles",
            wi->m_winRarDirectory,
            WinRarInterface::bufferSize);

        if (w > 0) // 取得该变量成功, 说明这应该是64位的操作系统
        {
            wchar_t *end(wi->m_winRarDirectory);
            while (*end)
                end++;
            WinRarInterface::_string_copy(end, L"\\WinRAR");
            if (WinRarInterface::is_winrar_directory(wi->m_winRarDirectory))
            {
                return wi;
            }
            else
            {
                // 表示该路径不可用
                wi->m_winRarDirectory[0] = 0;
            }
        }
    }

    if (wi->m_winRarDirectory[0] == 0)
    {
        // 自动检测失败
        delete wi;
        wi = nullptr;
    }

    return wi;
}

WinRarInterface *Pan::WinRarInterface::byInstallDirectory(const wchar_t *install_directory)
{
    // 排除空指针
    if (!install_directory)
        return nullptr;

    WinRarInterface *wi = new WinRarInterface(install_directory);

    // 不是一个可用的接口
    if (!WinRarInterface::is_winrar_directory(wi->m_winRarDirectory))
    {
        delete wi;
        wi = nullptr;
    }

    return wi;
}

unsigned int Pan::WinRarInterface::_string_copy(wchar_t *dest, const wchar_t *src)
{
    // 不进行参数检查, 内部使用
    unsigned int i(0);
    while (src[i] != 0)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
    return i;
}

bool WinRarInterface::is_existing_file(const wchar_t *path)
{

    HANDLE hfile(NULL);
    bool existing(false);

    // 直接打开文件，如果能打开，说明文件存在
    hfile = ::CreateFileW //
        (
            path,
            FILE_READ_EA,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL //
        );

    if (hfile == NULL || hfile == INVALID_HANDLE_VALUE)
    {
        // 打开文件失败，检查一下错误代码
        DWORD error = ::GetLastError();

        if (error == ERROR_PATH_NOT_FOUND ||
            error == ERROR_FILE_NOT_FOUND)
        {
            // 确实是因为文件不存在引起的打开失败
            existing = false;
        }
        else
        {
            // 其它原因（如拒绝访问）引起打开文件失败，但是文件还是存在的
            existing = true;
        }
    }
    else
    {
        // 打开文件成功，说明文件存在
        existing = true;
    }

    // 关闭句柄，释放资源
    if (hfile != NULL)
    {
        ::CloseHandle(hfile);
        hfile = NULL;
    }

    return existing;
}

bool WinRarInterface::is_existing_directory(const wchar_t *path)
{
    // 尝试获取文件的属性

    DWORD fa(INVALID_FILE_ATTRIBUTES);
    bool existing(false);

    fa = ::GetFileAttributesW(path);

    if (fa == INVALID_FILE_ATTRIBUTES)
    {
        // 获取文件属性失败，判断错误代码
        DWORD error = ::GetLastError();

        if (error == ERROR_PATH_NOT_FOUND ||
            error == ERROR_FILE_NOT_FOUND)
        {
            existing = false;
        }
        else
        {
            // 其它原因导致的无法获取文件属性，这时无法判断是否是一个存在的目录
            // 但是给一个默认值 false
            existing = false;
        }
    }
    else
    {
        // 获取文件属性成功，进一步判断
        if ((fa & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            // 确实是一个目录
            existing = true;
        }
        else
        {
            // 可能是一个存在的文件
            existing = false;
        }
    }
    return existing;
}

WinRarInterface::WinRarInterface(const wchar_t *winrar_directory_fullpath)
{
    // 内存分配
    this->m_winRarDirectory = new wchar_t[WinRarInterface::bufferSize];
    this->m_winRarDirectory[0] = 0;

    // 转换为绝对路径
    if (winrar_directory_fullpath)
    {
        ::GetFullPathNameW(
            winrar_directory_fullpath,
            WinRarInterface::bufferSize,
            this->m_winRarDirectory,
            nullptr);
    }
}

WinRarInterface::~WinRarInterface()
{
    delete[] this->m_winRarDirectory;
    this->m_winRarDirectory = nullptr;
}

bool WinRarInterface::_rar_toZip(const wchar_t *zipFile, const wchar_t *path, wchar_t *buffer1024_cmdline, const wchar_t *workingDir)
{
    // 这个宏只在这个函数中使用
#define STR_CAPACITY (1024)

    bool sucess = false; // 是否操作成功

    // 决定是否需要分配内存
    bool free_cmdline = buffer1024_cmdline == nullptr ? true : false;
    if (free_cmdline)
        buffer1024_cmdline = new wchar_t[STR_CAPACITY];

    // 生成命令行
    ::StringCchPrintfW(buffer1024_cmdline, STR_CAPACITY,
                       L"\"%ls\\Rar.exe\" a \"%ls\" %ls",
                       this->m_winRarDirectory,
                       zipFile, path);

    STARTUPINFOW sa;
    ::memset(&sa, 0, sizeof(sa));
    sa.cb = sizeof(sa);
    //    sa.dwFlags = STARTF_USESHOWWINDOW;
    //    sa.wShowWindow = SHOW_NORMAL;

    PROCESS_INFORMATION pi;
    ::memset(&pi, 0, sizeof(pi));

    // 创建进程
    if (::CreateProcessW(nullptr,
                         buffer1024_cmdline, // 命令行
                         nullptr,            // 默认
                         nullptr,            // 默认
                         false,              // 不继承
                         CREATE_NO_WINDOW,   // 不显示窗口
                         nullptr,
                         workingDir, // 工作目录
                         &sa,
                         &pi))
    { // 创建进程成功

        // 等待进程结束
        ::WaitForSingleObject(pi.hProcess, INFINITE);

        // 获取程序的返回值
        DWORD exitCode;
        if (::GetExitCodeProcess(pi.hProcess, &exitCode))
        {
            sucess = (exitCode == 0);
        }
        else
            sucess = false;
        // 关闭句柄, 释放资源
        ::CloseHandle(pi.hThread);
        ::CloseHandle(pi.hProcess);
    }
    else
    {
        // 创建进程失败
        sucess = false;
    }

    // 释放内存
    if (free_cmdline)
    {
        delete[] buffer1024_cmdline;
        buffer1024_cmdline = nullptr;
    }

    return sucess;

#undef STR_CAPACITY
}

bool WinRarInterface::winRarUnZip(const wchar_t *zipFile, const wchar_t *directory)
{
    bool sucess = false;

    const unsigned int capacity = 1024;
    wchar_t *cmdline = new wchar_t[capacity];

    // 生成命令行
    ::StringCchPrintfW(cmdline, capacity, L"\"%ls\\WinRAR.exe\" x \"%ls\" \"%ls\"", this->m_winRarDirectory,
                       zipFile, directory);

    // 执行解压
    STARTUPINFOW sa;
    ::memset(&sa, 0, sizeof(sa));
    sa.cb = sizeof(sa);
    sa.dwFlags = STARTF_USESHOWWINDOW;
    sa.wShowWindow = SW_SHOWNORMAL;

    PROCESS_INFORMATION pi;
    ::memset(&pi, 0, sizeof(pi));

    // 创建进程
    if (::CreateProcessW(nullptr,
                         cmdline, // 命令行
                         nullptr, // 默认
                         nullptr, // 默认
                         false,   // 不继承
                         0,
                         nullptr,
                         nullptr,
                         &sa,
                         &pi))
    { // 创建进程成功

        // 等待解压结束
        ::WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode(static_cast<DWORD>(-1));
        if (::GetExitCodeProcess(pi.hProcess, &exitCode))
        {
            sucess = (exitCode == 0);
        }
        else
        {
            sucess = false;
        }

        ::CloseHandle(pi.hThread);
        ::CloseHandle(pi.hProcess);
    }
    else
    {
        // 创建进程失败
        sucess = false;
    }

    delete[] cmdline;
    cmdline = nullptr;

    return sucess;
}

bool WinRarInterface::rarZipDirectory(const wchar_t *zipFile, const wchar_t *directory_path, wchar_t *buffer_findStr, bool (*select)(const wchar_t *, const wchar_t *))
{
    // 参数检查
    if (!zipFile)
        return false;
    if (!directory_path)
        return false;

    // 操作是否成功
    bool success(true);

    // 是否需要在函数内分配和释放内存
    bool free_findStr = !buffer_findStr;
    if (free_findStr)
        buffer_findStr = new wchar_t[1024]; // 决定是否需要分配内存

    { // 转换为绝对路径

        wchar_t *tmp(nullptr);

        tmp = new wchar_t[256]; // 要压缩的文件夹
        ::GetFullPathNameW(directory_path, 256, tmp, nullptr);
        directory_path = tmp;

        tmp = new wchar_t[256]; // zip文件
        ::GetFullPathNameW(zipFile, 256, tmp, nullptr);
        zipFile = tmp;
    }

    // 生成findStr
    {
        unsigned int end =
            WinRarInterface::_string_copy(buffer_findStr, directory_path);

        WinRarInterface::_string_copy(&buffer_findStr[end], L"\\*");
    }

    // 遍历要压缩的文件夹
    WIN32_FIND_DATAW data;
    ::memset(&data, 0, sizeof(data));
    HANDLE find = ::FindFirstFileW(buffer_findStr, &data);
    if (find != nullptr && find != INVALID_HANDLE_VALUE)
    {

        wchar_t *paths = new wchar_t[1024];
        paths[0] = 0;
        unsigned int path_end = 0;
        wchar_t *cmdline = new wchar_t[1024];
        cmdline[0] = 0;

        do
        {
            //不是.或..
            if ((data.cFileName[0] == '.' &&
                 data.cFileName[1] == '\0') ||
                (data.cFileName[0] == '.' &&
                 data.cFileName[1] == '.' &&
                 data.cFileName[2] == '\0'))
                continue;

            if ((!select) || (select && select(directory_path, data.cFileName)))
            {
                paths[path_end] = '"';
                path_end++;
                path_end = WinRarInterface::_string_copy(&paths[path_end], data.cFileName);
                paths[path_end] = '"';
                path_end++;
                paths[path_end] = ' ';
                path_end++;
            }
        } while (::FindNextFileW(find, &data));

        ::FindClose(find);

        // 去掉最后一个空格
        path_end--;
        paths[path_end] = 0;

        success = this->_rar_toZip(zipFile,         // zip文件
                                   paths,           // 几个路径(文件名)
                                   cmdline,         // 命令行缓冲区
                                   directory_path); // 工作目录

        // 释放内存
        delete[] paths;
        paths = nullptr;
        delete[] cmdline;
        cmdline = nullptr;
    }
    else
        success = false;

    // 释放内存
    if (free_findStr)
    {
        delete[] buffer_findStr;
        buffer_findStr = nullptr;
    }
    delete[] directory_path;
    directory_path = nullptr;
    delete[] zipFile;
    zipFile = nullptr;

    return success;
}

int WinRarInterface::winrar_add //
    (
        const wchar_t *compressed_file,
        const wchar_t *item,
        const wchar_t *current_directory,
        const wchar_t *args //
    )
{
    // 参数检测
    if (compressed_file == nullptr || compressed_file[0] == '\0')
    {
        return -1;
    }
    if (item == nullptr || item[0] == '\0')
    {
        return -1;
    }

    int exit_code = -1;

    const unsigned int capacity = 1024;
    wchar_t *cmdline = new wchar_t[capacity];

    if (cmdline == nullptr)
    { // 内存分配失败
        return -2;
    }

    // 生成命令行
    ::StringCchPrintfW //
        (              //
            cmdline,
            capacity,
            L"\"%ls\\WinRAR.exe\" \"a\" \"%ls\" \"%ls\" ",
            this->m_winRarDirectory,
            compressed_file,
            item //
        );

    // 拼接额外参数
    if (args != nullptr)
    {
        ::StringCchCatW(cmdline, capacity, args);
    }

    STARTUPINFOW sa;
    ::memset(&sa, 0, sizeof(sa));
    sa.cb = sizeof(sa);
    sa.dwFlags = STARTF_USESHOWWINDOW;
    sa.wShowWindow = SW_SHOWNORMAL;

    PROCESS_INFORMATION pi;
    ::memset(&pi, 0, sizeof(pi));

    // 创建进程
    if (
        ::CreateProcessW //
        (                //
            nullptr,
            cmdline, // 命令行
            nullptr, // 默认
            nullptr, // 默认
            false,   // 不继承
            0,
            nullptr,
            current_directory, // 指定WinRAR的运行目录
            &sa,
            &pi) //
    )
    {
        // 创建进程成功

        // 等待压缩结束
        ::WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode(static_cast<DWORD>(-1));

        // 获取WinRAR的退出码
        if (::GetExitCodeProcess(pi.hProcess, &exitCode))
        {
            exit_code = exitCode;
        }
        else
        {
            // 获取退出码失败
            exit_code = -3;
        }

        ::CloseHandle(pi.hThread);
        pi.hThread = NULL;
        ::CloseHandle(pi.hProcess);
        pi.hProcess = NULL;
    }
    else
    {
        // 创建进程失败
        exit_code = -2;
    }

    // 释放内存
    delete[] cmdline, cmdline = nullptr;

    return exit_code;
}
