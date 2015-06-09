/********************************************************************************************************

 Company: École de technologie supérieur
 Engineer:
 		Kevin Parent Legault
		Jonathan Lapointe
 Create Date:  2015-06-02
 Module Name: display.c
 Project Name: lab2n
 Target Devices: Altera cyclone V SOPC
 Tool versions: NIOS II v14.1
 Description:
	Module d'affichage permettant l'exécution de l'application NiosDraw, le déplacement d'un curseur sur un écran VGA d'une
	résolution de 320x240 redimensionné pour 640x480 et exécuté un tracé de couleur à l'écran.
	Le module possède également une fonction permettant d'afficher sur la console des informations télémétriques tel que :
				- Détection d'un clique droit et coordonnées x-y lors du clique.
				- Détection d'un clique gauche et coordonnées x-y lors du clique
	Le module affiche à l'écran la position du curseur.
	
	Possède les fonctions suivantes :
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
//Définition des symbols pour curseur et la conversion en caractère
#define CURSOR 0x2
#define ERASE_LAST 0x20
#define ACSII_CONVERSION 0x30

//Définition des couleurs de fond d'écran et de traceur
#define BLUE 0x187F
#define GREEN 0x0F00
#define YELLOW 0xFF00
#define RED 0xF000
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BACKGROUND BLACK
//Definition des dimensions de l'écran et de l'espace dessin
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
//Definition des coordonnées de positionnement des étiquettes
#define LABEL_POSITION_WIDTH_TOP 10
#define LABEL_POSITION_HEIGHT_TOP 1
#define COORDINATE_X_POSITION_WIDTH 58
#define COORDINATE_X_POSITION_HEIGHT 58
#define COORDINATE_Y_POSITION_WIDTH 63
#define COORDINATE_Y_POSITION_HEIGHT 58
#define X_SHIFT 2
#define Y_SHIFT 2
//Définition d'étiquettes
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
 * @des  Affiche un pixel de la couleur désiré aux coordonnés x-y spécifiez
 * @arg  U16 x , U16 y, U32 color
 * @ret  Retourne 1 si la zone de dessin est dépassé.
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
 * @des  Met à jour l'emplacement du curseur en effacant l'ancienne emplacement
 * @arg U16 x : Coordonnées en x. U16 y : Coordonnées en y
 * @ret Retourne 1 si les limites de la zone de dessin sont dépassés.
 */
U8 UpdateCursorPosition(U16 x, U16 y);

/*
 * @fn 	 DisplayCoordinate(U16 x, U16 y)
 * @des  Affiche à l'écran les coordonnées du pointeur aux positions suivantes :
 * 		 Coordonnée en x : Width = de 58 à 62 et Height = 58
 * 		 Coordonnée en y : Width = de 63 à 67 et Height = 58
 * @arg  U16 x : Coordonnées en x et U16 y : Coordonnées en Y.
 * @ret  Ne retourne rien
 */
void DisplayCoordinate(U16 x, U16 y);

/*
 * @fn 	 SendTelemetry(U16 x, U16 y, U8 FlagR,U8 FlagL)
 * @des  Envoie les données télémétriques tel que : Lorsqu'un bouton est appuyé et les coordonnées au moment de l'action.
 * @arg  U16 x : Coordonnés en x à envoyer.
 * 		 U16 y : Coordonnés en y à envoyer.
 * 		 U8  FlagR : Permet l'envoie sur le port jtag, lorsque FlagR = 1.
 * 		 U8  FlagL : Permet l'envoie sur le port jtag, lorsque FlagL = 1.
 * @ret  Ne retourne aucune donnée.
 */
void SendTelemetry(U16 x, U16 y, U8 FlagR, U8 FlagL);

/*
 * @fn 	 void InitDisplay(void)
 * @des  Ouvre les périphérique pixel buffer et char buffer
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
 * @des  Affiche un pixel de la couleur désiré aux coordonnés x-y spécifiez
 * @arg  U16 x , U16 y, U32 color
 * @ret  Retourne 1 si la zone de dessin est dépassé.
 */
