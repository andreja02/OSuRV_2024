
#include <linux/module.h> // module_init(), module_exit()
#include <linux/fs.h> // file_operations
#include <linux/errno.h> // EFAULT
#include <linux/uaccess.h> // copy_from_user(), copy_to_user()

MODULE_LICENSE("Dual BSD/GPL");

#include "include/motor_ctrl.h"
#include "gpio.h"
#include "pwm.h"

#define MOTOR_CLTR__N_SERVO 1

#define DEFUALT_THRESHOLD 500

static int motor_ctrl_open(struct inode *inode, struct file *filp) {
	return 0;
}

static int motor_ctrl_release(struct inode *inode, struct file *filp) {
	return 0;
}


static ssize_t motor_ctrl_write(
	struct file* filp,
	const char *buf,
	size_t len,
	loff_t *f_pos
) {
	return 0;
}

#include "log.h"

log_t log[LOG_LEN];
size_t log_idx;

void add_to_log(
	uint64_t time,
	uint32_t idx,
	uint8_t on
) {
	//TODO Check if log_idx < LOG_LEN 
	if(log_idx  >= LOG_LEN) 
	{
		printk(KERN_ERR DEV_NAME": log_idx >= LOG_LEN \n");
		return;
	}
	//TODO Put new log entry to log at log_idx

	log[log_idx].time = time;
	log[log_idx].idx = idx;
	log[log_idx].on = on;
	log_idx++;

}

static ssize_t motor_ctrl_read(
	struct file* filp,
	char* buf,
	size_t len,
	loff_t* f_pos
) {
	
	size_t max_bytes = sizeof(log_t) * LOG_LEN;

	//EOF
	if (*f_pos >= max_bytes)
	{
		return 0; 
	}

	if (len > max_bytes - *f_pos)
	{
		len = max_bytes - *f_pos;
	}

	if (copy_to_user(buf, ((uint8_t*)log) + *f_pos, len) != 0) {
		return -EFAULT;
	} else {
		*f_pos += len;
		return len;
	}

}




loff_t motor_ctrl_llseek(
	struct file* filp,
	loff_t offset,
	int whence
) {
	switch(whence){
		case SEEK_SET:
			filp->f_pos = offset;
			break;
		case SEEK_CUR:
			filp->f_pos += offset;
			break;
		case SEEK_END:
			return -ENOSYS; // Function not implemented.
		default:
			return -EINVAL;
		}
	return filp->f_pos;
}

static struct file_operations motor_ctrl_fops = {
	open           : motor_ctrl_open,
	release        : motor_ctrl_release,
	read           : motor_ctrl_read,
	llseek         : motor_ctrl_llseek
};


void motor_ctrl_exit(void) {
	pwm__exit();
	gpio__exit();
	unregister_chrdev(DEV_MAJOR, DEV_NAME);
	
	printk(KERN_INFO DEV_NAME": Module removed.\n");
}

int motor_ctrl_init(void) {
	int r;
	uint8_t ch;

	printk(KERN_INFO DEV_NAME": Inserting module...\n");
	
	r = register_chrdev(DEV_MAJOR, DEV_NAME, &motor_ctrl_fops);
	if(r < 0){
		printk(KERN_ERR DEV_NAME": cannot obtain major number %d!\n", DEV_MAJOR);
		goto exit;
	}

	r = gpio__init();
	if(r){
		goto exit;
	}
	
	r = pwm__init();
	if(r){
		goto exit;
	}
	
	// 10us*2000 -> 20ms.
	for(ch = 0; ch < MOTOR_CLTR__N_SERVO; ch++){
		pwm__set_moduo(ch, 1000 << 1);
		pwm__set_threshold(ch, DEFUALT_THRESHOLD << 1);
	}
	
	
exit:
	if(r){
		printk(KERN_ERR DEV_NAME": %s() failed with %d!\n", __func__, r);
		motor_ctrl_exit();
	}else{
		printk(KERN_INFO DEV_NAME": Inserting module successful.\n");
	}
	return r;
}


module_init(motor_ctrl_init);
module_exit(motor_ctrl_exit);
