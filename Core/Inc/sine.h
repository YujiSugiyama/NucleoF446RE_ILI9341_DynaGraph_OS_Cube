/*
 * sine.h
 *
 *  Created on: Aug 28, 2022
 *      Author: Yuji.Sugiyama
 */

#ifndef INC_SINE_H_
#define INC_SINE_H_

typedef struct{
  float val[5];
  uint16_t n;
} MSGQUEUE_WAVE_t;

void initSin();
void generateSin();
void infoDrawDone();

#endif /* INC_SINE_H_ */
