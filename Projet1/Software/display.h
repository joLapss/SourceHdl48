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
/*
 * @fn 	 DrawImage(U8 x,U8 y,U16 height,U16 width,U16 *ptr)
 * @des  Dessine une petite image stocker à une adresse spécifique. Scan du de gauche à droite et du haut vers le bas.
 * @arg  U8 x : Coordonnés en x0 de l'image à afficher.
 * 		 U8 y : Coordonnés en y0 de l'image à afficher.
 * 		 U16 height : à déterminer. spécifiez les dimensions de l'image à afficher. Peut être croppé si on le désire
 * 		 U16 width  : à déterminer. Le positionnement doit être impéccable pour ce paramètre. "sensitive case". Peut déformé l'image ou même les couleurs
 * 		 U16 *ptr : Pointeur d'adresse pour la lecture du tableau inculant le code rgb pour chaque pixel de l'image.
 * @ret  Ne retourne aucune donnée.
 */
void DrawImage(U8 x,U8 y,U16 height,U16 width,U16 *ptr);
/*
 * @fn 	 PlayNote(U8 data,U8 mode)
 * @des  Sélectionne à l'écran la note joué et affiche à l'écran le caractère de la dernière note affiché
 * @arg  U8 data : data provenant des touches du clavier.
 * 		 U8 mode : 2 modes soit : write et clear. Write : Dessine la note passé en argument. Clear: Efface la note passé en argument
 * @ret  Ne retourne aucune donnée.
 */
void PlayNote(U8 note,U8 mode);
/*
 * @fn 	 void SelectPianoMode(U8 Data)
 * @des  Inscrit sur le panneau d'affichage si on est en mode enregistrement ou en mode écoute.
 * @arg  U8 data : Reçoit les touches du clavier et réagit aux modes Enregistrement et Écoute.
 * @ret  Ne retourne aucune donnée.
 */
void SelectPianoMode(U8 Data);



#endif /* DISPLAY_H_ */
