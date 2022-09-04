#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <stdio.h>
#include <string.h>

#ifndef SIMULATOR
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "cmsis_os.h"
#include "sine.h"

static MSGQUEUE_WAVE_t  wave;
#endif

void shareModelInstance(Model *m);

Model::Model() : modelListener(0)
{
#ifndef SIMULATOR
  shareModelInstance(this);
#endif
}

void Model::tick()
{
#ifndef SIMULATOR
  extern osMessageQueueId_t waveQueueHandle;

  int waveCount = 0;
  while(osMessageQueueGet(waveQueueHandle, &wave, 0U, 0) == osOK){
    for(int i=0; i<5; i++)
      waveBuf[waveCount++] = wave.val[i];
#if 0
//    printf("--- Get Queue ---\n");
    for(int i=0; i<5; i++)
      printf("[%d] = %d, ", i, (int)wave.val[i]);
    printf("\n");
#endif
  }

  modelListener->putData(waveBuf, waveCount);
#endif
}

void Model::putData(float* wave, int n)
{
#if 0
//  printf("-----------------------------------\n");
//  printf("count = %d : data", n);
  for(int i=0; i<n; i++)
    printf("[%d] = %d, ", i, (int)wave[i]);
  printf("\n");
#endif

  modelListener->putData(wave, n);
}

/*
 * -------------------------------------- Model instance --------------------------------------
 * The below functions are not member of Model class but normal function to disclose Model instance to the public.
 */
Model *pmodel;

void shareModelInstance(Model *m)
{
  pmodel = m;
}

#ifndef SIMULATOR
//extern "C" { extern osMessageQueueId_t waveQueueHandle; }
extern "C" void putData(float *wave)
{
  pmodel->putData(wave, 1);
}
#endif


