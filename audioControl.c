// I2S Sound CODEC Test

#include "2440addr.h"
#include "device_driver.h"
#include "macro.h"

#define DMA_TC_SIZE		(0xfffff/(sound.Play_bit_per_sample/8))
#define NUM_OF_SONG 2

extern volatile int DMA_complete[];
extern volatile int Key_value;

extern void drawPlayUI(int, int);
extern void togglePlayIcon(int);
extern void showVolume(int);

void playAudio(int);

unsigned int address, block;
unsigned int offset, size, frame;
unsigned int Play_transfer_size;
unsigned char * p[2];

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

void readyAudio(void)
{
	int i, vol = 5;

	p[0] = Get_Heap_Base();
	p[1] = p[0] + 0x100000;

	block = Nand_Page_2_Addr(100, 0, 0);
	Sound_Control_Soft_Mute(0);

	for(;;)
	{
		frame = 0;
		DMA_complete[2] = 1;

		do
		{
			Uart_Printf("\n원하는 곡의 번호? [0]~[%d]", NUM_OF_SONG-1);
			i = Uart_GetIntNum();
			Uart_Printf("\rSONG=%d\n", i);
		}while((unsigned int)i >= NUM_OF_SONG);

		Nand_Read(block+i*8, (U8 *)&offset, 4);
		Nand_Read(block+i*8+4, (U8 *)&size, 4);
		Nand_Read(block+offset, (U8 *)p[0], 44);
		address = block + offset + 44;

		Uart_Printf("Block_Addr=0x%.8X, WAV_address=0x%.8X, Offset=%d, Size=%d\n", block, address, offset, size);

		Sound_Get_WAV_Info(&sound, p[0]);
		Uart_Printf("File Size=[%u]\n", sound.Play_file_size);
		Uart_Printf("Sampling Freq:[%d]Hz\n", sound.Play_sample_freq);
		Uart_Printf("Bit per Sample:[%d]bit\n", sound.Play_bit_per_sample);

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

		Uart_Printf("VOL = %d\n", vol);
		Sound_Control_Headphone_Volume(vol);
		Sound_Set_Sampling_Rate(sound.Play_sample_freq);
		Sound_Set_Mode(IIS_TX_ONLY, sound.Play_bit_per_sample);
		Sound_IIS_Start();

		drawPlayUI(i, vol);
		playAudio(vol);
	}
}



void playAudio(int vol) {
	int finish = 0;
	int paused = 0;
	int lock = 0;

	Sound_Control_Soft_Mute(0);

	for(;;)
	{
		if(!lock && Key_value == 8) {
			if (paused) {
				Sound_Play_Pause(0);
				paused = 0;
			}
			else {
				Sound_Play_Pause(1);
				paused = 1;
			}
			togglePlayIcon(paused);
			Key_value = 0;
			lock = 1;
		}

		if (!lock && (Key_value == 1 || Key_value == 3)) {
			if (Key_value == 1 && vol < 9){
				vol++;
				showVolume(vol);
				Sound_Control_Headphone_Volume(vol);
			}
			if (Key_value == 3 && vol) {
				vol--;
				showVolume(vol);
				Sound_Control_Headphone_Volume(vol);
			}
			Key_value = 0;
			lock = 1;
		}

		if (lock && !Key_value) lock = 0;

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
			break;
		}
	}
}
