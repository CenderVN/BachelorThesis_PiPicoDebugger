#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


/* Compatibility glue for RP2350 / SDK 2.2.0 ARM */
#ifndef SIO_IRQ_PROC0
    #define SIO_IRQ_PROC0 26
#endif
#ifndef SIO_IRQ_PROC1
    #define SIO_IRQ_PROC1 27
#endif

// This example uses a common include to avoid repetition
#include "FreeRTOSConfig_examples_common.h"


#endif