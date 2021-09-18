#include <cstdio>
#include "NVutils.h"
#include "NVmidi.h"
#include "NVsequ.h"
using namespace std;

FILE *fp;

static void WriteDeltaT(double T)
{
	static double Tcache = 0.0;

	if (T > Tcache)
	{
		fprintf(fp, "sleep %lf\n", (T - Tcache) * 1000);
		Tcache = T;
	}
}

static void WriteNoteOFF(int key)
{
	fprintf(fp, "noteoff 0 %d\n", key);
}

static void WriteNoteON(int key, int vel)
{
	fprintf(fp, "noteon 0 %d %d\n", key, vel);
}

u16_t kb[128], noff[128];
bool  sust[16];

static void WriteCC64off(double T, int chn)
{
	if (!sust[chn])
	{
		u16_t code = 1 << chn;
		WriteDeltaT(T);

		for (int k = 0; k < 128; noff[k++] &= ~code)
		{
			if (kb[k] ^ (noff[k] & code))
			{
				kb[k] &= ~(noff[k] & code);
			}
			else
			{
				WriteNoteOFF(k); kb[k] = 0;
			}
		}
	}
}

int main(int ac, char *av[])
{
	NVmidiFile  M;
	NVsequencer S;

	if (ac != 2 || !M.mid_open(av[1]))
	{
		nv_print("Usage", "%s <*.mid>\n", av[0]);
		return 1;
	}

	nv_print("NVmidi", "类型码: %4hu\n", M.type);
	nv_print("NVmidi", "轨道数: %4hu\n", M.tracks);
	nv_print("NVmidi", "分辨率: %4hu\n", M.ppnq);

	fp = fopen("script.txt", "w");

	S.seq_init(M);

	double T = 0.0, dT = 0.5 / M.ppnq;
	u32_t _tick = 0, count = 0;

	while (S.event().track != M.tracks)
	{
		const NVseq_event &E = S.event();

		if (E.abstick > _tick)
		{
			T += dT * (E.abstick - _tick);
			_tick = E.abstick;
		}

		switch (E.type)
		{
		case (NV_METYPE::OTHER):

			if (E.num == 0x51u)
			{
				u32_t speed = *E.data;
				(speed <<= 8) |= *(E.data + 1);
				(speed <<= 8) |= *(E.data + 2);
				dT = 1e-6 * speed / M.ppnq;
			}

			break;

		case (NV_METYPE::NOTEON):

			if (E.value > 0)
			{
				kb[E.num] |= 1 << E.chan;
				++count; WriteDeltaT(T);
				WriteNoteON(E.num, E.value);
				break;
			}

		case (NV_METYPE::NOTEOFF):

			if (sust[E.chan])
			{
				noff[E.num] |= 1 << E.chan;
				break;
			}

			if (kb[E.num] ^ (1 << E.chan))
			{
				kb[E.num] &= ~(1 << E.chan);
			}
			else
			{
				kb[E.num] = 0; WriteDeltaT(T);
				WriteNoteOFF(E.num);
			}

			break;

		case (NV_METYPE::CTROCH):

			if (E.num == 64)
			{
				sust[E.chan] = E.value > 64;
				WriteCC64off(T, E.chan);
			}

		default: break;
		}

		S.seq_next(M);
	}

	int t = T + 0.5;

	printf("共%d音符!!\n", count);
	printf("时长: %02d:%02d:%02d\n", t / 3600, t / 60 % 60, t % 60);
	fclose(fp); S.seq_destroy(); M.mid_close();
	return 0;
}