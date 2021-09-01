// NVsequ.h 2021-08-31 by 云中龙++
#ifndef __NV_SEQU_H_
#define __NV_SEQU_H_

class NVseq_event: // 序列器MIDI事件类
	public NVmidiEvent
{
public:

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

	/* 当前MIDI事件 */
	const NVseq_event& event();

	/* 获取下一个MIDI事件 */
	void seq_next(NVmidiFile &midi);

	/* 关闭此序列器 */
	void seq_destroy();

private:

	u32_t       len;
	u16_t       *T;    // 抉择树
	NVseq_event *E;    // 事件缓存

	/* 更新抉择树指定节点 */
	void update(int p, int a, int b);
};

#endif /* __NV_SEQU_H_ */
