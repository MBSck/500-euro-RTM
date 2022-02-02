#ifndef CONTROLLER
#define CONTROLLER

void controllerStart();
void controllerLoop(void* unused);
uint16_t saturate16bit(int32_t input, int32_t min, int32_t max);

#endif