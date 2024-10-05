#ifndef DELAY_H
#define DELAY_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void delay_init(void);
extern void delay_us(uint32_t nus);
extern void delay_ms(uint32_t nms);

#ifdef __cplusplus
}
#endif

#endif
