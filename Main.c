#include "2440addr.h"
#include "device_driver.h"
#include "macro.h"

#include <stdlib.h>

void User_Main(void);

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

extern void chooseSongToPlay(void);
extern void showMusicList(void);

void User_Main(void) {
	for (;;) {
		showMusicList();
		chooseSongToPlay();
	}
}

int generateRandomNumber(int maxNum) {
	return rand() % (maxNum + 1);
}
