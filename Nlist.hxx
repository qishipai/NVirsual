// Nlist.hxx 2021-10-21 by 云中龙++
#ifndef __NLIST_H_
#define __NLIST_H_

#include <stack>
#include <list>

struct NVnote // 绘制用音符类
{
    double  Tstart, Tend;       // 首尾时间
    u16_t   track;              // 所属轨道
    nv_byte channel, key, vel;  // 音符属性

    NVnote(double T, const NVseq_event &E);
};

class NVnoteList // 音符队列
{
public:

    NVmidiFile M;    // MIDI文件
    bool   err;      // 错误标志
    double Tread;    // 当前读取位置(秒)
    std::list<NVnote> L[128];  // 音符列表

    /* 打开一个MIDI文件 */
    NVnoteList(const char *name);

    ~NVnoteList();

    /* 将第T秒前的音符放入列表 */
    void update_to(double T);

    void OR(); // 懂BM的都懂，不懂的用不到

    /* 移除列表中第T秒前的音符 */
    void remove_to(double T);

private:

    NVsequencer S;   // 事件排列器
    double dT;       // 速率
    u32_t  abstick;  // 当前读取位置(tick)
    std::stack<std::list<NVnote>::iterator> (*keys)[128];
};

#endif
