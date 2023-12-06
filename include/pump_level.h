#ifndef __PUMP_LEVEL_H__
#define __PUMP_LEVEL_H__

void pump_level_init(uint8_t);
static void pump_level_callback(uint, uint32_t);
bool get_pump_level(uint8_t);

#endif /* __PUMP_LEVEL_H__ */