/********************************************************************************************************
 Company: École de technologie supérieur
 Engineer:
 		Kevin Parent Legault
		Jonathan Lapointe
 Create Date: 2015-06-02
 Module Name: mouse.c
 Project Name: lab2n
 Target Devices: Altera cyclone V SOPC
 Tool versions: NIOS II v14.1
 Description:
	

 Revision: v1
 Revision 0.01 - File Created
 Additional Comments:


********************************************************************************************************/

#include "hardware.h"
#include "mouse.h"
#include "sys/alt_irq.h"
#include "altera_up_avalon_ps2.h"

#define MOUSE_Y_MAX 229
#define MOUSE_Y_MIN 10
#define MOUSE_X_MAX 319
#define MOUSE_EVENT      1
#define MOUSE_NO_EVENT   0
#define MSG_Q_SIZE			10
typedef struct message_queue {
	volatile unsigned char messages[MSG_Q_SIZE][3];		//Tableau de messages
	volatile unsigned char read_addr;					//Addresse de lecture
	volatile unsigned char write_addr;					//Addresse d'ecriture
}message_queue_t;
//Declaration de la structure du contenant de variables de contexte
typedef struct context {
	volatile unsigned char reset_flag, ready;	//Drapeaux de statut du PS/2
	message_queue_t *message_q;					//Pointeur vers le tableau de message
	alt_up_ps2_dev *ps2;						//Pointeur vers le port PS/2
} context_t;
context_t ps2_context;
typedef enum eMouseState
{
	START_BYTE,
	X_BYTE,
	Y_BYTE
}tMouseState;

static U8 mouseState=START_BYTE;
alt_up_ps2_dev *ps2Inst;
U8 mouseStatusByte=0;
U8 mouseXByte=0;
U8 mouseYByte=0;

U8 mouseLastStatusByte=0;
U8 mouseLastXByte=0;
U8 mouseLastYByte=0;

U16 mousePosX;
U16 mousePosY;
U8 mouseEvent;

static void ps2_isr(void *context, alt_u32 id)
{
	context_t *ctxt=context;
	U8 data;
	if(alt_up_ps2_read_data_byte(ps2Inst, &data)==0)
	{
		switch(mouseState)
		{

		default:
		 if(data&0x08)
		 {
			 mouseStatusByte=data;
			 mouseState=X_BYTE;
		 }
		break;

		case X_BYTE:
			mouseXByte=data;
		break;

		case Y_BYTE:
			mouseYByte=data;
		break;

		if(mouseLastStatusByte!=mouseStatusByte)
		{

			mouseEvent=MOUSE_EVENT;
			mouseLastStatusByte=mouseStatusByte;
		}
		if(mouseLastXByte!=mouseXByte)
		{
			mousePosX+=(mouseXByte-mouseLastXByte);
			if(mousePosX>MOUSE_X_MAX)
				mousePosX=MOUSE_X_MAX;
			mouseEvent=MOUSE_EVENT;
;
			mouseLastXByte=mouseXByte;
		}

		if(mouseLastYByte!=mouseYByte)
		{
			mousePosY+=(mouseYByte-mouseLastYByte);
			if(mousePosX>MOUSE_Y_MAX)
					mousePosX=MOUSE_Y_MAX;
			if(mousePosX<MOUSE_Y_MIN)
					mousePosX=MOUSE_Y_MIN;
			mouseEvent=MOUSE_EVENT;
			mouseLastYByte=mouseYByte;
		}
	}




	}


}
void mouseInit(void)
{
	ps2Inst=alt_up_ps2_open_dev(MOUSE_0_NAME);
	alt_irq_register(MOUSE_0_IRQ_INTERRUPT_CONTROLLER_ID,(void *) (&ps2_context),ps2_isr);

}

U8 mouseGetEvent(void)
{
	U8 retVal=mouseEvent;
	mouseEvent=MOUSE_NO_EVENT;
	return retVal;
}
U16 mouseGetX(void)
{
	return mousePosX;
}

U16 mouseGetY(void)
{
	return mousePosY;
}
