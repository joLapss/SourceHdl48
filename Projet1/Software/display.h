/********************************************************************************************************

 Company: École de technologie supérieur
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
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma_regs.h"
#include "system.h"
#include "picture.h"
#include "keyboard.h"

#define DISABLED 0
#define ENABLED 1

#define WRITE 1
#define CLEAR 0
//Pointeur périphérique pixel buffer eyt char buffer
alt_up_pixel_buffer_dma_dev* pixelptr;
alt_up_char_buffer_dev* charbuff;


//void DrawImage(U8 x,U8 y,U16 height,U16 width);
// Variables globales
U16 xLastValue;
U16 yLastValue;



/*
 * @fn 	 void InitDisplay(void)
 * @des  Ouvre les périphérique pixel buffer et char buffer
 * @arg  N'a besoin d'aucun argument
 * @ret  Retourne rien.
 */
void InitDisplay(void);
void DrawImage(U8 x,U8 y,U16 height,U16 width,U16 *ptr);
void PlayNote(U8 note,U8 mode);
void SelectPianoMode(U8 Data);
/*
 * @fn 	 void NiosDrawApp(void)
 * @des  Exécute l'application NiosDrawv0.1.
 * 											- Clique gauche: Active le tracé de couleur (Rouge,Bleu,Jaune et Vert).
 * 											- Clique droite: Efface la zone de dessin
 * 											- Le fond d'écran est modifiable par le define: BACKGROUND (choix entre : Rouge,Bleu,Jaune et Vert).
 * 											- Cycle de rafraichissement de l'écran à 60 cycles par seconde.
 * 											- Envoie de la télémétrie sur le port Jtag. (Clique gauche, clique droit, coordonées x-y lors du clique)
 *
 * @arg  Ne prend aucun argument
 * @ret  Ne retourne pas de valeur
 */


#endif /* DISPLAY_H_ */
