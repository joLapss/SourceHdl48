/********************************************************************************************************

 Company: �cole de technologie sup�rieur
 Engineer:
 		Kevin Parent Legault
		Jonathan Lapointe
 Create Date:  2015-06-02
 Module Name: display.c
 Project Name: lab2n
 Target Devices: Altera cyclone V SOPC
 Tool versions: NIOS II v14.1
 Description:
	Module d'affichage permettant l'ex�cution de l'application NiosDraw, le d�placement d'un curseur sur un �cran VGA d'une
	r�solution de 320x240 redimensionn� pour 640x480 et ex�cut� un trac� de couleur � l'�cran.
	Le module poss�de �galement une fonction permettant d'afficher sur la console des informations t�l�m�triques tel que :
				- D�tection d'un clique droit et coordonn�es x-y lors du clique.
				- D�tection d'un clique gauche et coordonn�es x-y lors du clique
	Le module affiche � l'�cran la position du curseur.
	
	Poss�de les fonctions suivantes :
		1. void InitDisplay(void);
		2. void NiosDrawApp(void);
		3. U8 DrawPixelColored(U16 x,U16 y)
		4. void CleanDrawZone(void)
		5. U8 UpdateCursorPosition(U16 x, U16 y)
		6. void DisplayCoordinate(U16 x, U16 y)
		7. void SendTelemetry(U16 x, U16 y, U8 FlagR, U8 FlagL)


 Revision: v1
 Revision 0.01 - File Created
 Additional Comments:
	N/A
*********************************************************************************************************/




#include "display.h"
//D�finition des symbols pour curseur et la conversion en caract�re
#define CURSOR 0x2
#define ERASE_LAST 0x20
#define ACSII_CONVERSION 0x30

//D�finition des couleurs de fond d'�cran et de traceur
#define BLUE 0x187F
#define GREEN 0x0F00
#define YELLOW 0xFF00
#define RED 0xF000
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BACKGROUND BLACK
//Definition des dimensions de l'�cran et de l'espace dessin
#define DISPLAY_RES_WIDTH  319
#define DISPLAY_RES_HEIGHT 239
#define SCALING_FACTOR_X 4  //   319/80
#define SCALING_FACTOR_Y 4  // 229/57
#define DISPLAY_RES_ORIGIN 0
#define DRAW_LIMIT_HEIGHT_MIN 10
#define DRAW_LIMIT_HEIGHT_MAX 229

//Flags
#define DRAW_LIMIT_EXCEEDED 1
#define OK 0
//Definition des coordonn�es de positionnement des �tiquettes
#define LABEL_POSITION_WIDTH_TOP 10
#define LABEL_POSITION_HEIGHT_TOP 1
#define COORDINATE_X_POSITION_WIDTH 58
#define COORDINATE_X_POSITION_HEIGHT 58
#define COORDINATE_Y_POSITION_WIDTH 63
#define COORDINATE_Y_POSITION_HEIGHT 58
#define X_SHIFT 2
#define Y_SHIFT 2
//D�finition d'�tiquettes
#define X "X:"
#define Y "Y:"
#define LABEL "NiosDraw V0.1 - Kevin Parent Legault & Jonathan Lapointe"

//Initialisation des variables
U8 init=1;
U16 xValue=0;
U16 yValue=0;
U16 xTempo=0;
U16 yTempo=0;
U8 updatePositionFlag=0;
U8 TelemetryFlagButtonL=1;
U8 TelemetryFlagButtonR=1;
U8 colorInc=0;
U16 colorSwap[4]={BLUE,GREEN,YELLOW,RED};


//Function declaration

/*
 * @fn 	 DrawPixel(U16 x, U16 y,U32 color)
 * @des  Affiche un pixel de la couleur d�sir� aux coordonn�s x-y sp�cifiez
 * @arg  U16 x , U16 y, U32 color
 * @ret  Retourne 1 si la zone de dessin est d�pass�.
 */
U8 DrawPixelColored(U16 x,U16 y);

/*
 * @fn 	 CleanDrawZone
 * @des  Nettoie la zone de dessin en remplissant la zone de la couleur BACKGROUND.
 * @arg  Ne prend aucun argument.
 * @ret  Ne retourne aucune valeur.
 */
