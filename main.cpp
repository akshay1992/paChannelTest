#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <atomic>
#include <string>

#include "portaudio.h"
#include "utils.hpp"

#include "Gamma/Oscillator.h"

#define SAFETY_GAIN60 0.00001	// -60dBFS

using namespace std;
using namespace gam;

// Setting up a simple sine oscillator
Sine<> s(440.0);

typedef struct 
{
	atomic<int> nchannels;	// Total number of channels
	atomic<int> channel;	// Selected channel
} paUserData;

// Port Audio Callback
int callback( const void *input, 
				void *output,
             	unsigned long frameCount, 
             	const PaStreamCallbackTimeInfo *timeInfo,
             	PaStreamCallbackFlags statusFlags, 
             	void *userData ) 
{

  float *in = (float *) input;
  float *out = (float *) output;
  paUserData *data = (paUserData*) userData;
  int nchannels = data->nchannels.load();
  int channel = data->channel.load();

  for(int i=0; i<frameCount; i+= nchannels)
  {
	out[i+channel-1] = s() * SAFETY_GAIN60;
  }

  return paContinue;
}

int main (void)
{
	// --------------------------
	// INIT PA AND CHOOSE DEVICE
	// --------------------------

	Sine<> p;
	for (int i=0; i<010000; i++)
		cout << p() << endl;;

	PaError err;
	err = Pa_Initialize();
	if (err != paNoError) PaErrorHandler();	

	for(int i = 0; i < Pa_GetDeviceCount(); i++) {
	   const PaDeviceInfo* info = Pa_GetDeviceInfo(i);
	   printf("%2i - %s\n", i, info->name);
	}
	fflush(stdout);

	cout << "Please select device: ";
	int deviceIndex;

	// cout << "0" << endl;			// Audiofire12
	scanf("%d", &deviceIndex);
	
	deviceIndex = 0;
	PaDeviceInfo info = *Pa_GetDeviceInfo(deviceIndex);

	printf("* Device %i Selected *\n", deviceIndex);
	printDeviceInfo(info);

	// --------------------------
	// SETUP PA STREAM
	// --------------------------

	int bufferSize = 128;
	int sr = info.defaultSampleRate;
	int nchannels = info.maxOutputChannels;

	PaStream *stream;
	PaStreamParameters inparams;
	PaStreamParameters outparams;

	inparams.device = deviceIndex;
	inparams.channelCount = info.maxInputChannels;
	inparams.sampleFormat = paInt32;
	inparams.suggestedLatency = info.defaultLowInputLatency;
	inparams.hostApiSpecificStreamInfo = NULL;

	outparams.device = deviceIndex;
	outparams.channelCount = info.maxOutputChannels;
	outparams.sampleFormat = paInt32;
	outparams.suggestedLatency = info.defaultLowOutputLatency;
	outparams.hostApiSpecificStreamInfo = NULL;

	// --------------------------
	// START PA THREAD
	// --------------------------

	paUserData data;
	data.channel = 1;
	data.nchannels = nchannels;

	err = Pa_OpenStream(&stream, &inparams, &outparams, sr, bufferSize, paNoFlag, callback, &data);
	if (err != paNoError) PaErrorHandler();

  	err = Pa_StartStream(stream);
	if (err != paNoError) PaErrorHandler();

	printf("Input channel number (1-%d) to send audio to that channel\n", nchannels);
	cout << "Press 'q' to quit." << endl;
	
	char c ;
	while(c != 'q')
	{
		fflush(stdin);
		c = cin.get();
		if( c=='1' )
			atomic_store(&data.channel, 1);
		else if(c=='2') 
			atomic_store(&data.channel, 2);
		else if(c=='3') 
			atomic_store(&data.channel, 3);
		else if(c=='4') 
			atomic_store(&data.channel, 4);
		else if(c=='5') 
			atomic_store(&data.channel, 5);
		else if(c=='6') 
			atomic_store(&data.channel, 6);
		else if(c=='7') 
			atomic_store(&data.channel, 7);
		else if(c=='8') 
			atomic_store(&data.channel, 8);
		else if(c=='9') 
			atomic_store(&data.channel, 9);
	}

	err = Pa_StopStream(stream);
	if (err != paNoError) PaErrorHandler();	

	err = Pa_CloseStream(stream);
	if (err != paNoError) PaErrorHandler();	

	err = Pa_Terminate();
	if (err != paNoError) PaErrorHandler();	

	return 0;
}
