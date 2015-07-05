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
#include "keyboard.h"
#include "sys/alt_irq.h"
#include "altera_up_avalon_ps2.h"
#include "jtagUart.h"


#define KB_START_BYTE	   0xf0
#define MSG_Q_SIZE	   	   4
#define KB_MAX_KEYS		   16


#define KB_ERROR_FLAG "PS2 device not found!"
#define KB_FLAG 	  "PS2 device found!"

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
typedef enum eKbState
{
	START_BYTE_STATE,
	KB_KEY
}tKbState;


U8  kbDataBuffer=0;
//  Tableau d'assignations des valeurs retourne par le clavier
U8  kbKeyDecode[]={0x1C,0x1B,0x23,0x2b,0x34,0x33,0x3B,0x42,0x1d,0x24,0x2d,0x2c,0x35,0x3c,0x46,0x45};

//variables machine a état
static volatile U8 KBState=START_BYTE_STATE;
alt_up_ps2_dev *ps2Inst;

//variables lecture des valeurs souris
static volatile U8 kbDataFlag=0;


/*
 * @fn 	 static void ps2_isr(void *context, alt_u32 id)
 * @des  routine d'interruption reception données souris
 * @arg  *context non utilisé, U32 id
 * @ret  Retourne rien.
 */
static void ps2_isr(void *context, U32 id)
{
	U8 data=0;
	if(alt_up_ps2_read_data_byte(ps2Inst, &data)==0)
	{
		switch(KBState)
		{
		//START_STATE
		default:
			 if(data==KB_START_BYTE)
						 KBState=KB_KEY;
		break;

		case KB_KEY:
			kbDataBuffer=data;
			kbDataFlag=1;
			KBState=START_BYTE_STATE;
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
void kbInit(void)
{

	ps2Inst=alt_up_ps2_open_dev(PS2_CONTROLLER_NAME);
	alt_irq_register(PS2_CONTROLLER_IRQ_INTERRUPT_CONTROLLER_ID,(void *) (&ps2_context),ps2_isr);
	alt_up_ps2_enable_read_interrupt(ps2Inst);
	if(ps2Inst == 0)
		jUartSendString((U8*)KB_ERROR_FLAG);
	else if(ps2Inst != 0)
		jUartSendString((U8*)KB_FLAG);

}
/*
 * @fn 	 void kbInterruptState(U8 state)
 * @des  active/desactive les interrupt du kb
 * @arg  state
 * @ret  rien
 */
void kbInterruptState(U8 state)
{
	if(state)
		alt_up_ps2_enable_read_interrupt(ps2Inst);
	else
		alt_up_ps2_disable_read_interrupt(ps2Inst);
}
/*
 * @fn 	 U8 mouseGetNbEvent(void)
 * @des  retourne le flag d'evenement
 * @arg  void
 * @ret  U8 nBEvent.
 */
U8 kBGetEvent(void)
{
	U8 flagValue=kbDataFlag;
	kbDataFlag=0;
	return flagValue;
}

/*
 * @fn 	 U8 kBGetKey(void)
 * @des  retourne la derniere touche  appuyée
 * @arg  void
 * @ret  U8 nBEvent.
 */
U8 kBGetKey(void)
{
	U8 i=0;
	while((kbDataBuffer!=kbKeyDecode[i])&&(KB_MAX_KEYS>i))
		i++;
	return i;
}

