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
#include "jtagUart.h"

#define MOUSE_Y_MAX 229
#define MOUSE_Y_MIN 10
#define MOUSE_X_MAX 319
#define MOUSE_EVENT  1
#define MOUSE_IDLE   0
#define MSG_Q_SIZE	10
#define MOUSE_X_SIGN_MASK  0x10
#define MOUSE_Y_SIGN_MASK  0x20
#define MOUSE_START_BYTE_MASK 0x08

#define MOUSE_BUFFER_SIZE 300

#define MOUSE_SW_L_MASK  0x01
#define MOUSE_SW_R_MASK  0x02
#define MOUSE_SW_L_SIGN_DEL   0
#define MOUSE_SW_R_SIGN_DEL   1
#define MOUSE_ERROR_FLAG "PS2 device not found!"
#define MOUSE_FLAG "PS2 device found!"
//type Variable contexte non utilisée
typedef struct message_queue {
	volatile unsigned char messages[MSG_Q_SIZE][3];		//Tableau de messages
	volatile unsigned char read_addr;					//Addresse de lecture
	volatile unsigned char write_addr;					//Addresse d'ecriture
}message_queue_t;
//Declaration de la structure du contenant de variables de contexte
//type Variable contexte non utilisée
typedef struct context {
	volatile unsigned char reset_flag, ready;	//Drapeaux de statut du PS/2
	message_queue_t *message_q;					//Pointeur vers le tableau de message
	alt_up_ps2_dev *ps2;						//Pointeur vers le port PS/2
} context_t;
context_t ps2_context;

//états de lectures des données envoyé par la souris
typedef enum eMouseState
{
	START_BYTE_STATE,
	X_BYTE_STATE,
	Y_BYTE_STATE
}tMouseState;

//DECLARATIONS DES VARIABLES POUR LES FIFO
//FIFOs de position
U8  yPosBuffer[MOUSE_BUFFER_SIZE];
U16 xPosBuffer[MOUSE_BUFFER_SIZE];
//FIFOS d'états des boutons 
U8  swLeft[MOUSE_BUFFER_SIZE];
U8  swRight[MOUSE_BUFFER_SIZE];
//pointeurs in et out 
U16  ptrIn=0;
U16 ptrOut=0;

U16  nbBytes=0;
//
U8 lastSwitchRight=0;
//fanion d'écriture aux fifo
U8 writeTobuffer=1;

//variables machine a état
static volatile U8 mouseState=START_BYTE_STATE;
alt_up_ps2_dev *ps2Inst;

//variables lecture des valeurs souris
static volatile S8 signX;
static volatile S8 signY;
static volatile U8 swLeftState;
static volatile U8 swRightState;

//compteur de position x et y
static volatile U16 mousePosX;
static volatile U16 mousePosY;

/*
 * @fn 	 static void ps2_isr(void *context, alt_u32 id)
 * @des  routine d'interruption reception données souris
 * @arg  *context non utilisé, U32 id
 * @ret  Retourne rien.
 */
static void ps2_isr(void *context, U32 id)
{
	U8 data=0;
	if(alt_up_ps2_read_data_byte(ps2Inst, &data)==0 && nbBytes<MOUSE_BUFFER_SIZE)
	{
		switch(mouseState)
		{
		//START_STATE
		default:
		 if(data&MOUSE_START_BYTE_MASK)
		 {
			 signX=(data&MOUSE_X_SIGN_MASK);
			 signY=(data&MOUSE_Y_SIGN_MASK);
			 swRightState =(data&MOUSE_SW_R_MASK)>>MOUSE_SW_R_SIGN_DEL;
			 swLeftState=(data&MOUSE_SW_L_MASK)>>MOUSE_SW_L_SIGN_DEL;
			 if((lastSwitchRight==swRightState)&&(swRightState==MOUSE_SW_R_MASK))
			 {
				 writeTobuffer=0;

			 }
			 else
			 {
				 writeTobuffer=1;
			 }
			 mouseState=X_BYTE_STATE;
			 lastSwitchRight=swRightState;
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
			//Place les données dans des FIFO
			if(writeTobuffer)
			{
				yPosBuffer[ptrIn]=mousePosY;
				xPosBuffer[ptrIn]=mousePosX;
				swLeft[ptrIn]=swLeftState;
				swRight[ptrIn]=swRightState;
				ptrIn++;
				nbBytes++;

				if(ptrIn>MOUSE_BUFFER_SIZE-1)
					ptrIn=0;
			}

			mouseState=START_BYTE_STATE;
		break;
		}
	}


}
/*
 * @fn 	 void mouseInit(void)
 * @des  init de la souris et des interruption souris
 * @arg  void
 * @ret  Retourne rien.
 */
void mouseInit(void)
{

	ps2Inst=alt_up_ps2_open_dev(MOUSE_0_NAME);

	alt_irq_register(MOUSE_0_IRQ_INTERRUPT_CONTROLLER_ID,(void *) (&ps2_context),ps2_isr);
	alt_up_ps2_enable_read_interrupt(ps2Inst);

	if(ps2Inst == 0)jUartSendString(MOUSE_ERROR_FLAG);
	else if(ps2Inst != 0)jUartSendString(MOUSE_FLAG);

}
/*
 * @fn 	 U8 mouseGetNbEvent(void)
 * @des  retourne le nb de trames disponibles
 * @arg  void
 * @ret  U8 nBEvent.
 */
U8 mouseGetNbEvent(void)
{
	return nbBytes;
}
/*
 * @fn 	 void mousePtrOutInc(void)
 * @des  incremente le ptr de sortie du fifo
		 décrémente le nb de trames disponibles
		 doit etre appeler apres avoir récupéré
		 toutes les données souris 
 */
void mousePtrOutInc(void)
{
	ptrOut++;
	nbBytes--;
	if(ptrOut>MOUSE_BUFFER_SIZE-1)
		ptrOut=0;
}
/*
 * @fn 	 U16 mouseGetX(void)
 * @des  retourne la donnée de position X		 
 */
U16 mouseGetX(void)
{
	return xPosBuffer[ptrOut];
}
/*
 * @fn 	 U16 mouseGetY(void)
 * @des  retourne la donnée de position Y		 
 */
U16 mouseGetY(void)
{
	return (U16)yPosBuffer[ptrOut];
}
/*
 * @fn 	 U8 mouseGetSWL(void)
 * @des  retourne la donnée de bouton gauche		 
 */
U8 mouseGetSWL(void)
{
	return swLeft[ptrOut];
}
/*
 * @fn 	 U8 mouseGetSWR(void)
 * @des  retourne la donnée de bouton droit		 
 */
U8 mouseGetSWR(void)
{
	return swRight[ptrOut];
}
