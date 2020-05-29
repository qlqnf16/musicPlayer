// I2S Sound CODEC Test

#include "2440addr.h"
#include "device_driver.h"
#include "macro.h"

#define DMA_TC_SIZE		(0xfffff/(sound.Play_bit_per_sample/8))
#define NUM_OF_SONG 6

#define PAUSECLICKED(X, Y)			(X >= 77 && X <= 99 && Y >= 187)
#define PREVCLICKED(X, Y)			(X <= 55 && Y >= 187)
#define NEXTCLICKED(X, Y)			(X >= 110 && X <= 155 && Y >= 187)
#define VOLUMEDOWNCLICKED(X, Y)		(X >= 200 && X <= 230 && Y <= 50)
#define VOLUMEUPCLICKED(X, Y)		(X >= 270 && X <= 310 && Y <= 50)
#define SHUFFLECLICKED(X, Y)		(X >= 165 && X <= 190 && Y >= 60 && Y <= 90)
#define REPEATCLICKED(X, Y)			(X >= 205 && X <= 230 && Y >= 60 && Y <= 90)
#define BACKCLICKED(X, Y)			(X <= 30 && Y <= 30)

extern volatile int DMA_complete[];
extern volatile int Touch_x, Touch_y;
extern volatile int Timer0_time_out;

extern int generateRandomNumber(int);
extern void drawPlayerUI(int, int);
extern void drawSongUI(int idx);
extern void togglePlayIcon(int);
extern void showVolume(int);
extern void drawProgressBar(int ,int);
extern void toggleShuffleIcon(int);
extern void toggleRepeatIcon(int);
extern void checkLyrics(int, int);

void readyAudio(int, int);
int playAudio(int, int);
void changeVolume(int);
int pauseAndPlayAudio(int);
int changeSong(int, int);

unsigned int address, block;
unsigned int offset, size, frame;
unsigned int Play_transfer_size;
unsigned char * p[2];

const int startBlockNums[] = {100, 100, 280, 460, 640, 790};
int vol = 5;
int lock, shuffleOn, repeatOn;

UNI_SRCC srcc;
UNI_DSTC dstc;
UNI_DCON dcon;
IIS_WAV sound = {0,0,0};

void Read_WAV_From_Nand(void)
{
	if(sound.Play_file_size > (DMA_TC_SIZE * (sound.Play_bit_per_sample/8)))
	{
		Play_transfer_size = DMA_TC_SIZE;
		sound.Play_file_size = sound.Play_file_size - DMA_TC_SIZE * (sound.Play_bit_per_sample/8);
	}

	else
	{
		Play_transfer_size = sound.Play_file_size / (sound.Play_bit_per_sample/8);
		sound.Play_file_size = 0;
	}

	frame = frame?(frame-1):(frame+1);
	Nand_Read(address, (U8 *)p[frame], Play_transfer_size * (sound.Play_bit_per_sample/8));
	address = address + Play_transfer_size * (sound.Play_bit_per_sample/8);
	dcon.st.TC = Play_transfer_size;

	Uart_Printf("NAND: %d, 0x%.8X, %d\n", frame, p[frame], dcon.st.TC);
}

void readyAudio(int blockId, int songId)
{
	int duration;

	p[0] = Get_Heap_Base();
	p[1] = p[0] + 0x100000;

	drawPlayerUI(songId, vol);

	for (;;) {
		block = Nand_Page_2_Addr(startBlockNums[songId], 0, 0);
		Sound_Control_Soft_Mute(0);
		frame = 0;
		DMA_complete[2] = 1;

		Nand_Read(block+blockId*8, (U8 *)&offset, 4);
		Nand_Read(block+blockId*8+4, (U8 *)&size, 4);
		Nand_Read(block+offset, (U8 *)p[0], 44);
		address = block + offset + 44;

		Uart_Printf("Block_Addr=0x%.8X, WAV_address=0x%.8X, Offset=%d, Size=%d\n", block, address, offset, size);

		Sound_Get_WAV_Info(&sound, p[0]);
		Uart_Printf("File Size=[%u]\n", sound.Play_file_size);
		Uart_Printf("Sampling Freq:[%d]Hz\n", sound.Play_sample_freq);
		Uart_Printf("Bit per Sample:[%d]bit\n", sound.Play_bit_per_sample);
		duration = sound.Play_file_size / ((sound.Play_bit_per_sample/8) * sound.Play_sample_freq * 2);
		Uart_Printf("재생시간 : %d\n", duration);

		srcc.udata = 0;
		srcc.st.INC = DMA_ADDR_INC;
		srcc.st.LOC = DMA_LOC_AHB;

		dstc.udata = 0;
		dstc.st.INC = DMA_ADDR_FIX;
		dstc.st.LOC = DMA_LOC_APB;
		dstc.st.CHK_INT = DMA_INT_TC;

		dcon.udata = 0;
		dcon.st.DMD_HS = DMA_DEMAND;
		dcon.st.SYNC = DMA_SYNC_PCLK;
		dcon.st.INT = DMA_INT_EN;
		dcon.st.TSZ = DMA_TSZ_1UNIT;
		dcon.st.SERVMODE = DMA_SVC_SINGLE;
		dcon.st.HWSRCSEL = DMA_HWSRC_CH2_I2SSDO;
		dcon.st.SWHW_SEL = DMA_TRIGGER_HW;
		dcon.st.RELOAD = DMA_RELOAD_OFF;
		dcon.st.DSZ = DMA_DSZ_2B;

		Read_WAV_From_Nand();

		Uart_Printf("VOL = %d shuffle %s repeat %s\n", vol, shuffleOn ? "on" : "off", repeatOn ? "on" : "off");
		Sound_Control_Headphone_Volume(vol);
		Sound_Set_Sampling_Rate(sound.Play_sample_freq);
		Sound_Set_Mode(IIS_TX_ONLY, sound.Play_bit_per_sample);
		Sound_IIS_Start();

		drawSongUI(songId);
		songId = playAudio(songId, duration);
		if (songId == -1)return;
		if (songId <= 1) {
			blockId = songId;
		} else {
			blockId = 0;
		}
	}
}



