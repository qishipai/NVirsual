// <NVirsual> Nlist.hxx 2021-11-18 by 云中龙++

#pragma once

#include <stack>
#include <list>

struct NVnote    /* ===== 绘制用音符类 ===== */
{
    NVi::u16_t   track;          // 所属轨道
    double       Tstart, Tend;   // 首尾时间
    NVi::nv_byte chn, key, vel;  // 音符属性

    NVnote(double T, const NVseq_event &E);
};

class NVnoteList  /* ===== 音符队列类 ===== */
{
public:

    NVmidiFile M;      // MIDI文件
    double Tread;      // 当前读取位置(秒)
    std::list<NVnote> L[128]; // 音符列表

    /* 打开一个MIDI文件 */
    bool start_parse(const char *name);

    /* 关闭组件 */
    void destroy_all();

    /* 定位至T秒，并清空列表 */
    void list_seek(double T);

    /* 将第T秒前的音符放入列表 */
    void update_to(double T);

    void OR();  // 懂BM的都懂，不懂的用不到

    /* 移除列表中第T秒前的音符 */
    void remove_to(double T);

private:

    NVsequencer S;       // 事件序列器
    double     dT;       // 速率
    NVi::u32_t abstick;  // 当前读取位置(tick)
    std::stack<std::list<NVnote>::iterator> (*keys)[128];
};
