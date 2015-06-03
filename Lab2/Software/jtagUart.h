//*jtagUart.c pour description de fichier.
#ifndef __JTAG_UART_H__
#define __JTAG_UART_H__

#include "hardware.h"


/*
 * @fn 	 void jUartSendString(U8 *ptrStr)
 * @des  Permet d'envoy� des chaines de caract�re sur le port console jtagUart
 * @arg  Pointeur sur un tableau contenant le message � envoyer
 */
void jUartSendString(U8 *ptrStr);

/*
 * @fn 	 void jUartSendVar(U16 value)
 * @des  Permet l'envoie de variable d�cimale
 * @arg  U16 valeur d�cimale entre 0 et 1023.
 */

void jUartSendVar(U16 value);

#endif
