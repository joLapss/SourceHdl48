/********************************************************************************************************

 Company: École de technologie supérieur
 Engineer:
 		Kevin Parent Legault
		Jonathan Lapointe
 Create Date:  2015-06-02
 Module Name: display.c
 Project Name: lab2n
 Target Devices: Altera cyclone V SOPC
 Tool versions: NIOS II v14.1
 Description:
	

 Revision: v1
 Revision 0.01 - File Created
 Additional Comments:


*********************************************************************************************************/

//Function declaration


#include "display.h"

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


U8 init=1;

U8 DrawPixel(U16 x,U16 y,U32 color);
void CleanDrawZone(void);
U8 UpdateCursorPosition(U16 x, U16 y);
void CursorControl(U16 x, U16 y,U32 color);


void InitDisplay(void)
{
	pixelptr=alt_up_pixel_buffer_dma_open_dev(PIXEL_BUFFER_NAME);
	charbuff = alt_up_char_buffer_open_dev("/dev/character_buffer");
}
/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
U8 DrawPixel(U16 x,U16 y,U32 color)
{
	U16 val=0;

	if(y < DRAW_LIMIT_HEIGHT_MIN  || y > DRAW_LIMIT_HEIGHT_MAX )
	{
		val = DRAW_LIMIT_EXCEEDED;
	}
	else
	{
		val = OK;
		alt_up_pixel_buffer_dma_draw(pixelptr,color,x,y);
	}

return val;
}
/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
void CleanDrawZone(void)
{
	alt_up_pixel_buffer_dma_clear_screen(pixelptr,0);
	alt_up_pixel_buffer_dma_draw_box(pixelptr,DISPLAY_RES_ORIGIN,DRAW_LIMIT_HEIGHT_MIN, DISPLAY_RES_WIDTH, DRAW_LIMIT_HEIGHT_MAX, BACKGROUND, 0);
}
/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
U8 UpdateCursorPosition(U16 x,U16 y)
{
	U16 val=0;
	U8 valueConv[3];
	U8 i=0;
	U8 xTemp, yTemp;
	xTemp = x;


	if(y < DRAW_LIMIT_HEIGHT_MIN  || y > DRAW_LIMIT_HEIGHT_MAX )
	{
		val = DRAW_LIMIT_EXCEEDED;
	}
	else
	{
		val = OK;
		CursorControl(x,y,WHITE);
		CursorControl(xLastValue,yLastValue,BACKGROUND);
		xLastValue = x;
		yLastValue = y;

//		for(i=2;i>=0;i--)
//		{
//			valueConv[i]=(xTemp %10)+0x30;
//			alt_up_char_buffer_draw(charbuff,valueConv[i],59+i,58);
//			xTemp=xTemp/10;
//
//		}


	}

return val;
}
void CursorControl(U16 x, U16 y,U32 color)
{

	alt_up_pixel_buffer_dma_draw_hline(pixelptr,(x-7),(x-2),y,color,1);
	alt_up_pixel_buffer_dma_draw_hline(pixelptr,(x+2),(x+7),y,color,1);
	alt_up_pixel_buffer_dma_draw_vline(pixelptr,x,(y-7),(y-2),color,1);
	alt_up_pixel_buffer_dma_draw_vline(pixelptr,x,(y+2),(y+7),color,1);
	alt_up_pixel_buffer_dma_draw(pixelptr,color,x+1,y+1);




}
void NiosDrawApp(void)
{
	U16 xValue=0;
	U16 yValue=0;
	if(init)
	{
		alt_up_pixel_buffer_dma_clear_screen(pixelptr,0);
		alt_up_char_buffer_clear(charbuff);
		alt_up_pixel_buffer_dma_draw_box(pixelptr,DISPLAY_RES_ORIGIN,DRAW_LIMIT_HEIGHT_MIN, DISPLAY_RES_WIDTH, DRAW_LIMIT_HEIGHT_MAX,BACKGROUND, 0);
		alt_up_char_buffer_string(charbuff,LABEL,LABEL_POSITION_WIDTH_TOP, LABEL_POSITION_HEIGHT_TOP);
		alt_up_char_buffer_string(charbuff,X,COORDINATE_X_POSITION_WIDTH,COORDINATE_X_POSITION_HEIGHT);
		alt_up_char_buffer_string(charbuff,Y,COORDINATE_Y_POSITION_WIDTH,COORDINATE_Y_POSITION_HEIGHT);
		alt_up_pixel_buffer_dma_swap_buffers(pixelptr);
	init = 0;
	}

if(alt_up_pixel_buffer_dma_check_swap_buffers_status(pixelptr) == 0)
{
	if(mouseGetEvent())
	{

				xValue = mouseGetX();
				yValue = mouseGetY();
				//alt_up_pixel_buffer_dma_clear_screen(pixelptr,1);
				//alt_up_pixel_buffer_dma_draw_box(pixelptr,DISPLAY_RES_ORIGIN,DRAW_LIMIT_HEIGHT_MIN, DISPLAY_RES_WIDTH, DRAW_LIMIT_HEIGHT_MAX, BACKGROUND, 1);
				//CursorControl(xValue, yValue,WHITE);
				UpdateCursorPosition(xValue,yValue);
				if(mouseGetSWL())DrawPixel(xValue,yValue,WHITE);
				else if(mouseGetSWR()) CleanDrawZone();
				alt_up_pixel_buffer_dma_swap_buffers(pixelptr);
	}
}

}


