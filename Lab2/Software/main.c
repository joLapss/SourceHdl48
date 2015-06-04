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


#include "hardware.h"
#include "jtagUart.h"
#include "system.h"
#include "display.h"
#include "mouse.h"



/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
int main(void)
{
	U16 xValue=0;
	U16 yValue=0;
	InitDisplay();
	mouseInit();
	for(;;)
	{

		if(mouseGetEvent())
		{
			xValue = mouseGetX();
			yValue = mouseGetY();
			//UpdateCursorPosition(xValue,yValue);
			if(mouseGetSWL())DrawPixel(xValue,yValue,WHITE);
			else if(mouseGetSWR()) CleanDrawZone();

		}




		//jUartSendVar(1);

	}
return 0;
}
