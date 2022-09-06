#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/Utils.hpp>
#include <stdio.h>
#include <math.h>
#include <BitmapDatabase.hpp>

#ifndef SIMULATOR
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#endif


Screen1View::Screen1View()
{
  graphActive = 1;

#ifndef SIMULATOR
  waveReady = 0;
#else
  waveCount = 0;
  for(int i=0; i<360; i++)
    sin_lut[i] = sinf((float)(i) * 2.0f * 3.141592f / 360.0f * 2.0f);
#endif
}

void Screen1View::setupScreen()
{
  Screen1ViewBase::setupScreen();
  presenter->pause(graphActive);
}

void Screen1View::tearDownScreen()
{
  Screen1ViewBase::tearDownScreen();
}

void Screen1View::graphClicked(AbstractDataGraph::GraphClickEvent value)
{
  if(value.clickEvent.getType() == ClickEvent::PRESSED){
    int x, y;
    x = value.index;
    y = dynamicGraph1.indexToDataPointYAsInt(x);
    x *= dynamicGraph1.getXAxisFactorAsInt();
    touchgfx_printf("(%d, %d)\n", x, y);
    Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%d", y);
    textArea1.invalidate();
  }
}

void Screen1View::graphPause()
#ifndef SIMULATOR
{
  extern TIM_HandleTypeDef htim3;

  __HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
  graphActive ^= 1;
  presenter->pause(graphActive);
  if(graphActive == 1)
    flexButton1.setIconBitmaps(Bitmap(BITMAP_DARK_ICONS_PAUSE_32_ID), Bitmap(BITMAP_DARK_ICONS_PAUSE_32_ID));
  else
    flexButton1.setIconBitmaps(Bitmap(BITMAP_DARK_ICONS_PLAY_32_ID), Bitmap(BITMAP_DARK_ICONS_PLAY_32_ID));
  __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
}
#else
{
  graphActive ^= 1;
  if(graphActive == 1)
    flexButton1.setIconBitmaps(Bitmap(BITMAP_DARK_ICONS_PAUSE_32_ID), Bitmap(BITMAP_DARK_ICONS_PAUSE_32_ID));
  else
    flexButton1.setIconBitmaps(Bitmap(BITMAP_DARK_ICONS_PLAY_32_ID), Bitmap(BITMAP_DARK_ICONS_PLAY_32_ID));
}
#endif

void Screen1View::putData(float* wave, int n)
{
#ifndef SIMULATOR
//  extern TIM_HandleTypeDef htim3;

//  __HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
  for(int i=0; i<n; i++)
    temp[i] = wave[i];
  tempCount = n;
  waveReady = 1;
//  __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
#endif
}

void Screen1View::handleTickEvent()
{
#ifndef SIMULATOR
//  extern TIM_HandleTypeDef htim3;

//  __HAL_TIM_DISABLE_IT(&htim3, TIM_IT_UPDATE);
  if(waveReady == 1){
    for(int i=0; i<tempCount; i++){
      dynamicGraph1.addDataPoint(temp[i]);
    }
    waveReady = 0;
  }
//  __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
#else
  if(graphActive){
    dynamicGraph1.addDataPoint((sin_lut[waveCount++] + 1.0f) * 40.0f);
    waveCount %= 360;
  }
#endif
}



