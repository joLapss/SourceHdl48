/********************************************************************************************************

 Company: �cole de technologie sup�rieur
 Engineer:
 		Kevin Parent Legault
		Jonathan Lapointe
 Create Date:  2015-05-26
 Module Name: main.c
 Project Name: lab2n
 Target Devices: Altera cyclone V SOPC
 Tool versions: NIOS II v14.1
 Description: Programme principale
	

 Revision: v1
 Revision 0.01 - File Created
 Additional Comments:


*********************************************************************************************************/

//Function declaration


#include "hardware.h"
#include "jtagUart.h"
#include "display.h"
#include "system.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_regs.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_up_avalon_audio_and_video_config_regs.h"
#include "math.h"
#include "keyboard.h"
#include "audio.h"


/*
 * @fn 	 �crire fonction
 * @des  �crire description
 * @arg  �crire les arguments
 * @ret  �crire le retour de fonction
 */
int main(void)
{

	U8 lastNotePlayed,noteToPlay,state,mode,sampleFlag,playFlag;
	U16 sustainValue=0;
	lastNotePlayed=0;
	state = DISABLED;
	sampleFlag =0;
	playFlag=0;
	/* used for audio record/playback */
	// open the Audio port

	InitDisplay();
	audioInit();
	kbInit();
	audioCalculateNotes(100000,10);

	while(1)
	{

//V�rifie les �v�nements du clavier
		 if(kBGetEvent())
		 {
			 sustainValue=10000;
			 noteToPlay = kBGetKey();
			 mode = kBGetKey();
			 if(mode == KB_KEY_SAMPLE)
				 sampleFlag=1;
			 else if(mode == KB_KEY_PLAY)
				 playFlag=1;

			 if(state == ENABLED)PlayNote(lastNotePlayed,CLEAR);
			 PlayNote(noteToPlay,WRITE); //s�lectionne la note � l'�cran
			 SelectPianoMode(mode);//Change le mode � l'�cran
			 lastNotePlayed = noteToPlay;
			 state=ENABLED;

		 }
		 if(sustainValue!=0)
		 {
			 audioPlayNote(noteToPlay); //joue la note durant un d�lai d�finie par sustainValue
			 sustainValue--;
		 }
		 else
			 PlayNote(noteToPlay,CLEAR); // efface la note � l'�cran lorsque le d�lai est terminer
		 if(sampleFlag)
		 {
			 audioSampling(); // D�marre l'enregistrement.
			 SelectPianoMode(1);
			 sampleFlag=0;
		 }
		 else if(playFlag)
		 {
			 audioPlaySampling(); // Joue l'enregistrement.
			 SelectPianoMode(1);
			 playFlag=0;
		 }


	}

return 0;
}
