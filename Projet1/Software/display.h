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
//Pointeur p�riph�rique pixel buffer eyt char buffer
alt_up_pixel_buffer_dma_dev* pixelptr;
alt_up_char_buffer_dev* charbuff;


//void DrawImage(U8 x,U8 y,U16 height,U16 width);
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
 * @fn 	 DrawImage(U8 x,U8 y,U16 height,U16 width,U16 *ptr)
 * @des  Dessine une petite image stocker � une adresse sp�cifique. Scan du de gauche � droite et du haut vers le bas.
 * @arg  U8 x : Coordonn�s en x0 de l'image � afficher.
 * 		 U8 y : Coordonn�s en y0 de l'image � afficher.
 * 		 U16 height : � d�terminer. sp�cifiez les dimensions de l'image � afficher. Peut �tre cropp� si on le d�sire
 * 		 U16 width  : � d�terminer. Le positionnement doit �tre imp�ccable pour ce param�tre. "sensitive case". Peut d�form� l'image ou m�me les couleurs
 * 		 U16 *ptr : Pointeur d'adresse pour la lecture du tableau inculant le code rgb pour chaque pixel de l'image.
 * @ret  Ne retourne aucune donn�e.
 */
void DrawImage(U8 x,U8 y,U16 height,U16 width,U16 *ptr);
/*
 * @fn 	 PlayNote(U8 data,U8 mode)
 * @des  S�lectionne � l'�cran la note jou� et affiche � l'�cran le caract�re de la derni�re note affich�
 * @arg  U8 data : data provenant des touches du clavier.
 * 		 U8 mode : 2 modes soit : write et clear. Write : Dessine la note pass� en argument. Clear: Efface la note pass� en argument
 * @ret  Ne retourne aucune donn�e.
 */
void PlayNote(U8 note,U8 mode);
/*
 * @fn 	 void SelectPianoMode(U8 Data)
 * @des  Inscrit sur le panneau d'affichage si on est en mode enregistrement ou en mode �coute.
 * @arg  U8 data : Re�oit les touches du clavier et r�agit aux modes Enregistrement et �coute.
 * @ret  Ne retourne aucune donn�e.
 */
void SelectPianoMode(U8 Data);



#endif /* DISPLAY_H_ */
