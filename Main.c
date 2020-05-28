#include "2440addr.h"
#include "device_driver.h"
#include "macro.h"

#define NUM_OF_SONG 2

#include <stdlib.h>

extern void showMusicList(void);
extern void moveMusicList(int, int, int);
extern void readyAudio(int);
void chooseSongToPlay(void);
void User_Main(void);

extern volatile int Key_value;

void Main(void)
{
	Led_Init();
	Key_Push_ISR_Init();
	Key_ISR_Init();
	Key_Push_ISR_Enable(1);
	Key_ISR_Enable(1);
	Nand_Init();
	Sound_Init();
	Uart_Init(115200);
	Lcd_Graphic_Init();
	Uart_Printf("Welcome GBOX World!\n");
	User_Main();
	Uart_Printf("Good Bye~\n");
	srand((unsigned int)RTC_Get_Time());
}

void User_Main(void) {
	for (;;) {
		Uart_Printf("back to main");
		showMusicList();
		chooseSongToPlay();
	}
}

void chooseSongToPlay(void) {
	int i = NUM_OF_SONG+1;
	int selectedSongIdx = 0;
	Key_value = 0;

	for (;;) {
		if (Key_value) {
			if (Key_value == 1) {
				if (selectedSongIdx == 0) continue;
				moveMusicList(selectedSongIdx-1, selectedSongIdx, 1);
				selectedSongIdx--;
			}
			if (Key_value == 3) {
				if (selectedSongIdx == 5) continue;
				moveMusicList(selectedSongIdx+1, selectedSongIdx, 0);
				selectedSongIdx++;
			}
			if (Key_value == 8) {
				if (selectedSongIdx >= NUM_OF_SONG) continue;
				readyAudio(selectedSongIdx);
				return;
			}
			Key_value = 0;
		}
	}
}

int generateRandomNumber(int maxNum) {
	return rand() % (maxNum + 1);
}
