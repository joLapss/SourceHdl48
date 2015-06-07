/********************************************************************************************************

 Company: �cole de technologie sup�rieur
 Engineer:
 		Kevin Parent Legault
		Jonathan Lapointe
 Create Date:  2015-06-02
 Module Name: display.h
 Project Name: lab2n
 Target Devices: Altera cyclone V SOPC
 Tool versions: NIOS II v14.1
 Description:
	

 Revision: v1
 Revision 0.01 - File Created
 Additional Comments:


*********************************************************************************************************/
#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "hardware.h"
#include "jtagUart.h"
#include "mouse.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma_regs.h"
#include "system.h"



//Pointeur p�riph�rique pixel buffer eyt char buffer
alt_up_pixel_buffer_dma_dev* pixelptr;
alt_up_char_buffer_dev* charbuff;

// Variables globales
U16 xLastValue;
U16 yLastValue;


/*
 * @fn 	 void InitDisplay(void)
 * @des  Ouvre les p�riph�rique pixel buffer et char buffer
 * @arg  N'a besoin d'aucun argument
 * @ret  Retourne rien.
 */
void InitDisplay(void);

/*
 * @fn 	 void NiosDrawApp(void)
 * @des  Ex�cute l'application NiosDrawv0.1.
 * 											- Clique gauche: Active le trac� de couleur (Rouge,Bleu,Jaune et Vert).
 * 											- Clique droite: Efface la zone de dessin
 * 											- Le fond d'�cran est modifiable par le define: BACKGROUND (choix entre : Rouge,Bleu,Jaune et Vert).
 * 											- Cycle de rafraichissement de l'�cran � 60 cycles par seconde.
 * 											- Envoie de la t�l�m�trie sur le port Jtag. (Clique gauche, clique droit, coordon�es x-y lors du clique)
 *
 * @arg  Ne prend aucun argument
 * @ret  Ne retourne pas de valeur
 */
void NiosDrawApp(void);

#endif /* DISPLAY_H_ */
