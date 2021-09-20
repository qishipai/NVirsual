#include <cstdio>
#include "NVutils.h"
#include "NVmidi.h"
#include "NVsequ.h"
using namespace std;

void StartWrite();                  // 开始写入
void WriteNoteOFF(int key);         // 写入NOTEON
void WriteNoteON(int key, int vel); // 写入NOTEOFF
void WriteDeltaT(double T);         // 写入DELAY
void CloseWrite();                  // 停止写入

static u16_t kb[128], noff[128];
static bool sust[16];

static void WriteCC64off(double T, int chn)
{
	if (!sust[chn])
	{
		u16_t ch = 1 << chn, off;
		WriteDeltaT(T);

		for (int k = 0; k < 128; ++k)
		{
			if ((off = noff[k] & ch))
			{
				noff[k] &= ~ch;

				if (kb[k] ^ off)
				{
					kb[k] &= ~off;
				}
				else
				{
					kb[k] = 0;
					WriteNoteOFF(k);
				}
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

	S.seq_init(M); StartWrite();

	double T = 0.0, dT = 0.5 / M.ppnq;
	u32_t _tick = 0, count = 0;

	while (S.event().track != M.tracks)
	{
		const NVseq_event &E = S.event();
		u16_t ch = 1 << E.chan;

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
				kb[E.num] |= ch; ++count;
				WriteDeltaT(T);
				WriteNoteON(E.num, E.value);
				break;
			}

		case (NV_METYPE::NOTEOFF):

			if (sust[E.chan])
			{
				noff[E.num] |= ch;
				break;
			}

			if (kb[E.num] ^ ch)
			{
				kb[E.num] &= ~ch;
			}
			else
			{
				kb[E.num] = 0;
				WriteDeltaT(T);
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
	CloseWrite(); S.seq_destroy(); M.mid_close();
	return 0;
}



static FILE *fp;

void StartWrite()
{
	fp = fopen("script.txt", "w");
}

void WriteNoteOFF(int key)
{
	fprintf(fp, "noteoff 0 %d\n", key);
}

void WriteNoteON(int key, int vel)
{
	fprintf(fp, "noteon 0 %d %d\n", key, vel);
}

void WriteDeltaT(double T)
{
	static double Tcache = 0.0;

	if (T > Tcache)
	{
		fprintf(fp, "sleep %lf\n", (T - Tcache) * 1000);
		Tcache = T;
	}
}

void CloseWrite()
{
	fclose(fp);
}
