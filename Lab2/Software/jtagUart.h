//*jtagUart.c pour description de fichier.
#ifndef __JTAG_UART_H__
#define __JTAG_UART_H__

#include "hardware.h"


/*
 * @fn 	 void jUartSendString(U8 *ptrStr)
 * @des  Permet d'envoyé des chaines de caractère sur le port console jtagUart
 * @arg  Pointeur sur un tableau contenant le message à envoyer
 */
void jUartSendString(U8 *ptrStr);

/*
 * @fn 	 void jUartSendVar(U16 value)
 * @des  Permet l'envoie de variable décimale
 * @arg  U16 valeur décimale entre 0 et 1023.
 */

void jUartSendVar(U16 value);

#endif
