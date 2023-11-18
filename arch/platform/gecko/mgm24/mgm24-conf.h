#ifndef MGM240GB32VNN_CONF_H_
#define MGM240GB32VNN_CONF_H_


#include "em_gpio.h"
/*---------------------------------------------------------------------------*/
#define PLATFORM_HAS_BUTTON             1
#define PLATFORM_SUPPORTS_BUTTON_HAL    1
/*---------------------------------------------------------------------------*/
#define GECKO_BUTTON1_PIN     6
#define GECKO_BUTTON1_PORT    gpioPortD
#define GECKO_BUTTON2_PIN     7
#define GECKO_BUTTON2_PORT    gpioPortD
/*---------------------------------------------------------------------------*/
#define GECKO_LED1_PIN        4
#define GECKO_LED1_PORT       gpioPortD
#define GECKO_LED2_PIN        5
#define GECKO_LED2_PORT       gpioPortD
/*---------------------------------------------------------------------------*/
#define LEDS_CONF_COUNT     2

#endif  /* MGM240GB32VNN_CONF_H_ */