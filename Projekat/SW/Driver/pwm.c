
#include "pwm.h"


#include "sw_pwm.h"
#include "include/motor_ctrl.h" // MOTOR_CLTR__N_SERVO

int pwm__init(void) {
	int r;
	
	r = sw_pwm__init();
	if(r){
		goto exit;
	}

exit:
	if(r){
		pwm__exit();
	}
	return 0;
}

void pwm__exit(void) {
	sw_pwm__exit();
}


void pwm__set_moduo(pwm__ch_t ch, uint32_t moduo) {
	sw_pwm__set_moduo(
		ch,
		moduo
	);
}

void pwm__set_threshold(pwm__ch_t ch, uint32_t threshold) {
	sw_pwm__set_threshold(
		ch,
		threshold
	);
}
