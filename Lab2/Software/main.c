/********************************************************************************************************

 Company: École de technologie supérieur
 Engineer:
 Kevin Parent Legault
 Jonathan Lapointe
 Create Date:  2015-05-26
 Module Name: main.c
 Project Name: lab2n
 Target Devices: Altera cyclone V SOPC
 Tool versions: NIOS II v14.1
 Description:


 Revision: v1
 Revision 0.01 - File Created
 Additional Comments:


 *********************************************************************************************************/

//Function declaration
/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
#include "hardware.h"
#include "jtagUart.h"
#include "system.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma_regs.h"
#include "sys/alt_sys_init.h"
#include "mouse.h"

int main(void) {
	U8 i = 0;
	U16 j = 0;
	U16 color;

	//mouseInit();

	for (;;) {
		if (mouseGetEvent()) {
			jUartSendString("pos Y:");
			jUartSendVar(mouseGetY());
			jUartSendString("pos X:");
			jUartSendVar(mouseGetX());
		}

	}
	return 0;
}
