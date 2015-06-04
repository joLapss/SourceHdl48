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


void CursorControl(U16 x, U16 y,U32 color);
/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
void InitDisplay(void)
{

	pixelptr=alt_up_pixel_buffer_dma_open_dev(PIXEL_BUFFER_NAME);
	charbuff = alt_up_char_buffer_open_dev("/dev/character_buffer");
	alt_up_pixel_buffer_dma_clear_screen(pixelptr,0);
	alt_up_char_buffer_clear(charbuff);
	alt_up_pixel_buffer_dma_draw_box(pixelptr,DISPLAY_RES_ORIGIN,DRAW_LIMIT_HEIGHT_MIN, DISPLAY_RES_WIDTH, DRAW_LIMIT_HEIGHT_MAX,BACKGROUND, 0);
	alt_up_char_buffer_string(charbuff,LABEL,LABEL_POSITION_WIDTH_TOP, LABEL_POSITION_HEIGHT_TOP);
	alt_up_char_buffer_string(charbuff,X,COORDINATE_X_POSITION_WIDTH,COORDINATE_X_POSITION_HEIGHT);
	alt_up_char_buffer_string(charbuff,Y,COORDINATE_Y_POSITION_WIDTH,COORDINATE_Y_POSITION_HEIGHT);

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
	U8 valueConv[2];
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
		CursorControl(x,y,BLACK);
		CursorControl(xLastValue,yLastValue,BACKGROUND);
		xLastValue = x;
		yLastValue = y;

		for(i=2;i>=0;i--)
		{
			valueConv[i]=(xTemp %10)+0x30;
			alt_up_char_buffer_draw(charbuff,valueConv[i],59+i,58);
			xTemp=xTemp/10;
		}


	}

return val;
}
void CursorControl(U16 x, U16 y,U32 color)
{

	alt_up_pixel_buffer_dma_draw_hline(pixelptr,(x-7),(x-2),y,color,0);
	alt_up_pixel_buffer_dma_draw_hline(pixelptr,(x+2),(x+7),y,color,0);
	alt_up_pixel_buffer_dma_draw_vline(pixelptr,x,(y-7),(y-2),color,0);
	alt_up_pixel_buffer_dma_draw_vline(pixelptr,x,(y+2),(y+7),color,0);
	alt_up_pixel_buffer_dma_draw(pixelptr,color,x+1,y+1);




}


