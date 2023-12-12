#ifndef __APP_TYPES_H__
#define __APP_TYPES_H__

#include <stdint.h>
#include <stdbool.h>

typedef union {
    uint8_t flags;
    struct {
        bool send_sensors : 1;
        bool start_water_plant : 1;
        bool stop_water_plant : 1;
        uint8_t : 5;
    };
} flags_t;  /*!<Tipo para las flags*/

extern volatile flags_t green_sense_flags; /*!<Flags de la aplicacion*/
#endif // __APP_TYPES_H__