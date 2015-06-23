/********************************************************************************************************

 Company: École de technologie supérieur
 Engineer:
 		Kevin Parent Legault
		Jonathan Lapointe
 Create Date:  2015-05-26
 Module Name: main.c
 Project Name: lab2n
 Target Devices: Altera cyclone V SOPC
 Tool versions: NIOS II v14.1
 Description: Programme principale appelant l'application NiosDrawv0.1
	

 Revision: v1
 Revision 0.01 - File Created
 Additional Comments:


*********************************************************************************************************/

//Function declaration


#include "hardware.h"
#include "jtagUart.h"
#include "system.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "math.h"
#define SINCOEFF (2 * 3.14159265)
#define N_MAX 32
#define FS 48000

float p[32] = {
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
				0.00148504944587359

			};

unsigned int yOut[256];



/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
int main(void)
{
	float angle;
	U32 zMax,freq,A;
	U16 z;
	U8 n;
	A = 8000000;
	freq = 600;


	alt_up_audio_dev * audio_dev;

	/* used for audio record/playback */
	// open the Audio port
	audio_dev = alt_up_audio_open_dev (AUDIO_0_NAME);
	if(audio_dev == NULL )jUartSendString("problem");

	for(z=0;z<256;z++)yOut[z]=0;
	for(n=1;n<10;n++)
	{
		zMax=FS/(freq*n);
		angle = (float)(SINCOEFF/zMax);
		for(z=0;z<zMax;z++)
			yOut[z]=yOut[z]+(unsigned int)((A*p[n+2]*cos(angle*(float)z))+A);
	}
	for(z=0;z<zMax;z++)
		yOut[z]=yOut[z]&0xFFFFFF00;
	while(1)
	{



	// write audio buffer


		//alt_up_audio_play_l(audio_dev, &buf, 1);
		//alt_up_audio_play_r(audio_dev, &buf, 1);
		alt_up_audio_write_fifo (audio_dev, yOut, zMax, ALT_UP_AUDIO_RIGHT);
		alt_up_audio_write_fifo (audio_dev, yOut, zMax, ALT_UP_AUDIO_LEFT);



	}



return 0;
}
