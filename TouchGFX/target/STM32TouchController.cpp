/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : STM32TouchController.cpp
  ******************************************************************************
  * This file is generated by TouchGFX Generator 4.20.0.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* USER CODE BEGIN STM32TouchController */

#include <STM32TouchController.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <stdio.h>
#include "ili9341.h"
#include "xpt2046.h"

void STM32TouchController::init()
{
    /**
     * Initialize touch controller and driver
     *
     */
  touchgfx::HAL::getInstance()->setTouchSampleRate(1);
}

bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
{
  /**
   * By default sampleTouch returns false,
   * return true if a touch has been detected, otherwise false.
   *
   * Coordinates are passed to the caller by reference by x and y.
   *
   * This function is called by the TouchGFX framework.
   * By default sampleTouch is called every tick, this can be adjusted by HAL::setTouchSampleRate(int8_t);
   *
  */
  static uint16_t prevx = GUI_WIDTH;
  static uint16_t prevy = GUI_HEIGHT;
  uint16_t intx, inty, convx, convy;
  int32_t intz;

//  HAL_GPIO_TogglePin(GPIOC, TickOut_Pin);     // PC3

  XPT2046_Update(&intx, &inty, &intz);
  convx = intx;
  convy = inty;

  if (XPT2046_IsReasonable(convx, convy)) {
    XPT2046_XPTtoILI(&convx, &convy);
    if (convx != prevx || convy != prevy) {
      prevx = convx;
      prevy = convy;
      x = (int32_t)convx;
      y = (int32_t)convy;

      printf("x=%d, y=%d, z=%d\n", (int)x, (int)y, (int)intz);

      return true;
    }
  }

  return false;
}

/* USER CODE END STM32TouchController */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/