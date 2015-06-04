/********************************************************************************************************

 Company: École de technologie supérieur
 Engineer:
 		Kevin Parent Legault
		Jonathan Lapointe
 Create Date:  2015-06-02
 Module Name: display.h
 Project Name: lab2n
 Target Devices: Altera cyclone V SOPC
 Tool versions: NIOS II v14.1
 Description:
	

 Revision: v1
 Revision 0.01 - File Created
 Additional Comments:


*********************************************************************************************************/
#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "hardware.h"
#include "jtagUart.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma_regs.h"

#define CURSOR 0x2
#define ERASE_LAST 0x20
#define BLUE 0x187F
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BACKGROUND BLUE
#define DISPLAY_RES_WIDTH  319
#define DISPLAY_RES_HEIGHT 239
#define SCALING_FACTOR_X 3.9 							//   319/80
#define SCALING_FACTOR_Y 3.8  // 229/60
#define DISPLAY_RES_ORIGIN 0
#define DRAW_LIMIT_HEIGHT_MIN 10
#define DRAW_LIMIT_HEIGHT_MAX 229
#define DRAW_LIMIT_EXCEEDED 1
#define OK 0
#define LABEL_POSITION_WIDTH_TOP 10
#define LABEL_POSITION_HEIGHT_TOP 1
#define COORDINATE_X_POSITION_WIDTH 58
#define COORDINATE_X_POSITION_HEIGHT 58
#define COORDINATE_Y_POSITION_WIDTH 63
#define COORDINATE_Y_POSITION_HEIGHT 58
#define X "X:"
#define Y "Y:"
#define LABEL "NiosDraw V0.1 - Kevin Parent Legault & Jonathan Lapointe"

alt_up_pixel_buffer_dma_dev* pixelptr;
alt_up_char_buffer_dev* charbuff;

U16 xLastValue;
U16 yLastValue;
/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
void InitDisplay(void);
/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
U8 DrawPixel(U16 x,U16 y,U32 color);
/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
void CleanDrawZone(void);
/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
U8 UpdateCursorPosition(U16 x, U16 y);
/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */

#endif /* DISPLAY_H_ */
