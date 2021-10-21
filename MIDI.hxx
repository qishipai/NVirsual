// MIDI.hxx 2021-10-21 by 云中龙++
#ifndef __MIDI_H_
#define __MIDI_H_

enum class NV_METYPE // MIDI事件类型码
{
    NOTEOFF = (nv_byte)0x80, // 音符停止发声
    NOTEON  = (nv_byte)0x90, // 音符开始发声
    KEY_AT  = (nv_byte)0xA0, // 触后音符(?
    CTROCH  = (nv_byte)0xB0, // ControlChange
    PROGCH  = (nv_byte)0xC0, // ProgramChange
    CHAN_AT = (nv_byte)0xD0, // 触后通道(?
    PITCH   = (nv_byte)0xE0, // Pitchbend
    SYSCODE = (nv_byte)0xF0, // 系统码数据包
    OTHER   = (nv_byte)0xFF, // Meta-Event
};

struct NVmidiFile  // MIDI文件类
{
    /* 类型、轨道数、分辨率 */
    u16_t type, tracks, ppnq;

    bool    *trk_over;   // 轨道结束标志
    nv_byte **trk_data;  // 轨道数据
    nv_byte **trk_ptr;   // 读取位置
    nv_byte *grp_code;   // 事件组类型码

    /* 打开MIDI文件 */
    bool mid_open(const char *name);

    /* 关闭MIDI文件 */
    void mid_close();
};

struct NVmidiEvent // MIDI事件类
{
    u32_t  tick, datasz; // tick与元数据长度
    NV_METYPE type;      // 事件类型码
    nv_byte chan, num;   // 通道号 编号
    u16_t   value;       // 事件值
    const nv_byte *data; // 元数据指针

    /* 从指定文件的指定轨道中获取事件 */
    bool get(u16_t track, NVmidiFile &midi);
};

#endif
