
#ifndef MOTOR_CLTR_H
#define MOTOR_CLTR_H

// For uint16_t.
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif


#define DEV_NAME "motor_ctrl"
#define DEV_FN "/dev/motor_ctrl"


typedef struct {
	uint64_t time;
	uint32_t idx;
	uint8_t on;
} log_t;

typedef struct {
	uint8_t ch;
	uint16_t moduo;
} motor_ctrl__ioctl_arg_moduo_t;

#define IOCTL_MOTOR_CLTR_SET_MODUO \
	_IOW('s', 0, motor_ctrl__ioctl_arg_moduo_t)

#define LOG_LEN 1000

#endif // MOTOR_CLTR_H
