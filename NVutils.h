// NVutils.h 2021-08-31 by 云中龙++
#ifndef __NV_UTILS_H_
#define __NV_UTILS_H_

typedef unsigned char  nv_byte;
typedef unsigned int   u32_t;
typedef unsigned short u16_t;

/* 日志函数 */
void nv_error(const char *prefix, const char *str, ...);
void nv_warn(const char *prefix, const char *str, ...);
void nv_print(const char *prefix, const char *str, ...);

/* 调转16位整型变量的端序 */
void nv_revu16(u16_t &x);

/* 调转32位整型变量的端序 */
void nv_revu32(u32_t &x);

#define NV_VERSION_STR "NVirsual 0.0.004 dev by 云中龙++"

#endif /* __NV_UTILS_H_ */
