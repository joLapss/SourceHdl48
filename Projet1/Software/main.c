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
#include "display.h"
#include "system.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_regs.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_up_avalon_audio_and_video_config_regs.h"
#include "math.h"
#define SINCOEFF (2 * 3.14159265)
#define N_MAX 32
#define FS 32000
#define COUNTER_MAX 5000
#define ATTACK_MAX_VALUE 250
#define DECAY_COUNTER_MAX 150
#define DECAY_MIN_VALUE 150
#define SUSTAIN_MAX_VALUE 1200
#define RELEASE_MIN_VALUE 0
#define THRESHOLD 1000000
#define MAX 35000


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
				0.00148504944587359,
//		9,71376770711095e-05,
//		1,1609958083e-04,
//		4,53772621849378e-05,
//		8,85994333616839e-05,
//		4,26634796205724e-05,
//		3,95047692707856e-05,
//		4,42489929019252e-05,
//		3,98435642912692e-05,
//		4,06633182795738e-05,
//		4,75375763420847e-05,
//		7,36483043337695e-05,
//		1,86254827716298e-05,
//		4,31312730616798e-05,
//		3,45156712317513e-05,
//		3,16655668406403e-05,
//		8,79244275788583e-06,
//		2,23576228301962e-05,
//		4,18111575244602e-06,
//		1,09502302263083e-05,
//		3,77848978766485e-05,
//		3,46467944068002e-05,
//		1,11679459039710e-05,
//		2,72500773842878e-05,
//		3,96494007335124e-05,
//		3,63768793345249e-05,
//		2,34858193091308e-05,
//		1,15682297779148e-05,
//		8,83304989667264e-06,
//		4,42339540762903e-05,
//		1,38273093067247e-05,
//		1,18418099143231e-05,

			};

typedef enum eAttackState
{
	WAIT,
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE

}tAttackState;



/*
 * @fn 	 écrire fonction
 * @des  écrire description
 * @arg  écrire les arguments
 * @ret  écrire le retour de fonction
 */
