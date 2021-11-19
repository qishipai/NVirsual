// <NVirsual> MIDI.hxx 2021-11-18 by 云中龙++

#pragma once

enum class NV_METYPE  /* === MIDI事件类型码 === */
{
    NOFF = (NVi::nv_byte)0x80, // 音符停止发声
    NOON = (NVi::nv_byte)0x90, // 音符开始发声
    NOAT = (NVi::nv_byte)0xA0, // 触后音符(?
    CTRO = (NVi::nv_byte)0xB0, // ControlChange
    PROG = (NVi::nv_byte)0xC0, // ProgramChange
    CHAT = (NVi::nv_byte)0xD0, // 触后通道(?
    PITH = (NVi::nv_byte)0xE0, // Pitchbend
    SYSC = (NVi::nv_byte)0xF0, // 系统码数据包
    META = (NVi::nv_byte)0xFF, // Meta-Event
};

struct NVmidiFile   /* ===== MIDI文件类 ===== */
{
    /* 类型、轨道数、分辨率 */
    NVi::u16_t type, tracks, ppnq;

    bool         *trk_over;   // 轨道结束标志
    NVi::nv_byte **trk_data;  // 轨道数据
    NVi::nv_byte **trk_ptr;   // 读取位置
    NVi::nv_byte *grp_code;   // 事件组类型码

    /* 打开MIDI文件 */
    bool mid_open(const char *name);

    void rewind_all();  // 重置全部轨道指针

    void mid_close();   // 关闭MIDI文件
};

struct NVmidiEvent  /* =====  MIDI事件类 ===== */
{
    NV_METYPE    type;         // 事件类型码
    NVi::u32_t   tick;         // 事件Tick值
    NVi::nv_byte chan, num;    // 通道与编号
    NVi::u16_t   value;        // 事件值
    NVi::size_t  datasz;       // 元数据长度
    const NVi::nv_byte *data;  // 元数据指针

    /* 从指定文件的指定轨道中获取事件 */
    bool get(NVi::u16_t track, NVmidiFile &midi);
};