void CleanDrawZone(void);

/*
 * @fn 	 UpdateCursorPosition(U16 x,U16 y)
 * @des  Met � jour l'emplacement du curseur en effacant l'ancienne emplacement
 * @arg U16 x : Coordonn�es en x. U16 y : Coordonn�es en y
 * @ret Retourne 1 si les limites de la zone de dessin sont d�pass�s.
 */
U8 UpdateCursorPosition(U16 x, U16 y);

/*
 * @fn 	 DisplayCoordinate(U16 x, U16 y)
 * @des  Affiche � l'�cran les coordonn�es du pointeur aux positions suivantes :
 * 		 Coordonn�e en x : Width = de 58 � 62 et Height = 58
 * 		 Coordonn�e en y : Width = de 63 � 67 et Height = 58
 * @arg  U16 x : Coordonn�es en x et U16 y : Coordonn�es en Y.
 * @ret  Ne retourne rien
 */
void DisplayCoordinate(U16 x, U16 y);

/*
 * @fn 	 SendTelemetry(U16 x, U16 y, U8 FlagR,U8 FlagL)
 * @des  Envoie les donn�es t�l�m�triques tel que : Lorsqu'un bouton est appuy� et les coordonn�es au moment de l'action.
 * @arg  U16 x : Coordonn�s en x � envoyer.
 * 		 U16 y : Coordonn�s en y � envoyer.
 * 		 U8  FlagR : Permet l'envoie sur le port jtag, lorsque FlagR = 1.
 * 		 U8  FlagL : Permet l'envoie sur le port jtag, lorsque FlagL = 1.
 * @ret  Ne retourne aucune donn�e.
 */
void SendTelemetry(U16 x, U16 y, U8 FlagR, U8 FlagL);

/*
 * @fn 	 void InitDisplay(void)
 * @des  Ouvre les p�riph�rique pixel buffer et char buffer
 * @arg  N'a besoin d'aucun argument
 * @ret  Retourne rien.
 */
void InitDisplay(void)
{
	pixelptr=alt_up_pixel_buffer_dma_open_dev(PIXEL_BUFFER_NAME); // Initialise pixel buffer
	charbuff = alt_up_char_buffer_open_dev("/dev/character_buffer"); // Initialise char buffer
}
/*
 * @fn 	 DrawPixel(U16 x, U16 y,U32 color)
 * @des  Affiche un pixel de la couleur d�sir� aux coordonn�s x-y sp�cifiez
 * @arg  U16 x , U16 y, U32 color
 * @ret  Retourne 1 si la zone de dessin est d�pass�.
 */
U8 DrawPixelColored(U16 x,U16 y)
{
	U16 val=0;
	if(colorInc > 3)colorInc = 0; // r�initialise la boucle � z�ro
	if(y < DRAW_LIMIT_HEIGHT_MIN  || y > DRAW_LIMIT_HEIGHT_MAX )
	{
		val = DRAW_LIMIT_EXCEEDED;
	}
	else
	{
		val = OK;
		alt_up_pixel_buffer_dma_draw(pixelptr,colorSwap[colorInc++],x,y); // Active un pixel aux coordonn�es x-y.
	}
return val;
}
/*
 * @fn 	 CleanDrawZone
 * @des  Nettoie la zone de dessin en remplissant la zone de la couleur BACKGROUND.
 * @arg  Ne prend aucun argument.
 * @ret  Ne retourne aucune valeur.
 */
void CleanDrawZone(void)
{
	alt_up_pixel_buffer_dma_draw_box(pixelptr,DISPLAY_RES_ORIGIN,DRAW_LIMIT_HEIGHT_MIN, DISPLAY_RES_WIDTH, DRAW_LIMIT_HEIGHT_MAX,BACKGROUND, 0);
}
/*
 * @fn 	 UpdateCursorPosition(U16 x,U16 y)
 * @des  Met � jour l'emplacement du curseur en effacant l'ancienne emplacement
 * @arg U16 x : Coordonn�es en x. U16 y : Coordonn�es en y
 * @ret Retourne 1 si les limites de la zone de dessin sont d�pass�s.
 */