int main(void)
{
	unsigned int yOutL[256],yOutR[256],yOut1,yOut2,yOut[256],yOutResult[256],accR[MAX],accL[MAX];
	int fifospaceR,fifospaceL,wordReadR,wordReadL,R,L;
	signed int note[MAX];
	float angle;
	U32 zMax,zMaxAngle,freq,A,audioDigitalInterface,samplingCtrl,analogAudioPathCtrl,digitalAudioPathCtrl,dataTest,pitchShiftMax,i,j;
	U16 z,counter,pitchShifter;
	U16 compteurPallier,CoeffAttack;
	U8 n;
	U8 stateAttack=ATTACK;
	U16 leftPtr=0;
	U16 rightPtr=0;
	A = 1000;
	freq =440;
	wordReadR=0;
	wordReadL=0;
	R=0;
	L=0;
	counter = 0;
	pitchShifter=1000;

	alt_up_audio_dev * audio_dev;
	alt_up_av_config_dev* audiocfg=alt_up_av_config_open_dev(AUDIO_AND_VIDEO_CONFIG_0_NAME);

	/* used for audio record/playback */
	// open the Audio port
	audio_dev = alt_up_audio_open_dev (AUDIO_0_NAME);
	InitDisplay();

	if(audio_dev == NULL )jUartSendString("problem");
	zMax=FS/(freq);

	for(z=0;z<256;z++)yOut[z]=0;
//	for(z=0;z<zMax;z++)
//		yOut[z]=(unsigned int)((float)(A*cos((SINCOEFF/zMax)*z)));
	for(n=1;n<32;n++)
	{
		zMaxAngle=FS/(freq*n);
		angle = (float)(SINCOEFF/zMaxAngle);
		for(z=0;z<zMax;z++)
			yOut[z]=yOut[z]+(unsigned int)((A*p[n+2]*cos(angle*(float)z)));
	}
	for(z=0;z<zMax;z++)
	yOut[z]=yOut[z]&0xFFFFFF00;
   //alt_up_av_config_write_audio_cfg_register(audiocfg, AUDIO_REG_SAMPLING_CTRL, 0x0);
	accR[0]=0;

	while((signed int)accR[0] < THRESHOLD)
	{
		fifospaceR = alt_up_audio_read_fifo_avail(audio_dev,ALT_UP_AUDIO_RIGHT);
		if(fifospaceR > 0)
			alt_up_audio_read_fifo(audio_dev,accR,1, ALT_UP_AUDIO_RIGHT);

	}

	while(R < MAX)
	{
		fifospaceR = alt_up_audio_read_fifo_avail(audio_dev,ALT_UP_AUDIO_RIGHT);
		//fifospaceR = (IORD_ALT_UP_AUDIO_FIFOSPACE(audio_dev->base)& ALT_UP_AUDIO_FIFOSPACE_WSRC_MSK) >> ALT_UP_AUDIO_FIFOSPACE_WSRC_OFST;
		if(fifospaceR > 100) //before zMax
		{
			wordReadR = alt_up_audio_read_fifo(audio_dev,&accR[R],100, ALT_UP_AUDIO_RIGHT);
			//alt_up_audio_read_fifo(audio_dev, &(yOut2),1, ALT_UP_AUDIO_LEFT);
			//alt_up_audio_write_fifo(audio_dev,yOutResult,zMax, ALT_UP_AUDIO_RIGHT);
			R=R+100;

		}

	}
	pitchShiftMax = MAX/pitchShifter;
	for(i=0;i<=pitchShiftMax;i++)
	{
		note[i]=accR[j];
		j=j+pitchShifter;

	}


	//InitDisplay();
	while(1)
	{
//		switch(stateAttack)
//		{
//			case ATTACK:
//				compteurPallier++;
//				if(compteurPallier==COUNTER_MAX)
//				{
//					CoeffAttack++;
//					compteurPallier=0;
//					for(z=0;z<zMax;z++)
//						yOutResult[z]=(signed int)(yOut[z]*CoeffAttack);
//				}
//				if(CoeffAttack==ATTACK_MAX_VALUE)
//					stateAttack=DECAY;
//			break;
//			case DECAY:
//				compteurPallier++;
//				if(compteurPallier==COUNTER_MAX)
//				{
//						CoeffAttack--;
//						compteurPallier=0;
//						for(z=0;z<zMax;z++)
//								yOutResult[z]=(signed int)(yOut[z]*CoeffAttack);
//				}
//				if(CoeffAttack==DECAY_MIN_VALUE)
//						stateAttack=DECAY;
//			break;
//			case SUSTAIN:
//				compteurPallier++;
//				if(compteurPallier==COUNTER_MAX)
//						compteurPallier=0;
//				if(CoeffAttack==SUSTAIN_MAX_VALUE)
//						stateAttack=DECAY;
//			break;
//			case RELEASE:
//				compteurPallier++;
//				if(compteurPallier==COUNTER_MAX)
//				{
//						CoeffAttack--;
//						compteurPallier=0;
//						for(z=0;z<zMax;z++)
//							yOutResult[z]=(signed int)(yOut[z]*CoeffAttack);
//				}
//				if(CoeffAttack==RELEASE_MIN_VALUE)
//				{
//							stateAttack=WAIT;
//				}
//			break;
//			case WAIT:
//				compteurPallier++;
//				if(compteurPallier==COUNTER_MAX)
//				{
//					CoeffAttack++;
//					compteurPallier=0;
//
//				}
//				if(CoeffAttack==300)
//				{
//					stateAttack=ATTACK;
//					CoeffAttack=0;
//				}
//			break;
//
//		}

		//   alt_up_av_config_read_data(audiocfg,AUDIO_REG_AUDIO_DIGITAL_INTERFACE, &audioDigitalInterface);
		//   alt_up_av_config_read_data(audiocfg,AUDIO_REG_SAMPLING_CTRL, &samplingCtrl);
		//   alt_up_av_config_read_data(audiocfg, AUDIO_REG_ANALOG_AUDIO_PATH_CTRL, &analogAudioPathCtrl);
		//   alt_up_av_config_read_data(audiocfg, AUDIO_REG_DIGITAL_AUDIO_PATH_CTRL, &digitalAudioPathCtrl);
		//   alt_up_av_config_read_data(audiocfg, 0x0000012, &dataTest);


		  // jUartSendVar((U16)((audioDigitalInterface&0xFFFF0000)>>16));
		 //  jUartSendVar((U16)(audioDigitalInterface&0x0000FFFF));
	//	NiosDrawApp();

	// write audio buffer
	//	for(i=0;i<500;i++);

//		if(alt_up_audio_play_l(audio_dev, &yOut[leftPtr],1))
//			leftPtr++;
//		if(alt_up_audio_play_r(audio_dev, &yOut[rightPtr],1))
//			rightPtr++;
//		if(leftPtr>=zMax)
//			leftPtr=0;
//		if(rightPtr>=zMax)
//			rightPtr=0;

			//rightPtr++;

			//if(rightPtr>=256)
			//		rightPtr=0;
		 //fifospaceL = alt_up_audio_read_fifo_avail(audio_dev,ALT_UP_AUDIO_LEFT);
		DrawPiano(50, 50);
		 fifospaceL = (IORD_ALT_UP_AUDIO_FIFOSPACE(audio_dev->base)& ALT_UP_AUDIO_FIFOSPACE_WSLC_MSK) >> ALT_UP_AUDIO_FIFOSPACE_WSLC_OFST;
			if(fifospaceL > 100) //before zMax
			{

				//alt_up_audio_read_fifo(audio_dev,&accL[L],fifospaceL, ALT_UP_AUDIO_LEFT);

			//	alt_up_audio_read_fifo(audio_dev, &(yOut2),1, ALT_UP_AUDIO_LEFT);
				//L=L+wordReadL;
				//if(L>35000)L=0;

				alt_up_audio_write_fifo(audio_dev,&accR[L],100, ALT_UP_AUDIO_LEFT);
				L=L+100;
				if(L>=MAX)L=0;
				//rightPtr++;

				//if(rightPtr>=256)
				//		rightPtr=0;
			}
			 fifospaceR = (IORD_ALT_UP_AUDIO_FIFOSPACE(audio_dev->base)& ALT_UP_AUDIO_FIFOSPACE_WSRC_MSK) >> ALT_UP_AUDIO_FIFOSPACE_WSRC_OFST;
				if(fifospaceR > 100) //before zMax
				{

					//alt_up_audio_read_fifo(audio_dev,&accL[L],fifospaceL, ALT_UP_AUDIO_LEFT);

				//	alt_up_audio_read_fifo(audio_dev, &(yOut2),1, ALT_UP_AUDIO_LEFT);
					//L=L+wordReadL;
					//if(L>35000)L=0;

					alt_up_audio_write_fifo(audio_dev,&accR[R],100, ALT_UP_AUDIO_RIGHT);
					R=R+100;
					if(R>=MAX)R=0;
					//rightPtr++;

					//if(rightPtr>=256)
					//		rightPtr=0;
				}

	//	alt_up_audio_write_fifo (audio_dev, yOut, zMax, ALT_UP_AUDIO_RIGHT);
	//	alt_up_audio_write_fifo (audio_dev, yOut, zMax, ALT_UP_AUDIO_LEFT);



	}



return 0;
}
