#include "contiki.h"
#include "gecko_heart_rate.h"
#include "heart_rate_sensor.h"

const struct sensors_sensor heart_rate_sensor;

/** 
* \brief Returns device heart_rate
* \param type ignored
* \return Device heart_rate
*/
static int value(int type){
    int32_t volatile temp;


    return temp;
}

/**
 * @brief Configures data sensor
 * @param type initializes the hardware sensor when \a type is set to 
 *                      \a SENSOR_HW_INIT
 * @param c ignored
 * @return 1
 */
static int configure(int type, int c){
    if (type == SENSOR_HW_INIT){
        gecko_sensor_init();
    }
    return 1;
}

/**
 * @brief Return data sensor status
 * @param type ignord
 * @return 1
 */
static int status(int type){
    return 1;
}

SENSORS_SENSOR(heart_rate_sensor, HEART_RATE_SENSOR, value, configure, status);