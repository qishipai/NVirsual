#include <cstdarg>
#include <cstdio>
#include "Utils.hxx"
using namespace NVi;

#if defined(_WIN32) || defined(_WIN64) /* 使用Windows API设置字体颜色 */

#include <Windows.h>

void NVi::error(const char *prefix, const char *str, ...)
{
    va_list args;
    static HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
    SetConsoleTextAttribute(h, 0x0F); fprintf(stderr, "[%s] ", prefix);
    SetConsoleTextAttribute(h, 0x0C); fprintf(stderr, "错误: ");
    SetConsoleTextAttribute(h, 0x07);
    va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

void NVi::warn(const char *prefix, const char *str, ...)
{
    va_list args;
    static HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
    SetConsoleTextAttribute(h, 0x0F); fprintf(stderr, "[%s] ", prefix);
    SetConsoleTextAttribute(h, 0x0E); fprintf(stderr, "警告: ");
    SetConsoleTextAttribute(h, 0x07);
    va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

void NVi::info(const char *prefix, const char *str, ...)
{
    va_list args;
    static HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
    SetConsoleTextAttribute(h, 0x0F); fprintf(stderr, "[%s] ", prefix);
    SetConsoleTextAttribute(h, 0x0A); fprintf(stderr, "注意: ");
    SetConsoleTextAttribute(h, 0x07);
    va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

#else /* 使用\033控制字符设置终端字体颜色 */

void NVi::error(const char *prefix, const char *str, ...)
{
    va_list args;
    fprintf(stderr, "\033[1m[%s] \033[31m错误: \033[m", prefix);
    va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

void NVi::warn(const char *prefix, const char *str, ...)
{
    va_list args;
    fprintf(stderr, "\033[1m[%s] \033[33m警告: \033[m", prefix);
    va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

void NVi::info(const char *prefix, const char *str, ...)
{
    va_list args;
    fprintf(stderr, "\033[1m[%s] \033[32m注意: \033[m", prefix);
    va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

#endif

nv_ul64 NVi::operator"" _u64be(const char *str, size_t )
{
    nv_ul64 ans = 0;
    u16_t   sft = 0; 

    while (nv_ul64 ch = str[sft])
    {
        ans |= ch << (sft++ << 3);
    }

    return ans;
}

void NVi::revU16(u16_t &x)
{
    x = x >> 8 | x << 8;
}

void NVi::revU32(u32_t &x)
{
    x = x >> 24 | (x & 0xFF0000) >> 8 | (x & 0xFF00) << 8 | x << 24;
}
