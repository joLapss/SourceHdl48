/********************************************************************************************************

 Company: École de technologie supérieur
 Engineer:
 		Kevin Parent Legault
		Jonathan Lapointe
 Create Date:    2015-05-17
 Design Name: Lab1
 Module Name: jtagUart
 Project Name: Lab1
 Target Devices: Altera Cyclone V SOPC
 Tool versions: NIOSII V14.2
 Description:
 Module de communication jtagUart pour la transmission de trame entre le port console NIOSII console et le
 board.
 Contient les fonctions suivantes :

 1. void jUartSendString(U8 *ptrStr)
 	 - Tant que le tableau contenant le message n'est pas vide, on envoie une donnée sur le port jtagUart
 	 - Vérifie le registre controle et autorise la transmission sur le port si celui-çi est en mode lecture.

 2. void jUartSendVar(U16 value)
 	 -Converti les valeurs numériques en leur valeur ascii correspondante.
 	 - Fait appel à la fonction JuartSendString pour l'envoie des données


 Revision: v1
 Revision 0.01 - File Created
 Additional Comments:


*********************************************************************************************************/


#include "jtagUart.h"

#define JUART_DATA_REG_OFT		 0		 	 // offset du data register
#define JUART_CTRL_REG_OFT		 1


/*
 * @fn 	 void jUartSendString(U8 *ptrStr)
 * @des  Permet d'envoyé des chaines de caractère sur le port console jtagUart
 * @arg  Pointeur sur un tableau contenant le message à envoyer
 */

void jUartSendString(U8 *ptrStr)
{
	U32 data32;
	while(*ptrStr != '\0')
	{
		 	 data32 = (U32) *ptrStr;
		 	 if (IORD(JTAG_UART_BASE,JUART_CTRL_REG_OFT) != 0) {
				 IOWR(JTAG_UART_BASE,JUART_DATA_REG_OFT, data32);
				 ptrStr++;
		 	 }
	}
}


/*
 * @fn 	 void jUartSendVar(U16 value)
 * @des  Permet l'envoie de variable décimale
 * @arg  U16 valeur décimale entre 0 et 1023.
 */

void jUartSendVar(U16 value)
{
	S8 i;
	U8 valueDigit[7];
	//segmentation des digits
	for(i=3;i>=0;i--)
	{
		valueDigit[i]=(value %10)+0x30; // conversion en caractère ascii équivalent.
		value=value/10;
	}
	valueDigit[4]=0;
	jUartSendString(valueDigit); // appel de fonction.

}
