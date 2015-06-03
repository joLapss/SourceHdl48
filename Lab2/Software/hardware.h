//*hardware.c pour description de fichier




#ifndef HARDWARE_H
#define HARDWARE_H

// Inclusion des librairies bsp
#include "io.h"
#include "alt_types.h"
#include "system.h"
#include "sys/alt_sys_init.h"



//DECLARATION DE TYPES
#define U8  alt_u8
#define S8  alt_8
#define U32 alt_u32
#define U16 alt_u16


void hDInitHardware(void);


#endif
