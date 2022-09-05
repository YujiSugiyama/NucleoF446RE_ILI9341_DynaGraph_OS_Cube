#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <stdio.h>

void shareModelInstance(Model *m);

Model::Model() : modelListener(0)
{
#ifndef SIMULATOR
  shareModelInstance(this);
#endif
}

void Model::tick()
{

}

void Model::putData(float* wave, int n)
{
#if 0
  printf("-----------------------------------\n");
  printf("count = %d : data", n);
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
extern "C" void putData(float *data, int n)
{
  pmodel->putData(data, n);
}
#endif



