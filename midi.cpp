// midi.cpp 2021-08-31 by 云中龙++
#include <cstdio>
#include "NVutils.h"
#include "NVmidi.h"

bool NVmidiFile::mid_open(const char *name)
{
	u32_t tmp = 0, size = 0;
	FILE *midifp = fopen(name, "rb");

	if (!midifp)
	{
		nv_error("NVmidi", "%s: 不存在或无法访问。  \n", name);
		return false;
	}

	fread(&tmp, 4, 1, midifp);

	if (tmp != 0x6468544Du)
	{
		nv_error("NVmidi", "%s: 不是标准MIDI文件。 \n", name);
		fclose(midifp);
	}

	fread(&size   , 4, 1, midifp);
	fread(&type   , 2, 1, midifp);
	fread(&tracks , 2, 1, midifp);
	fread(&ppnq   , 2, 1, midifp);
	nv_revu32(size  ); nv_revu16(type);
	nv_revu16(tracks); nv_revu16(ppnq);
	fseeko(midifp, SEEK_SET, (off_t)size + 8);

	trk_over = new bool     [tracks];
	trk_data = new nv_byte* [tracks];
	trk_ptr  = new nv_byte* [tracks];
	grp_code = new nv_byte  [tracks];

	for (u16_t i = 0; i < tracks; ++i)
	{
		fread(&tmp,  4, 1, midifp);
		fread(&size, 4, 1, midifp);
		trk_over[i] = false;
		grp_code[i] = 0x0Fu;

		if (tmp != 0x6B72544Du)
		{
			nv_error("NVmidi", "异常数据！(track=%hu)\n", i);
			tracks = i;
			mid_close(); fclose(midifp);
			return false;
		}

		nv_revu32(size); tmp = 0;
		trk_data[i] = new nv_byte [size];
		trk_ptr[i] = trk_data[i];
		fread(trk_data[i], size, 1, midifp);
	}

	fclose(midifp);
	return true;
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

static inline u32_t nv_getVLi_u32(nv_byte **p)
{
	u32_t VLnum = **p & 0x7Fu;

	while (*(*p)++ & 0x80u)
	{
		(VLnum <<= 7) |= **p & 0x7Fu;
	}

	return VLnum;
}

bool NVmidiEvent::get(u16_t track, NVmidiFile &midi)
{
	if (midi.trk_over[track])
	{
		return false;
	}

	nv_byte code, **p = midi.trk_ptr + track;
	tick = nv_getVLi_u32(p);

	if  (**p & 0x80u)
	{
		code = *(*p)++;
		midi.grp_code[track] = code;
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

		datasz = nv_getVLi_u32(p);
		data   = *p; *p += datasz; chan = -1;
		break;

	default:

		nv_warn("NVmidi", "异常数据！(track=%hu) \n", track);
		return false;
	}

	return true;
}
