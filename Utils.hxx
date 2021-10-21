// Utils.hxx 2021-10-21 by 云中龙++
#ifndef __UTILS_H_
#define __UTILS_H_

typedef unsigned char  nv_byte;
typedef unsigned int   u32_t;
typedef unsigned short u16_t;

namespace NV
{
    template<typename T> struct rmPR     {typedef T t; };

    template<typename T> struct rmPR<T&> {typedef T t; };

    template<typename T> struct rmPR<T*> {typedef T t; };

    /* 日志函数 */
    void error(const char *prefix, const char *str, ...);

    void warn(const char *prefix, const char *str, ...);

    void print(const char *prefix, const char *str, ...);

    /* 调转16位整型变量的端序 */
    void revU16(u16_t &x);

    /* 调转32位整型变量的端序 */
    void revU32(u32_t &x);
};

#define NV_VERSION_STR "NVirsual 0.0.008 dev by 云中龙++"

#endif
