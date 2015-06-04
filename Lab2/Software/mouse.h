/********************************************************************************************************
 Company: École de technologie supérieur
 Engineer:
 		Kevin Parent Legault
		Jonathan Lapointe
 Create Date: 2015-06-02
 Module Name: mouse.h
 Project Name: lab2n
 Target Devices: Altera cyclone V SOPC
 Tool versions: NIOS II v14.1
 Description:
	

 Revision: v1
 Revision 0.01 - File Created
 Additional Comments:


********************************************************************************************************/
#include"hardware.h"


void mouseInit(void);

U8 mouseGetEvent(void);

U16 mouseGetX(void);


U16 mouseGetY(void);

U8 mouseGetSWL(void);


U8 mouseGetSWR(void);
