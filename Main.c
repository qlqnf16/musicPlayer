#include "2440addr.h"
#include "device_driver.h"
#include "macro.h"

#define NUM_OF_SONG 6

#include <stdlib.h>

extern void showMusicList(void);
extern void moveMusicList(int);
extern void readyAudio(int, int);
void chooseSongToPlay(void);
void User_Main(void);

extern volatile int Touch_x, Touch_y;

void Main(void)
{
	Led_Init();
	Key_Push_ISR_Init();
	Key_ISR_Init();
	Key_Push_ISR_Enable(1);
	Key_ISR_Enable(1);
	Touch_ISR_Enable(1);

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
		showMusicList();
		chooseSongToPlay();
	}
}

void chooseSongToPlay(void) {
	int currentTop = 0;
	int i, j, lock = 0;
	Touch_x = Touch_y = 0;

	for (;;) {
		if (!lock && (Touch_x || Touch_y)) {
			for (i = currentTop, j = 0; i < currentTop+4; i++, j++) {
				if (i >= NUM_OF_SONG) continue;
				if (Touch_x > 275 || (Touch_y > 55 * (j+1) || Touch_y < 55 * j)) continue;
				Touch_x = Touch_y = 0;
				if (i <= 1) readyAudio(i, i);
				else readyAudio(0, i);
				return;
			}

			if (Touch_x >= 275 && Touch_y > 0 && Touch_y < 30) {
				if (currentTop) {
					moveMusicList(1);
					currentTop--;
				}
			}
			if (Touch_x >= 275 && Touch_y < 240 && Touch_y > 200) {
				if (currentTop != 2) {
					moveMusicList(0);
					currentTop++;
				}
			}
			Uart_Printf("currentTOP: %d", currentTop);

			Touch_x = Touch_y = 0;
			lock = 1;
		}

		if (lock && (!Touch_x && !Touch_y)) {
			lock = 0;
		}
	}
}

int generateRandomNumber(int maxNum) {
	return rand() % (maxNum + 1);
}
