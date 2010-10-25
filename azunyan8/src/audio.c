/*
	audio.c - SDL audio system initialization & usage
*/

#include "globals.h"

#define NUM_SOUNDS		48
#define SOUND_FREQUENCY	44100

struct sample {
	unsigned char *data;
	unsigned int dpos;
	unsigned int dlen;
} sounds[NUM_SOUNDS];

void mixAudio(void * unused, unsigned char * stream, int len)
{
	unused = NULL;

	int i;
	unsigned int amount;

	for(i = 0; i < NUM_SOUNDS; i++) {
		amount = (sounds[i].dlen - sounds[i].dpos);
		if(amount > (unsigned int)len) amount = (unsigned int)len;
		SDL_MixAudio(stream, &sounds[i].data[sounds[i].dpos], amount, SDL_MIX_MAXVOLUME);
		sounds[i].dpos += amount;
	}
}

int initAudio()
{
	#ifdef HW_RVL
	return EXIT_SUCCESS;	//temporary disable audio for Wii, to get rid of constant noise
	#endif

	SDL_AudioSpec fmt;

	fmt.freq = SOUND_FREQUENCY;
	fmt.format = AUDIO_S16;
	fmt.channels = 1;
	fmt.samples = 512;
	fmt.callback = (void*)mixAudio;
	fmt.userdata = NULL;

	if(SDL_OpenAudio(&fmt, NULL) < 0) {
		printf("Unable to open audio: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	int i;
	for(i = 0; i < NUM_SOUNDS; i++) {
		sounds[i].dpos = 0;
		sounds[i].dlen = 0;
	}

	SDL_PauseAudio(0);

	return EXIT_SUCCESS;
}

void endAudio()
{
	SDL_PauseAudio(0);
	SDL_CloseAudio();
}

int playSound(char * file)
{
	int index;
	SDL_AudioSpec wave;
	Uint8 *data;
	Uint32 dlen;
	SDL_AudioCVT cvt;

	for(index = 0; index < NUM_SOUNDS; index++) {
		if(sounds[index].dpos == sounds[index].dlen) break;
	}

	if(index == NUM_SOUNDS) {
		printf("Couldn't find empty sound slot!\n");
		return EXIT_FAILURE;
	}

	if(SDL_LoadWAV(file, &wave, &data, &dlen) == NULL) {
		printf("Couldn't load %s: %s\n", file, SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq, AUDIO_S16, 2, SOUND_FREQUENCY);
	cvt.buf = malloc(dlen*cvt.len_mult);
	memcpy(cvt.buf, data, dlen);
	cvt.len = dlen;
	SDL_ConvertAudio(&cvt);
	SDL_FreeWAV(data);

	if(sounds[index].data) free(sounds[index].data);

	SDL_LockAudio();
	sounds[index].data = cvt.buf;
	sounds[index].dlen = cvt.len_cvt;
	sounds[index].dpos = 0;
	SDL_UnlockAudio();

	return EXIT_SUCCESS;
}
