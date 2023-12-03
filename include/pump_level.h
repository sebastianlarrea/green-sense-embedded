#ifndef __PUMP_LEVEL_H__
#define __PUMP_LEVEL_H__

#define GPIO_PUMP_LEVEL 4

void pump_level_init(void);
static void pump_level_callback(uint, uint32_t);
bool get_pump_level(void);

#endif /* __PUMP_LEVEL_H__ */