/*
	audio.c - SDL audio system initialization & usage
*/

#include "globals.h"

#define NUM_SOUNDS		2

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
		SDL_MixAudio(stream, &sounds[i].data[sounds[i].dpos], amount, 64);
		sounds[i].dpos += amount;
	}
}

int initAudio()
{
	// set wav file path
	strcpy(program.wavfile, parseFormat("%s%cbeep.wav", program.apppath, FILESEP));

	#ifdef HW_RVL
	return EXIT_SUCCESS;	//temporary disable audio for Wii, to get rid of constant noise
	#endif

	// SDL audio spec struct
	SDL_AudioSpec fmt;

	// load wav file for analyzation (ch, freq, etc)
	FILE* fp;
	fp = fopen(program.wavfile, "rb");
	if(fp == NULL) return EXIT_FAILURE;
	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	rewind(fp);
	unsigned char * fdata = (unsigned char *)malloc(sizeof(char) * fsize);
	memset(fdata, 0x00, fsize);
	fread(fdata, fsize, 1, fp);

	// get the values and put into struct
	int WAVChunkID = (read32(fdata, 0)), WAVFormat = (read32(fdata, 8)), SubChunk1ID = (read32(fdata, 12));
	if((WAVChunkID != 0x52494646) && (WAVFormat != 0x57415645) && (SubChunk1ID != 0x666d7420)) {	// "RIFF", "WAVE", "fmt "
		printf("Invalid sound file!\n");
		return EXIT_FAILURE;
	}
	fmt.channels = read16s(fdata, 22);
	fmt.freq = read32s(fdata, 24);
	switch(read16s(fdata, 34)) {
		case 8: fmt.format = AUDIO_S8; break;
		case 16: fmt.format = AUDIO_S16; break;
		default: fmt.format = AUDIO_S8; break;
	}
//	printf("ch:%i, freq:%i, bits:%i\n", fmt.channels, fmt.freq, read16s(fdata, 34));

	// free the data, close the file
	free(fdata);
	fclose(fp);

	// set remaining values
	fmt.samples = 512;
	fmt.callback = (void*)mixAudio;
	fmt.userdata = NULL;

	// open audio
	if(SDL_OpenAudio(&fmt, NULL) < 0) {
		printf("Unable to open audio: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	// empty sound list
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
		// just drop the sound and return
		return EXIT_SUCCESS;
	}

	if(SDL_LoadWAV(file, &wave, &data, &dlen) == NULL) {
		printf("Couldn't load %s: %s\n", file, SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_BuildAudioCVT(&cvt,
		wave.format, wave.channels, wave.freq,
		wave.format, wave.channels, wave.freq);

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
