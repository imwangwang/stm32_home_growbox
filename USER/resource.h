#ifndef _RESOURCE_H
#define _RESOURCE_H

/* How many sensors we are expecting on 1wire bus? */
#define EXPECTING_SENSORS	2

enum {
	RES_RTC = 0,
	RES_TMP = 1,
	RES_AIR_HUMDITY = 2,
	RES_SOIL_HUMDITY = 3,
	RES_REAR_GROW_LIGHT = 4,
	RES_REAR_WARM_LIGHT = 5,
	RES_ALARM = 6,
	RES_USART = 7,
	RES_LIGHT = 8,
	RES_PUMPA = 9,
	RES_PUMPB = 10
};

typedef int (*handle_get)(void *data);
typedef int (*handle_set)(void *data);

struct resource {
	const char id_name[11];
	uint8_t id_type;
	handle_get get;
	handle_set set;
};

enum {
	CMD_INVALID = 0,
	CMD_START_PUMPA = 1,
	CMD_REAR_GROWLIGHT_ON,
	CMD_REAR_GROWLIGHT_OFF,
	CMD_HUMDITY_PERIOD_TIMER,
	CMD_LIGHT_PERIOD_TIMER,
	CMD_TMP_PERIOD_TIMER,
	CMD_PUMPA_STOP,
	};


extern const struct resource *res_array[];
#define get_res_from_array(x) res_array[x]

#endif