int playAudio(int idx, int duration) {
	float barStart = 0;
	float gap = 128.0 / duration;
	int finish = 0;
	int paused = 0;
	int sec = 1;

	Touch_x = Touch_y = 0;
	Sound_Control_Soft_Mute(0);
	Timer0_Delay_ISR_Enable(1, 1000);

	for(;;)
	{
		if(Timer0_time_out) {
			Timer0_time_out = 0;
			Timer0_Delay_ISR_Enable(1, 1000);

			checkLyrics(idx, sec++);
			if (paused || barStart >= 128) continue;
			drawProgressBar((int)barStart, (int)(barStart+gap));
			barStart += gap;
		}

		if(!lock && (Touch_x || Touch_y)) {
			lock = 1;

			if (PAUSECLICKED(Touch_x, Touch_y)) {
				paused = pauseAndPlayAudio(paused);
			}
			else if (NEXTCLICKED(Touch_x, Touch_y)) {
				return changeSong(idx, 1);
			}
			else if (PREVCLICKED(Touch_x, Touch_y)) {
				return changeSong(idx, 0);
			}
			else if (VOLUMEUPCLICKED(Touch_x, Touch_y)) {
				changeVolume(1);
			}
			else if (VOLUMEDOWNCLICKED(Touch_x, Touch_y)) {
				changeVolume(0);
			}
			else if (SHUFFLECLICKED(Touch_x, Touch_y)) {
				shuffleOn = !shuffleOn;
				toggleShuffleIcon(shuffleOn);
			}
			else if (REPEATCLICKED(Touch_x, Touch_y)) {
				repeatOn = !repeatOn;
				toggleRepeatIcon(repeatOn);
			}
			else if (BACKCLICKED(Touch_x, Touch_y)) {
				Sound_Stop_Sound();
				return -1;
			}

			Touch_x = Touch_y = 0;
		}

		if (lock && !Touch_x && !Touch_y) lock = 0;

		if(DMA_complete[2] && !finish)
		{
			DMA_complete[2] = 0;
			DMA_Start(2, (void *)p[frame], (void *)IISFIFO, srcc, dstc, dcon);
			Uart_Printf("DMA2: %d, 0x%.8X, %d\n", frame, p[frame], dcon.st.TC);

			if(sound.Play_file_size == 0) finish = 1;
			else Read_WAV_From_Nand();
		}

		else if(DMA_complete[2] && finish)
		{
			Uart_Printf("Stop\n");
			Sound_Stop_Sound();
			if (idx == NUM_OF_SONG-1) {
				if (repeatOn) return 0;
				return -1;
			}
			return idx+1;
		}
	}

	return -1;
}

int pauseAndPlayAudio(int paused) {
	Sound_Play_Pause(!paused);
	togglePlayIcon(!paused);
	return !paused;
}

void changeVolume(int up) {
	if ((up && vol >= 9) || (!up && vol == 0)) return;
	vol = up ? vol + 1 : vol - 1;
	showVolume(vol);
	Sound_Control_Headphone_Volume(vol);
}

int changeSong(int cur, int next) {
	if (shuffleOn) {
		int new = generateRandomNumber(NUM_OF_SONG-1);
		while (new == cur) new = generateRandomNumber(NUM_OF_SONG-1);
		return new;
	}
	if (next) {
		if (cur == NUM_OF_SONG-1) {
			if (repeatOn) return 0;
			Sound_Stop_Sound();
			return -1;
		}
		return cur+1;
	}
	else {
		if (cur == 0) {
			if (repeatOn) return NUM_OF_SONG-1;
			Sound_Stop_Sound();
			return -1;
		}
		return cur-1;
	}
}
