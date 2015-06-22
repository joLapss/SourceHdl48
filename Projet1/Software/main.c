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





/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
int main(void)
{
	alt_up_audio_dev * audio_dev;
	//alt_up_av_config_dev *control_audio_dev;

	/* used for audio record/playback */
	U16 counter,counterloop,counterloop2;

	counter = 0;
	counterloop = 0;
	counterloop2=0;
	unsigned int buf;
	buf = 0;
	// open the Audio port
	audio_dev = alt_up_audio_open_dev (AUDIO_0_NAME);
	//control_audio_dev= alt_up_av_config_open_dev(AUDIO_AND_VIDEO_CONFIG_0_NAME);
	//alt_up_av_config_reset(control_audio_dev);
	//alt_up_audio_reset_audio_core(audio_dev);
	if(audio_dev == NULL )jUartSendString("problem");


	while(1)
	{



	// write audio buffer


		//alt_up_audio_play_l(audio_dev, &buf, 1);
		//alt_up_audio_play_r(audio_dev, &buf, 1);
		alt_up_audio_write_fifo (audio_dev, &(buf), 1, ALT_UP_AUDIO_RIGHT);
		alt_up_audio_write_fifo (audio_dev, &(buf), 1, ALT_UP_AUDIO_LEFT);

	if(counterloop2 > 200)
	{
		buf =~buf;
		counterloop2 = 0;
	}


	counterloop++;
	counterloop2++;

	}



return 0;
}
