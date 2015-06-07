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




#include "display.h"
//Définition des symbols pour curseur et la conversion en caractère
#define CURSOR 0x2
#define ERASE_LAST 0x20
#define ACSII_CONVERSION 0x30

//Définition des couleurs de fond d'écran et de traceur
#define BLUE 0x187F
#define GREEN 0x0F00
#define YELLOW 0xFF00
#define RED 0xF000
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BACKGROUND BLACK
//Definition des dimensions de l'écran et de l'espace dessin
#define DISPLAY_RES_WIDTH  319
#define DISPLAY_RES_HEIGHT 239
#define SCALING_FACTOR_X 4  //   319/80
#define SCALING_FACTOR_Y 4  // 229/57
#define DISPLAY_RES_ORIGIN 0
#define DRAW_LIMIT_HEIGHT_MIN 10
#define DRAW_LIMIT_HEIGHT_MAX 229

//Flags
#define DRAW_LIMIT_EXCEEDED 1
#define OK 0
//Definition des coordonnées de positionnement des étiquettes
#define LABEL_POSITION_WIDTH_TOP 10
#define LABEL_POSITION_HEIGHT_TOP 1
#define COORDINATE_X_POSITION_WIDTH 58
#define COORDINATE_X_POSITION_HEIGHT 58
#define COORDINATE_Y_POSITION_WIDTH 63
#define COORDINATE_Y_POSITION_HEIGHT 58
#define X_SHIFT 2
#define Y_SHIFT 2
//Définition d'étiquettes
#define X "X:"
#define Y "Y:"
#define LABEL "NiosDraw V0.1 - Kevin Parent Legault & Jonathan Lapointe"

//Initialisation des variables
U8 init=1;
U16 xValue=0;
U16 yValue=0;
U16 xTempo=0;
U16 yTempo=0;
U8 updatePositionFlag=0;
U8 TelemetryFlagButtonL=1;
U8 TelemetryFlagButtonR=1;
U8 colorInc=0;
U16 colorSwap[4]={BLUE,GREEN,YELLOW,RED};


//Function declaration
U8 DrawPixelColored(U16 x,U16 y);
void CleanDrawZone(void);
U8 UpdateCursorPosition(U16 x, U16 y);
void DisplayCoordinate(U16 x, U16 y);
void SendTelemetry(U16 x, U16 y, U8 FlagR, U8 FlagL);

/*
 * @fn 	 void InitDisplay(void)
 * @des  Ouvre les périphérique pixel buffer et char buffer
 * @arg  N'a besoin d'aucun argument
 * @ret  Retourne rien.
 */
void InitDisplay(void)
{
	pixelptr=alt_up_pixel_buffer_dma_open_dev(PIXEL_BUFFER_NAME);
	charbuff = alt_up_char_buffer_open_dev("/dev/character_buffer");
}
/*
 * @fn 	 DrawPixel(U16 x, U16 y,U32 color)
 * @des  Affiche un pixel de la couleur désiré aux coordonnés x-y spécifiez
 * @arg  U16 x , U16 y, U32 color
 * @ret  Retourne 1 si la zone de dessin est dépassé.
 */
U8 DrawPixelColored(U16 x,U16 y)
{
	U16 val=0;
	if(colorInc > 3)colorInc = 0;
	if(y < DRAW_LIMIT_HEIGHT_MIN  || y > DRAW_LIMIT_HEIGHT_MAX )
	{
		val = DRAW_LIMIT_EXCEEDED;
	}
	else
	{
		val = OK;
		alt_up_pixel_buffer_dma_draw(pixelptr,colorSwap[colorInc++],x,y);
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
	alt_up_pixel_buffer_dma_draw_box(pixelptr,DISPLAY_RES_ORIGIN,DRAW_LIMIT_HEIGHT_MIN, DISPLAY_RES_WIDTH, DRAW_LIMIT_HEIGHT_MAX,BACKGROUND, 0);
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
	if(y < DRAW_LIMIT_HEIGHT_MIN  || y > DRAW_LIMIT_HEIGHT_MAX )val = DRAW_LIMIT_EXCEEDED;
	else
	{
		val = OK;
		alt_up_char_buffer_draw(charbuff,ERASE_LAST,xLastValue>>2,yLastValue>>2);
		alt_up_char_buffer_draw(charbuff,CURSOR,x>>2,y>>2);

		xLastValue = x;
		yLastValue = y;
	}
return val;
}

/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
void NiosDrawApp(void)
{
	if(init)
	{
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

		while(mouseGetNbEvent())
		{
				updatePositionFlag = 1;
				xValue = mouseGetX();
				yValue = mouseGetY();
				xTempo=xValue;
				yTempo=yValue;
				if(mouseGetSWL())
				{
					DrawPixelColored(xValue,yValue);
					SendTelemetry(xTempo,yTempo,0,TelemetryFlagButtonL);
					TelemetryFlagButtonL=0;
				}
				else if(mouseGetSWR())
				{
					CleanDrawZone();
					SendTelemetry(xTempo,yTempo,TelemetryFlagButtonR,0);
					TelemetryFlagButtonR=0;
				}
				if(mouseGetSWL()==0)TelemetryFlagButtonL=1;
				if(mouseGetSWR()==0)TelemetryFlagButtonR=1;
				mousePtrOutInc();
				alt_up_pixel_buffer_dma_swap_buffers(pixelptr);
		}

		if(updatePositionFlag == 1)
		{
			DisplayCoordinate(xTempo, yTempo);
			UpdateCursorPosition(xValue,yValue);
			updatePositionFlag = 0;
		}
	}
}

/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
void DisplayCoordinate(U16 x, U16 y)
{
	U8 valueConv[4];
	S8 i=0;
	valueConv[3]=0;

		for(i=2;i>=0;i--)
		{
			valueConv[i]=(x %10)+0x30;
			x=x/10;
		}
		alt_up_char_buffer_string(charbuff,valueConv,(COORDINATE_X_POSITION_WIDTH + X_SHIFT),COORDINATE_X_POSITION_HEIGHT);

		for(i=2;i>=0;i--)
		{
			valueConv[i]=(y %10)+0x30;
			y=y/10;
		}
		alt_up_char_buffer_string(charbuff,valueConv,COORDINATE_Y_POSITION_WIDTH+Y_SHIFT, COORDINATE_Y_POSITION_HEIGHT);

}
/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
void SendTelemetry(U16 x, U16 y, U8 FlagR,U8 FlagL)
{

	U8 valueConvx[4],valueConvy[4];
	S8 i=0;
	valueConvx[3]=0;
	valueConvy[3]=0;

	for(i=2;i>=0;i--)
	{
		valueConvx[i]=(x %10)+0x30;
		x=x/10;
	}
	for(i=2;i>=0;i--)
	{
		valueConvy[i]=(y %10)+0x30;
		y=y/10;
	}

	if(FlagR)
	{

		jUartSendString("Right button pressed!\n");

		jUartSendString("X :  ");
		jUartSendString(valueConvx);
		jUartSendString(" ");

		jUartSendString("Y :  ");
		jUartSendString(valueConvy);
		jUartSendString("\n ");

	}
	if(FlagL)
	{

		jUartSendString("Left button pressed!\n");
		jUartSendString("X :  ");
		jUartSendString(valueConvx);
		jUartSendString(" ");

		jUartSendString("Y :  ");
		jUartSendString(valueConvy);
		jUartSendString("\n ");

	}
}


