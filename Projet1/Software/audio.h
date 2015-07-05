/*
 * audio.h
 *
 *  Created on: 2015-07-01
 *      Author: Jonathan
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include "hardware.h"


void audioInit(void);
void audioPlayNote(U8 note);
void audioSampling(void);
void audioPlaySampling(void);
void audioCalculateNotes(U32 Amplitude,U8 nMax);

#endif /* AUDIO_H_ */
