// <NVirsual> Utils.hxx 2021-11-18 by 云中龙++

#pragma once

namespace NVi    /* ===== 工具函数命名空间 ===== */
{
    using nv_ul64 = unsigned long long;
    using u32_t   = unsigned int;
    using u16_t   = unsigned short;
    using nv_byte = unsigned char;


    /* 显示错误 */
    void error(const char *prefix, const char *str, ...);

    /* 显示警告 */
    void warn(const char *prefix, const char *str, ...);

    /* 显示信息 */
    void info(const char *prefix, const char *str, ...);

    using std::size_t;

    /* 将字符串映射为大端序整数 */
    nv_ul64 operator"" _u64be(const char *str, size_t n);


    void revU16(u16_t &x); // 反转16位整型变量的端序

    void revU32(u32_t &x); // 反转32位整型变量的端序
};
