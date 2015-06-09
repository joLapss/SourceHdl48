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
 Description: Programme principale appelant l'application NiosDrawv0.1
	

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

	InitDisplay();
	mouseInit();
	for(;;)
	{

		NiosDrawApp();

	}
return 0;
}
