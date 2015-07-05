/*
 * audio.c
 *
 *  Created on: 2015-07-01
 *      Author: Jonathan
 */
#include "jtagUart.h"
#include "audio.h"
#include "system.h"
#include "hardware.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_regs.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_up_avalon_audio_and_video_config_regs.h"
#include "math.h"
#define AUDIO_SAMPLING_MAX_DATA 3500000

U32 zMax[14];
unsigned int audioYout[14][256];
U16 audioOutLIndex,audioOutRIndex;
unsigned int audioSamplingLData[AUDIO_SAMPLING_MAX_DATA];
unsigned int audioSamplingRData[AUDIO_SAMPLING_MAX_DATA];
alt_up_audio_dev * audio_dev;
alt_up_av_config_dev* audiocfg;

U32 audioFreq[14]={262,294,330,349,392,440,494,
				   523,277,311,370,415,466,554};

// coefficient des harmoniques
double audioP[32] = {
				0.0284526460403719,
				0.0381257369591583,
				0.0382913541784970,
				0.188655778821136,
				0.0809044605862136,
				0.0745141028130317,
				0.0526725257837954,
				0.110248205447958,
				0.0604132222922300,
				0.00437649649187595,
				0.00157551915761215,
				0.00423224770954643,
				0.0421781048852797,
				0.00754478477729322,
				0.00194372256570633,
				0.00466739974364155,
				0.0440091956721521,
				0.00911230166585922,
				0.00454613556279803,
				0.00413255445969453,
				0.0113172217581694,
				0.0115798300795365,
				0.00264163298150578,
				0.00288154774700639,
				0.00497065429082987,
				0.0223861926323031,
				0.00500983674940492,
				0.00404733038601476,
				0.000683451190844474,
				0.00700762889245614,
				0.00406492255798719,
				0.00148504944587359,
};


void audioInit(void)
{
   audio_dev = alt_up_audio_open_dev (AUDIO_0_NAME);
   audiocfg=alt_up_av_config_open_dev(AUDIO_AND_VIDEO_CONFIG_0_NAME);
}

void audioPlayNote(U8 note)
{
	int fifospaceR,fifospaceL;
	U16 toWriteNbL=100,toWriteNbR=100;


	if(audioOutLIndex>=zMax[note])
		audioOutLIndex=0;

	if(audioOutRIndex>=zMax[note])
		audioOutRIndex=0;

	if((zMax[note]-audioOutLIndex)<100)
		toWriteNbL=zMax[note]-audioOutLIndex;

	if((zMax[note]-audioOutRIndex)<100)
		toWriteNbR=zMax[note]-audioOutRIndex;

	fifospaceR = (IORD_ALT_UP_AUDIO_FIFOSPACE(audio_dev->base)& ALT_UP_AUDIO_FIFOSPACE_WSRC_MSK) >> ALT_UP_AUDIO_FIFOSPACE_WSRC_OFST;
	fifospaceL = (IORD_ALT_UP_AUDIO_FIFOSPACE(audio_dev->base)& ALT_UP_AUDIO_FIFOSPACE_WSLC_MSK) >> ALT_UP_AUDIO_FIFOSPACE_WSLC_OFST;

	if(fifospaceL > toWriteNbL)
	{
		alt_up_audio_write_fifo(audio_dev,&audioYout[note][audioOutLIndex],toWriteNbL, ALT_UP_AUDIO_LEFT);
		audioOutLIndex+=toWriteNbL;
	}
	if(fifospaceR > toWriteNbR)
	{
		alt_up_audio_write_fifo(audio_dev,&audioYout[note][audioOutRIndex],toWriteNbR, ALT_UP_AUDIO_RIGHT);
		audioOutRIndex+=toWriteNbR;
	}
}
void audioPlaySampling(void)
{
	int fifospaceL,fifospaceR;
	U32 ptrIndexL=0,ptrIndexR=0;
	while(ptrIndexR<AUDIO_SAMPLING_MAX_DATA||ptrIndexL<AUDIO_SAMPLING_MAX_DATA)
	{


			fifospaceL = (IORD_ALT_UP_AUDIO_FIFOSPACE(audio_dev->base)& ALT_UP_AUDIO_FIFOSPACE_WSLC_MSK) >> ALT_UP_AUDIO_FIFOSPACE_WSLC_OFST;

			if(fifospaceL>100&&ptrIndexL<AUDIO_SAMPLING_MAX_DATA)
			{
				alt_up_audio_write_fifo(audio_dev,&audioSamplingLData[ptrIndexL],100, ALT_UP_AUDIO_LEFT);
				ptrIndexL+=100;
			}
			fifospaceR = (IORD_ALT_UP_AUDIO_FIFOSPACE(audio_dev->base)& ALT_UP_AUDIO_FIFOSPACE_WSRC_MSK) >> ALT_UP_AUDIO_FIFOSPACE_WSRC_OFST;
			if(fifospaceR>100&&ptrIndexR<AUDIO_SAMPLING_MAX_DATA)
			{
				alt_up_audio_write_fifo(audio_dev,&audioSamplingRData[ptrIndexR],100, ALT_UP_AUDIO_RIGHT);
				ptrIndexR+=100;
			}
	}
}

void audioSampling(void)
{
	int fifospaceR,fifospaceL;
	U32 ptrIndexL=0,ptrIndexR=0;
	while(ptrIndexL<AUDIO_SAMPLING_MAX_DATA||ptrIndexR<AUDIO_SAMPLING_MAX_DATA)
	{

		fifospaceL=alt_up_audio_read_fifo_avail(audio_dev,ALT_UP_AUDIO_LEFT);
		if(fifospaceL>100&&ptrIndexL<AUDIO_SAMPLING_MAX_DATA)
		{
			alt_up_audio_read_fifo(audio_dev,&audioSamplingLData[ptrIndexL],100, ALT_UP_AUDIO_LEFT);
			ptrIndexL+=100;
		}
		fifospaceR=alt_up_audio_read_fifo_avail(audio_dev,ALT_UP_AUDIO_RIGHT);
		if(fifospaceR>100&&ptrIndexR<AUDIO_SAMPLING_MAX_DATA)
		{
			alt_up_audio_read_fifo(audio_dev,&audioSamplingRData[ptrIndexR],100, ALT_UP_AUDIO_RIGHT);
			ptrIndexR+=100;
		}

	}
}

#define SINCOEFF (2.0f * 3.14159265f)
#define AUDIO_F_SAMPLING 32000

void audioCalculateNotes(U32 Amplitude,U8 nMax)
{
	U8 n;
	U32 zMaxAngle;
	U16 z;
	U8 note;
	float angle;
	for(note=0;note<14;note++)
	{
		zMax[note]=AUDIO_F_SAMPLING/audioFreq[note];
		for(z=0;z<256;z++)
			audioYout[note][z]=0;
		for(n=1;n<nMax;n++)
		{
			zMaxAngle=AUDIO_F_SAMPLING/(audioFreq[note]*n);
			angle = (float)(SINCOEFF/(float)zMaxAngle);
			for(z=0;z<zMax[note];z++)
				audioYout[note][z]=audioYout[note][z]+(unsigned int)(((double)Amplitude*audioP[n])*cos(angle*(double)z));
					//audioYout[note][z]=audioYout[note][z]+(unsigned int)(Amplitude*audioP[n]*cos(angle*(float)z));
		}
		for(z=0;z<zMax[note];z++)
			audioYout[note][z]=audioYout[note][z]&0xFFFFFF00;
	}
}
