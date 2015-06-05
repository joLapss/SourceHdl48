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
#define MOUSE_EVENT  1
#define MOUSE_IDLE   0
#define MSG_Q_SIZE	10
#define MOUSE_X_SIGN_MASK  0x10
#define MOUSE_Y_SIGN_MASK  0x20
#define MOUSE_START_BYTE_MASK 0x08

#define MOUSE_SW_R_MASK  0x01
#define MOUSE_SW_L_MASK  0x02
#define MOUSE_SW_R_SIGN_DEL   0
#define MOUSE_SW_L_SIGN_DEL   1
#define MOUSE_ERROR_FLAG "PS2 device not found!"
#define MOUSE_FLAG "PS2 device found!"
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
	START_BYTE_STATE,
	X_BYTE_STATE,
	Y_BYTE_STATE
}tMouseState;

static volatile U8 mouseState=START_BYTE_STATE;
alt_up_ps2_dev *ps2Inst;


static volatile S8 signX;
static volatile S8 signY;
static volatile U8 swLeftState;
static volatile U8 swRightState;

static volatile U16 mousePosX;
static volatile U16 mousePosY;
static volatile U8  mouseEvent;

static void ps2_isr(void *context, alt_u32 id)
{
	U8 data=0;
	if(alt_up_ps2_read_data_byte(ps2Inst, &data)==0)
	{
		switch(mouseState)
		{
		//START_STATE
		default:
		 if(data&MOUSE_START_BYTE_MASK)
		 {
			 signX=(data&MOUSE_X_SIGN_MASK);
			 signY=(data&MOUSE_Y_SIGN_MASK);
			 swLeftState =(data&MOUSE_SW_R_MASK)>>MOUSE_SW_R_SIGN_DEL;
			 swRightState=(data&MOUSE_SW_L_MASK)>>MOUSE_SW_L_SIGN_DEL;
			 mouseState=X_BYTE_STATE;
		 }
		break;

		case X_BYTE_STATE:

			if(data!=0)
			{
				//effectue une soustraction si valeur signée
				if(signX)
				{
					data = (255-data);
					if(data>mousePosX)
					mousePosX=0;
					else

					mousePosX-=data;

				}
				else
				{


						mousePosX+=data;

				}
				if(mousePosX>MOUSE_X_MAX)
					mousePosX=MOUSE_X_MAX;
			}
			mouseState=Y_BYTE_STATE;
		break;

		case Y_BYTE_STATE:

			if(data!=0)
			{
				//effectue une soustraction si valeur signée
				if(signY)
				{
					data = (255-data);
					mousePosY+=data;

				}
				else
				{
					if(data>mousePosY)
						mousePosY=0;
					else
						mousePosY-=data;

				}
				if(mousePosY>MOUSE_Y_MAX)
							mousePosY=MOUSE_Y_MAX;
				if(mousePosY<MOUSE_Y_MIN)
							mousePosY=MOUSE_Y_MIN;
			}
			mouseEvent=MOUSE_EVENT;
			mouseState=START_BYTE_STATE;
		break;
		}
	}


}
void mouseInit(void)
{
	int i;

	ps2Inst=alt_up_ps2_open_dev(MOUSE_0_NAME);

	alt_irq_register(MOUSE_0_IRQ_INTERRUPT_CONTROLLER_ID,(void *) (&ps2_context),ps2_isr);
	alt_up_ps2_enable_read_interrupt(ps2Inst);

	if(ps2Inst == 0)jUartSendString(MOUSE_ERROR_FLAG);
	else if(ps2Inst != 0)jUartSendString(MOUSE_FLAG);

}

U8 mouseGetEvent(void)
{
	U8 retVal;
	retVal=mouseEvent;
	mouseEvent=MOUSE_IDLE;
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
U8 mouseGetSWL(void)
{
	return swLeftState;
}

U8 mouseGetSWR(void)
{
	return swRightState;
}
