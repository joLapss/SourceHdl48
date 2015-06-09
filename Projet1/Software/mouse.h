/********************************************************************************************************
 Company: �cole de technologie sup�rieur
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
 * @fn 	 void mouseInit(void)
 * @des  init de la souris et des interruption souris
 * @arg  void
 * @ret  Retourne rien.
 */
void mouseInit(void);
/*
 * @fn 	 U8 mouseGetNbEvent(void)
 * @des  retourne le nb de trames disponibles
 * @arg  void
 * @ret  U8 nBEvent.
 */
U8 mouseGetNbEvent(void);
/*
 * @fn 	 U16 mouseGetY(void)
 * @des  retourne la donn�e de position Y		 
 */
U16 mouseGetY(void);


/*
 * @fn 	 U16 mouseGetX(void)
 * @des  retourne la donn�e de position X		 
 */
U16 mouseGetX(void);

/*
 * @fn 	 U8 mouseGetSWL(void)
 * @des  retourne la donn�e de bouton gauche		 
 */
U8 mouseGetSWL(void);

/*
 * @fn 	 U8 mouseGetSWR(void)
 * @des  retourne la donn�e de bouton droit		 
 */
U8 mouseGetSWR(void);

/*
 * @fn 	 void mousePtrOutInc(void)
 * @des  incremente le ptr de sortie du fifo
		 d�cr�mente le nb de trames disponibles
		 doit etre appeler apres avoir r�cup�r�
		 toutes les donn�es souris 
 */
void mousePtrOutInc(void);
