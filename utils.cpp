// utils.cpp 2021-08-31 by 云中龙++
#include <stdarg.h>
#include <cstdio>
#include "NVutils.h"

#if defined(_WIN32) || defined(_WIN64)

#include <Windows.h> /* 使用Windows API设置字体颜色 */

void nv_error(const char *prefix, const char *str, ...)
{
	va_list args;
	static HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
	SetConsoleTextAttribute(h, 0x0F); fprintf(stderr, "[%s] ", prefix);
	SetConsoleTextAttribute(h, 0x0C); fprintf(stderr, "错误: ");
	SetConsoleTextAttribute(h, 0x07);
	va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

void nv_warn(const char *prefix, const char *str, ...)
{
	va_list args;
	static HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
	SetConsoleTextAttribute(h, 0x0F); fprintf(stderr, "[%s] ", prefix);
	SetConsoleTextAttribute(h, 0x0E); fprintf(stderr, "警告: ");
	SetConsoleTextAttribute(h, 0x07);
	va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

void nv_print(const char *prefix, const char *str, ...)
{
	va_list args;
	static HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
	SetConsoleTextAttribute(h, 0x0A); fprintf(stderr, "[%s] ", prefix);
	SetConsoleTextAttribute(h, 0x07);
	va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

#else /* 使用\033控制字符设置字体颜色 */

void nv_error(const char *prefix, const char *str, ...)
{
	va_list args;
	fprintf(stderr, "\033[1m[%s] \033[31m错误: \033[m", prefix);
	va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

void nv_warn(const char *prefix, const char *str, ...)
{
	va_list args;
	fprintf(stderr, "\033[1m[%s] \033[33m警告: \033[m", prefix);
	va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

void nv_print(const char *prefix, const char *str, ...)
{
	va_list args;
	fprintf(stderr, "\033[32;1m[%s] \033[m", prefix);
	va_start(args, str); vfprintf(stderr, str, args); va_end(args);
}

#endif /* defined(_WIN32) || defined(_WIN64) */

void nv_revu16(u16_t &x)
{
	x = x >> 8 | x << 8;
}

void nv_revu32(u32_t &x)
{
	x = x >> 24 | (x & 0xFF0000) >> 8 | (x & 0xFF00) << 8 | x << 24;
}
