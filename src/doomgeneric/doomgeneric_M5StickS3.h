#ifndef DOOMGENERIC_M5STICKS3_H
#define DOOMGENERIC_M5STICKS3_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void DG_Init(void);
    void DG_DrawFrame(void);
    void DG_SleepMs(uint32_t ms);
    uint32_t DG_GetTicksMs(void);
    int DG_GetKey(int *pressed, unsigned char *key);
    void DG_SetWindowTitle(const char *title);

#ifdef __cplusplus
}
#endif

#endif