#include <cstdio>
#include "Utils.hxx"
#include "MIDI.hxx"
using namespace NV;

bool NVmidiFile::mid_open(const char *name)
{
    FILE *midifp = fopen(name, "rb");
    u32_t tmp = 0, size = 0;

    if (!midifp)
    {
        error("MIDI", "%s: 不存在或无法访问。\n", name);
        return false;
    }

    fread(&tmp, 4, 1, midifp);

    if (tmp != 0x6468544Du)
    {
        error("MIDI", "%s: 不是标准MIDI文件!\n", name);
        return (fclose(midifp), false);
    }

    fread(&size  , 4, 1, midifp);
    fread(&type  , 2, 1, midifp);
    fread(&tracks, 2, 1, midifp);
    fread(&ppnq  , 2, 1, midifp);
    revU32(size  ); revU16(type);
    revU16(tracks); revU16(ppnq);
    fseek(midifp, SEEK_SET, size + 8);

    trk_over = new bool     [tracks];
    trk_data = new nv_byte* [tracks];
    trk_ptr  = new nv_byte* [tracks];
    grp_code = new nv_byte  [tracks];

    for (u16_t i = 0; i < tracks; ++i)
    {
        fread(&tmp , 4, 1, midifp);
        fread(&size, 4, 1, midifp);

        if (tmp != 0x6B72544Du)
        {
            fclose(midifp); tracks  = i;
            error("MIDI", "异常数据!(track%hu)\n", i);
            return (mid_close(), false);
        }

        trk_over[i] = false; tmp = 0;
        grp_code[i] = 0x0Fu; revU32(size);
        nv_byte *dat = new nv_byte [size];
        fread(dat, size,  1, midifp);
        trk_data[i] = trk_ptr[i] = dat;
    }

    return (fclose(midifp), true);
}

void NVmidiFile::mid_close()
{
    for (u16_t trk = 0; trk < tracks; ++trk)
    {
        delete[] trk_data[trk];
    }

    delete[]  trk_data; delete[]   trk_ptr;
    trk_data = nullptr; trk_ptr  = nullptr;
    delete[]  trk_over; delete[]  grp_code;
    trk_over = nullptr; grp_code = nullptr;
}

static u32_t getVLi_U32(nv_byte **p)
{
    u32_t VLi = **p & 0x7Fu;

    while (*(*p)++ & 0x80u)
    {
        (VLi <<= 7) |= **p & 0x7Fu;
    }

    return VLi;
}

bool NVmidiEvent::get(u16_t track, NVmidiFile &midi)
{
    if (midi.trk_over[track])
    {
        return false;
    }

    nv_byte code, **p = midi.trk_ptr + track;

    tick = getVLi_U32(p);

    if (**p & 0x80u)
    {
        code = midi.grp_code[track] = *(*p)++;
    }
    else
    {
        code = midi.grp_code[track];
    }

    chan = code & 0x0Fu;

    switch (type = (NV_METYPE)(code & 0xF0u))
    {
    case (NV_METYPE::NOTEOFF):
    case (NV_METYPE::NOTEON):
    case (NV_METYPE::KEY_AT):
    case (NV_METYPE::CTROCH):

        num = *(*p)++;

    case (NV_METYPE::PROGCH):
    case (NV_METYPE::CHAN_AT):

        value = *(*p)++;
        break;

    case (NV_METYPE::PITCH):

        value = *(*p)++;
        value = value << 7 | *(*p)++;
        break;

    case (NV_METYPE::SYSCODE):

        if (code == 0xFFu)
        {
            if ((num = *(*p)++) == 0x2Fu)
            {
                midi.trk_over[track] = true;
            }

            type = NV_METYPE::OTHER;
        }
        else
        {
            num = code & 0x0Fu;
        }

        datasz = getVLi_U32(p); data = *p;
        *p = *p + datasz; chan = (nv_byte)-1;
        break;

    default:

        warn("MIDI", "异常数据!(track%hu)\n", track);
        return false;
    }

    return true;
}