U8 UpdateCursorPosition(U16 x,U16 y)
{
	U16 val=0;
	if(y < DRAW_LIMIT_HEIGHT_MIN  || y > DRAW_LIMIT_HEIGHT_MAX )val = DRAW_LIMIT_EXCEEDED;
	else
	{
		val = OK;
		alt_up_char_buffer_draw(charbuff,ERASE_LAST,xLastValue>>2,yLastValue>>2); // Efface l'ancien curseur et mise � l'�chelle de la position x-y du curseur en fonction de la r�solution de l'�cran
		alt_up_char_buffer_draw(charbuff,CURSOR,x>>2,y>>2); // Dessine le curseur � la nouvelle position et mise � l'�chelle de la positio xy du curseur en fonction de la r�solution de l'�cran.

		xLastValue = x;
		yLastValue = y;
	}
return val;
}

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
void NiosDrawApp(void)
{
	//Initialisation du panneau de dessin.
	if(init)
	{
		alt_up_char_buffer_clear(charbuff);
		alt_up_pixel_buffer_dma_draw_box(pixelptr,DISPLAY_RES_ORIGIN,DRAW_LIMIT_HEIGHT_MIN, DISPLAY_RES_WIDTH, DRAW_LIMIT_HEIGHT_MAX,BACKGROUND, 0);
		alt_up_char_buffer_string(charbuff,LABEL,LABEL_POSITION_WIDTH_TOP, LABEL_POSITION_HEIGHT_TOP); // Envoie des caract�res pour la banni�re du haut avec les defines associ�s.
		alt_up_char_buffer_string(charbuff,X,COORDINATE_X_POSITION_WIDTH,COORDINATE_X_POSITION_HEIGHT);// Envoie des caract�res pour la banni�re en bas � droite avec les defines associ�s.
		alt_up_char_buffer_string(charbuff,Y,COORDINATE_Y_POSITION_WIDTH,COORDINATE_Y_POSITION_HEIGHT);
		alt_up_pixel_buffer_dma_swap_buffers(pixelptr); // Rotation du buffer. Attente du cycle de rafraichissement
	init = 0;
	}

	if(alt_up_pixel_buffer_dma_check_swap_buffers_status(pixelptr) == 0) // V�rifie que le buffer estb compl�tement vide. L'�cran VGA est donc rafraichie.
	{

		while(mouseGetNbEvent()) // Tourne tant que le FIFO de la souris n'est pas vide. (FIFO : Zone tampon stockant les valeurs de d�placement de la souris. Cela nous permet de dessin� plus de point)
		{
				updatePositionFlag = 1;
				xValue = mouseGetX(); // Stockage de la coordonn�e de la souris en x
				yValue = mouseGetY(); // Stockage de la coordonn�e de la souris en y
				xTempo=xValue;
				yTempo=yValue;
				if(mouseGetSWL()) //V�rifie si le bouton gauche est appuy�.
				{
					DrawPixelColored(xValue,yValue); // Dessine un pixel d'une couleur.
					SendTelemetry(xTempo,yTempo,0,TelemetryFlagButtonL);//Envoie des donn�es t�l�m�triques lorsque le flag est a 1.
					TelemetryFlagButtonL=0; // Le flag boutton gauche � 0.
				}
				else if(mouseGetSWR())
				{
					if(TelemetryFlagButtonR)CleanDrawZone(); // Nettoyage de la zone de dessin.
					SendTelemetry(xTempo,yTempo,TelemetryFlagButtonR,0); // Envoie des donn�es t�l�m�triques lorsque le flag est a 1
					TelemetryFlagButtonR=0;
				}
				if(mouseGetSWL()==0)TelemetryFlagButtonL=1; //L�ve le Flag pour envoie de la t�l�m�trie du bouton gauche
				if(mouseGetSWR()==0)TelemetryFlagButtonR=1; //L�ve le Flag pour envoie de la t�l�m�trie du bouton droit
				mousePtrOutInc(); // Incr�ment de l'index du FIFO souris.
				alt_up_pixel_buffer_dma_swap_buffers(pixelptr); // rotation du pixel buffer.D�lai de rafraichissement de l'�cran.
		}

		if(updatePositionFlag == 1) // Mis � jour des coordonn�es et d�placement du curseur.
		{
			DisplayCoordinate(xTempo, yTempo);
			UpdateCursorPosition(xValue,yValue);
			updatePositionFlag = 0;
		}
	}
}

