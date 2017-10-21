#include <stdio.h>
#include <signal.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include "vector_t/vector.h"

#define BUF_SIZE 16
#define FREQ 22050
#define CAP_SIZE 2048

ALuint buffer[BUF_SIZE],
       source[1];
vector_t    *queue;
ALCdevice   *audio,
            *input;
ALCcontext  *context;
ALenum error;

static volatile int running = 1;

void cleanup(int dummy) {
  printf("leaning up...");
  
  alcCaptureStop(input);
  alcCaptureCloseDevice(input);
  
  alSourceStopv(1,&source[0]);
  for (int i = 0; i < 1; ++i)
    alSourcei(source[i], AL_BUFFER, 0);
    
  alDeleteSources(1, &source[0]); 
  alDeleteBuffers(16, &buffer[0]);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(audio);
  vector_free(queue);
  
  running = 0;
  
  printf("done!\n");
}

int main(void) {
  signal(SIGINT, cleanup);
  
  queue = vector_init();
  
  audio = alcOpenDevice(NULL);
  error = alcGetError(audio);
  
  context = alcCreateContext(audio, NULL);
  alcMakeContextCurrent(context);
  error = alcGetError(audio);
  
  input = alcCaptureOpenDevice(NULL, FREQ, AL_FORMAT_MONO16, FREQ / 2);
  error = alcGetError(input);
  
  alcCaptureStart(input);
  error = alcGetError(input);
  
  alGenBuffers(BUF_SIZE, &buffer[0]);
  
  for (int i = 0; i < BUF_SIZE; ++i)
    vector_push(queue, (void*)buffer[i]);
    
  alGenSources (1, &source[0]);
  error = alGetError();
    
  short  audio_buffer[CAP_SIZE * 2];
  ALCint samples = 0;
  ALint  avail_buffs = 0;
  ALuint cur_buff;
  ALuint buff_hold[16];
  
  while (running) {
    alGetSourcei(source[0], AL_BUFFERS_PROCESSED, &avail_buffs);
    if (avail_buffs) {
      alSourceUnqueueBuffers(source[0], avail_buffs, buff_hold);
      for (int i = 0; i < avail_buffs; ++i)
        vector_push(queue, (void*)buff_hold[i]);
    }
    
    alcGetIntegerv(input, ALC_CAPTURE_SAMPLES, 1, &samples);
    if (samples > CAP_SIZE) {
      alcCaptureSamples(input, audio_buffer, CAP_SIZE);
      
      if (queue->length) {
        cur_buff = (int)vector_get(queue, 0);
        vector_del(queue, 0);
        
        alBufferData(cur_buff, AL_FORMAT_MONO16, audio_buffer, CAP_SIZE * sizeof(short), FREQ);
        
        
      }
    }
  }

  return 0;
}
