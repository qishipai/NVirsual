// <NVirsual> Sequ.hxx 2021-11-18 by 云中龙++

#pragma once

struct NVseq_event: /* 序列器MIDI事件类 */
    public NVmidiEvent
{
    NVi::u16_t track;   // 轨道号

    NVi::u32_t abstick; // 绝对Tick
};

class NVsequencer /* == 序列器类 == */
{
public:

    /* 初始化此序列器 */
    void seq_start(NVmidiFile &midi);

    /* 重置此序列器 */
    void seq_reset(NVmidiFile &midi);

    /* 获取下一个MIDI事件 */
    void seq_next(NVmidiFile &midi);

    /* 当前MIDI事件 */
    const NVseq_event& event() const;

    /* 关闭此序列器 */
    void seq_destroy();

private:

    NVi::u32_t   L;    // 最大结点数
    NVi::u16_t  *T;    // 抉择树
    NVseq_event *E;    // 事件缓冲器

    /* 更新抉择树指定节点 */
    void update(int p, int a, int b);
};
