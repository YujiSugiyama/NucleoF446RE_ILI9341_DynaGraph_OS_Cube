#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cmsis_os.h"
#include "sine.h"

static int tick, tickH, tickM, tickL;
static int intervalDeg;
static int unitDeg;
static float  sin_lut[360];
static int dataCount;
static int readyCount;
static MSGQUEUE_WAVE_t  wave;
/*
 * --------------------------------------SIN curve generation --------------------------------------
 */
void initSin()
{
  readyCount = 0;
  tick = 0;
  unitDeg = 2;      // 2deg / interval
  intervalDeg = 360 / unitDeg;
  tickH = 0;        //180 / (2*unitDeg);
  tickM = 170;       //90 / (2*unitDeg);
  tickL = 10;       //30;
  for(int i=0; i<360; i++)
    sin_lut[i] = sinf((float)(i) * 2.0f * 3.141592f / 360.0f);
  dataCount = 0;
}

void generateSin()
{
  extern osMessageQueueId_t waveQueueHandle;
  extern int isSineActive();
  float ymaxH, ymaxM, ymaxL;
  float value;

  if(readyCount++ < 30) return;

  if(isSineActive()){
    ymaxH = 2.0;  //2.0f
    ymaxM = 5.0;  //5.0f;
    ymaxL = 30.0;  //30.0f;
    value = (sin_lut[tickH*unitDeg] + 1.0f) * ymaxH;
    value += (sin_lut[tickM*unitDeg] + 1.0f) * ymaxM;
    value += (sin_lut[tickL*unitDeg] + 1.0f) * ymaxL;
    value += (float)(rand() % 20);
    wave.val[dataCount] = value;

//    if((tick % unitDeg) == 0)     tickL++;
//    if((tick % (unitDeg/2)) == 0) tickM++;
    tickL++;
    tickM += 10;
    tickH += 45;
    tick++;

    tickL %= intervalDeg;
    tickM %= intervalDeg;
    tickH %= intervalDeg;
    tick  %= intervalDeg;

    if(++dataCount == 5){
      wave.n = dataCount;

      osStatus_t  s;
      if((s = osMessageQueuePut(waveQueueHandle, &wave, 0U, 0)) == osOK){
#if 0
        printf("--- Put Queue ---\n");
        for(int i=0; i<5; i++)
          printf("[%d] = %d, ", i, (int)wave.val[i]);
        printf("\n");
#endif
      } else if(s == osErrorResource){
        printf("Queue is not ready(%d)\n", s);
      }
      dataCount = 0;
//      osThreadYield();
    }
  }
}



