
#include <linux/module.h> // module_init(), module_exit()
#include <linux/fs.h> // file_operations
#include <linux/errno.h> // EFAULT
#include <linux/uaccess.h> // copy_from_user(), copy_to_user()

MODULE_LICENSE("Dual BSD/GPL");

#include "include/motor_ctrl.h"
#include "pwm.h"




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

log_t log[LOG_LEN];

static ssize_t motor_ctrl_read(
	struct file* filp,
	char* buf,
	size_t len,
	loff_t* f_pos
) {
	log_t log_entry = {
		.time = 0xbabadeda,
		.idx = 101,
		.on = 0
	};
	
	if(copy_to_user(buf, (uint8_t*)&log_entry + *f_pos, len) != 0){
		return -EFAULT;
	}else{
		*f_pos += len;
		return len;
	}
}


static long motor_ctrl_ioctl(
	struct file* filp,
	unsigned int cmd,
	unsigned long arg
) {
	motor_ctrl__ioctl_arg_moduo_t ia;
	ia = *(motor_ctrl__ioctl_arg_moduo_t*)&arg;
	
	switch(cmd){
		case IOCTL_MOTOR_CLTR_SET_MODUO:
			pwm__set_moduo(ia.ch, ia.moduo);
			break;
		default:
			break;
	}

	return 0;
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
