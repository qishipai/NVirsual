// Sequ.hxx 2021-10-21 by 云中龙++
#ifndef __SEQU_H_
#define __SEQU_H_

struct NVseq_event: // 序列器MIDI事件类
    public NVmidiEvent
{
    /* 事件所属轨道 */
    u16_t track;

    /* 从零时刻计的tick值 */
    u32_t abstick;
};

class NVsequencer // 序列器类
{
public:

    /* 初始化此序列器 */
    void seq_init(NVmidiFile &midi);

    /* 获取下一个MIDI事件 */
    void seq_next(NVmidiFile &midi);

    /* 当前MIDI事件 */
    const NVseq_event& event() const;

    /* 关闭此序列器 */
    void seq_destroy();

private:

    u32_t        L;    // 最大结点数
    u16_t       *T;    // 抉择树
    NVseq_event *E;    // 事件缓冲器

    /* 更新抉择树指定节点 */
    void update(int p, int a, int b);
};

#endif