U8 DrawPixelColored(U16 x,U16 y)
{
	U16 val=0;
	if(colorInc > 3)colorInc = 0; // réinitialise la boucle à zéro
	if(y < DRAW_LIMIT_HEIGHT_MIN  || y > DRAW_LIMIT_HEIGHT_MAX )
	{
		val = DRAW_LIMIT_EXCEEDED;
	}
	else
	{
		val = OK;
		alt_up_pixel_buffer_dma_draw(pixelptr,colorSwap[colorInc++],x,y); // Active un pixel aux coordonnées x-y.
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
 * @des  Met à jour l'emplacement du curseur en effacant l'ancienne emplacement
 * @arg U16 x : Coordonnées en x. U16 y : Coordonnées en y
 * @ret Retourne 1 si les limites de la zone de dessin sont dépassés.
 */
U8 UpdateCursorPosition(U16 x,U16 y)
{
	U16 val=0;
	if(y < DRAW_LIMIT_HEIGHT_MIN  || y > DRAW_LIMIT_HEIGHT_MAX )val = DRAW_LIMIT_EXCEEDED;
	else
	{
		val = OK;
		alt_up_char_buffer_draw(charbuff,ERASE_LAST,xLastValue>>2,yLastValue>>2); // Efface l'ancien curseur et mise à l'échelle de la position x-y du curseur en fonction de la résolution de l'écran
		alt_up_char_buffer_draw(charbuff,CURSOR,x>>2,y>>2); // Dessine le curseur à la nouvelle position et mise à l'échelle de la positio xy du curseur en fonction de la résolution de l'écran.

		xLastValue = x;
		yLastValue = y;
	}
return val;
}

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
void NiosDrawApp(void)
{
	//Initialisation du panneau de dessin.
	if(init)
	{
		alt_up_char_buffer_clear(charbuff);
		alt_up_pixel_buffer_dma_draw_box(pixelptr,DISPLAY_RES_ORIGIN,DRAW_LIMIT_HEIGHT_MIN, DISPLAY_RES_WIDTH, DRAW_LIMIT_HEIGHT_MAX,BACKGROUND, 0);
		alt_up_char_buffer_string(charbuff,LABEL,LABEL_POSITION_WIDTH_TOP, LABEL_POSITION_HEIGHT_TOP); // Envoie des caractères pour la bannière du haut avec les defines associés.
		alt_up_char_buffer_string(charbuff,X,COORDINATE_X_POSITION_WIDTH,COORDINATE_X_POSITION_HEIGHT);// Envoie des caractères pour la bannière en bas à droite avec les defines associés.
		alt_up_char_buffer_string(charbuff,Y,COORDINATE_Y_POSITION_WIDTH,COORDINATE_Y_POSITION_HEIGHT);
		alt_up_pixel_buffer_dma_swap_buffers(pixelptr); // Rotation du buffer. Attente du cycle de rafraichissement
	init = 0;
	}

	if(alt_up_pixel_buffer_dma_check_swap_buffers_status(pixelptr) == 0) // Vérifie que le buffer estb complètement vide. L'écran VGA est donc rafraichie.
	{

		while(mouseGetNbEvent()) // Tourne tant que le FIFO de la souris n'est pas vide. (FIFO : Zone tampon stockant les valeurs de déplacement de la souris. Cela nous permet de dessiné plus de point)
		{
				updatePositionFlag = 1;
				xValue = mouseGetX(); // Stockage de la coordonnée de la souris en x
				yValue = mouseGetY(); // Stockage de la coordonnée de la souris en y
				xTempo=xValue;
				yTempo=yValue;
				if(mouseGetSWL()) //Vérifie si le bouton gauche est appuyé.
				{
					DrawPixelColored(xValue,yValue); // Dessine un pixel d'une couleur.
					SendTelemetry(xTempo,yTempo,0,TelemetryFlagButtonL);//Envoie des données télémétriques lorsque le flag est a 1.
					TelemetryFlagButtonL=0; // Le flag boutton gauche à 0.
				}
				else if(mouseGetSWR())
				{
					if(TelemetryFlagButtonR)CleanDrawZone(); // Nettoyage de la zone de dessin.
					SendTelemetry(xTempo,yTempo,TelemetryFlagButtonR,0); // Envoie des données télémètriques lorsque le flag est a 1
					TelemetryFlagButtonR=0;
				}
				if(mouseGetSWL()==0)TelemetryFlagButtonL=1; //Lève le Flag pour envoie de la télémétrie du bouton gauche
				if(mouseGetSWR()==0)TelemetryFlagButtonR=1; //Lève le Flag pour envoie de la télémétrie du bouton droit
				mousePtrOutInc(); // Incrément de l'index du FIFO souris.
				alt_up_pixel_buffer_dma_swap_buffers(pixelptr); // rotation du pixel buffer.Délai de rafraichissement de l'écran.
		}

		if(updatePositionFlag == 1) // Mis à jour des coordonnées et déplacement du curseur.
		{
			DisplayCoordinate(xTempo, yTempo);
			UpdateCursorPosition(xValue,yValue);
			updatePositionFlag = 0;
		}
	}
}

/*
 * @fn 	 DisplayCoordinate(U16 x, U16 y)
 * @des  Affiche à l'écran les coordonnées du pointeur aux positions suivantes :
 * 		 Coordonnée en x : Width = de 58 à 62 et Height = 58
 * 		 Coordonnée en y : Width = de 63 à 67 et Height = 58
 * @arg  U16 x : Coordonnées en x et U16 y : Coordonnées en Y.
 * @ret  Ne retourne rien
 */
void DisplayCoordinate(U16 x, U16 y)
{
	U8 valueConv[4];
	U16 xt;
	U16 yt;
	S8 i=0;
	valueConv[3]=0;
	//Mise à l'échelle pour une résolution de 640x480
	xt = x<<1;
	yt = y<<1;
		//Conversion des valeurs U8 en caractère ascii correspondant pour x
		for(i=2;i>=0;i--)
		{
			valueConv[i]=(xt %10)+0x30;
			xt=xt/10;
		}
		alt_up_char_buffer_string(charbuff,valueConv,(COORDINATE_X_POSITION_WIDTH + X_SHIFT),COORDINATE_X_POSITION_HEIGHT); //Envoie de la
		//Conversion des valeurs U8 en caractère ascii correspondant pour y
		for(i=2;i>=0;i--)
		{
			valueConv[i]=(yt %10)+0x30;
			yt=yt/10;
		}
		alt_up_char_buffer_string(charbuff,valueConv,COORDINATE_Y_POSITION_WIDTH+Y_SHIFT, COORDINATE_Y_POSITION_HEIGHT);//envoie chaîne de caractère avec fonction Hal.

}
/*
 * @fn 	 SendTelemetry(U16 x, U16 y, U8 FlagR,U8 FlagL)
 * @des  Envoie les données télémétriques tel que : Lorsqu'un bouton est appuyé et les coordonnées au moment de l'action.
 * @arg  U16 x : Coordonnés en x à envoyer.
 * 		 U16 y : Coordonnés en y à envoyer.
 * 		 U8  FlagR : Permet l'envoie sur le port jtag, lorsque FlagR = 1.
 * 		 U8  FlagL : Permet l'envoie sur le port jtag, lorsque FlagL = 1.
 * @ret  Ne retourne aucune donnée.
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

	//Mise à l'échelle pour une résoltion de 640x480
	xt = (x << 1)+2;
	yt = (y << 1)+2;

	//Conversion des valeurs en x en caractère ascii.
	for(i=2;i>=0;i--)
	{
		valueConvx[i]=(xt %10)+0x30;
		xt=xt/10;
	}
	//Conversion des valeurs en y en caractère ascii.
	for(i=2;i>=0;i--)
	{
		valueConvy[i]=(yt %10)+0x30;
		yt=yt/10;
	}
	//Envoie sur le port Jtag des informations d'état concernant le bouton de droite.
	if(FlagR)
	{

		jUartSendString("Right button pressed!\n"); // Envoie une chaine de caractère

		jUartSendString("X :  ");
		jUartSendString(valueConvx); // Envoie des coordonnées en x
		jUartSendString(" ");        // Espacement

		jUartSendString("Y :  ");
		jUartSendString(valueConvy); // Envoie des coordonnées en y
		jUartSendString("\n ");      // Saut de ligne

	}
	//Envoie sur le port Jtag des informations d'état concernant le bouton de gauche.
	if(FlagL)
	{

		jUartSendString("Left button pressed!\n");
		jUartSendString("X :  ");
		jUartSendString(valueConvx);	// Envoie des coordonnées en x
		jUartSendString(" ");			// Espacement

		jUartSendString("Y :  ");
		jUartSendString(valueConvy);	// Envoie des coordonées en y
		jUartSendString("\n ");			// Saut de ligne

	}
}


