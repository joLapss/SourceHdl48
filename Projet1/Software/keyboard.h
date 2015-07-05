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


/*
 * @fn 	 void kbInit(void);
 * @des  init du key et des interruption souris
 * @arg  void
 * @ret  Retourne rien.
 */
void kbInit(void);
/*
 * @fn 	 U8 kBGetEvent(void);
 * @des  retourne le nb de trames disponibles
 * @arg  void
 * @ret  U8 nBEvent.
 */
U8 kBGetEvent(void);
///Notes corespondantes aux chiffres
#define KB_KEY_DO     0
#define KB_KEY_RE     1
#define KB_KEY_Mi     2
#define KB_KEY_FA     3
#define KB_KEY_SOL    4
#define KB_KEY_LA     5
#define KB_KEY_SI     6
#define KB_KEY_DO     7
#define KB_KEY_DO_D   8
#define KB_KEY_RE_D   9
#define KB_KEY_FA_D   10
#define KB_KEY_SOL_D  11
#define KB_KEY_LA_D   12
#define KB_KEY_DO_D   13
#define KB_KEY_SAMPLE 14
#define KB_KEY_PLAY   15

/*
 * @fn 	 U8 kBGetKey(void);
 * @des  retourne la touche appuyer
 * @arg  void
 * @ret  U8 touche voir definition en haut
 */
U8 kBGetKey(void);

#define KB_DISABLE_INT   0
#define KB_ENABLE_INT    1
/*
 * @fn 	 void kbInterruptState(U8 state);
 * @des  active ou desactive les interrupt du Keyboard
 * @arg  state voir define plus haut
 */
void kbInterruptState(U8 state);