/*
 * @fn 	 DisplayCoordinate(U16 x, U16 y)
 * @des  Affiche � l'�cran les coordonn�es du pointeur aux positions suivantes :
 * 		 Coordonn�e en x : Width = de 58 � 62 et Height = 58
 * 		 Coordonn�e en y : Width = de 63 � 67 et Height = 58
 * @arg  U16 x : Coordonn�es en x et U16 y : Coordonn�es en Y.
 * @ret  Ne retourne rien
 */
void DisplayCoordinate(U16 x, U16 y)
{
	U8 valueConv[4];
	U16 xt;
	U16 yt;
	S8 i=0;
	valueConv[3]=0;
	//Mise � l'�chelle pour une r�solution de 640x480
	xt = x<<1;
	yt = y<<1;
		//Conversion des valeurs U8 en caract�re ascii correspondant pour x
		for(i=2;i>=0;i--)
		{
			valueConv[i]=(xt %10)+0x30;
			xt=xt/10;
		}
		alt_up_char_buffer_string(charbuff,valueConv,(COORDINATE_X_POSITION_WIDTH + X_SHIFT),COORDINATE_X_POSITION_HEIGHT); //Envoie de la
		//Conversion des valeurs U8 en caract�re ascii correspondant pour y
		for(i=2;i>=0;i--)
		{
			valueConv[i]=(yt %10)+0x30;
			yt=yt/10;
		}
		alt_up_char_buffer_string(charbuff,valueConv,COORDINATE_Y_POSITION_WIDTH+Y_SHIFT, COORDINATE_Y_POSITION_HEIGHT);//envoie cha�ne de caract�re avec fonction Hal.

}
/*
 * @fn 	 SendTelemetry(U16 x, U16 y, U8 FlagR,U8 FlagL)
 * @des  Envoie les donn�es t�l�m�triques tel que : Lorsqu'un bouton est appuy� et les coordonn�es au moment de l'action.
 * @arg  U16 x : Coordonn�s en x � envoyer.
 * 		 U16 y : Coordonn�s en y � envoyer.
 * 		 U8  FlagR : Permet l'envoie sur le port jtag, lorsque FlagR = 1.
 * 		 U8  FlagL : Permet l'envoie sur le port jtag, lorsque FlagL = 1.
 * @ret  Ne retourne aucune donn�e.
 */
void SendTelemetry(U16 x, U16 y, U8 FlagR,U8 FlagL)
{

	//Variables locaux
	U8 valueConvx[4],valueConvy[4];
	S8 i=0;
	valueConvx[3]=0;
	valueConvy[3]=0;
	U16 xt;
	U16 yt;

	//Mise � l'�chelle pour une r�soltion de 640x480
	xt = (x << 1)+2;
	yt = (y << 1)+2;

	//Conversion des valeurs en x en caract�re ascii.
	for(i=2;i>=0;i--)
	{
		valueConvx[i]=(xt %10)+0x30;
		xt=xt/10;
	}
	//Conversion des valeurs en y en caract�re ascii.
	for(i=2;i>=0;i--)
	{
		valueConvy[i]=(yt %10)+0x30;
		yt=yt/10;
	}
	//Envoie sur le port Jtag des informations d'�tat concernant le bouton de droite.
	if(FlagR)
	{

		jUartSendString("Right button pressed!\n"); // Envoie une chaine de caract�re

		jUartSendString("X :  ");
		jUartSendString(valueConvx); // Envoie des coordonn�es en x
		jUartSendString(" ");        // Espacement

		jUartSendString("Y :  ");
		jUartSendString(valueConvy); // Envoie des coordonn�es en y
		jUartSendString("\n ");      // Saut de ligne

	}
	//Envoie sur le port Jtag des informations d'�tat concernant le bouton de gauche.
	if(FlagL)
	{

		jUartSendString("Left button pressed!\n");
		jUartSendString("X :  ");
		jUartSendString(valueConvx);	// Envoie des coordonn�es en x
		jUartSendString(" ");			// Espacement

		jUartSendString("Y :  ");
		jUartSendString(valueConvy);	// Envoie des coordon�es en y
		jUartSendString("\n ");			// Saut de ligne

	}
}


