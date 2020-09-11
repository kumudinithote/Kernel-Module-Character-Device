#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/errno.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>


MODULE_LICENSE("Dual BSD/GPL");

#define DEVICE_NAME "process_list"
#define BUFFER_SIZE 250


struct task_struct *task;
static struct task_struct * currentTask;
static int open(struct inode *, struct file *);
static ssize_t read(struct file *, char *, size_t, loff_t *);
static int release(struct inode *, struct file *);
static char* State(long);
char receive_buffer[BUFFER_SIZE];


static struct file_operations fops = {
  .owner = THIS_MODULE,
  .open  = open,
  .read  = read,
  .release = release
};

static struct miscdevice char_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "process_list",
    .fops = &fops
};

static int open(struct inode *i, struct file *f)
{
  printk(KERN_ALERT "process_list: Device is Opened.\n");
  currentTask=next_task(&init_task);
  return 0;
}

static ssize_t read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    //struct task_struct * task;

    memset(receive_buffer,0,sizeof(char)*BUFFER_SIZE);
    printk(KERN_ALERT "process_list: Reading Informations of processess \n");

    for_each_process(task)
    {
      if(currentTask==task)
      {
        //printk(" kumudini %s [%d]\n",task->comm , task->pid);
        char* state = State(task->state);
        memset(receive_buffer,0,sizeof(char)*BUFFER_SIZE);

        sprintf(receive_buffer, "PID=%d PPID=%d CPU=%d STATE=%s",task->pid,task->parent->pid,task_cpu(task),state);

        printk(KERN_ALERT "Sending data to user_interface: %s\n",receive_buffer);

        if(copy_to_user(buf, receive_buffer, strlen(receive_buffer)) !=0)
        {
  				printk(KERN_ALERT "process_list: Error in sending data to user!");
  				return -EFAULT;
  			}
      currentTask=next_task(task);
		  break;
     }
    }
    return strlen(receive_buffer);
}

static int release(struct inode *i, struct file *f)
{
    printk(KERN_ALERT "process_list: Device is closed! \n");
    currentTask = &init_task;
    return 0;
}

char* State(long state)
{
  switch(state)
  {
    //converted hexa to decimal
    case 0:
      return "TASK_RUNNING";

    case 1:
      return "TASK_INTERRUPTIBLE";

    case 2:
      return "TASK_UNINTERRUPTIBLE";

    case 4:
      return "_TASK_STOPPED";

    case 8:
      return "_TASKED_TRACED";

    case 15:
  		return "TASK_NORMAL , __TASK_STOPPED , __TASK_TRACED";

    case 16:
      return "EXIT_DEAD";

    case 32:
      return "EXIT_ZOMBIE";

    case 63:
  		return "TASK_RUNNING , TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE , __TASK_STOPPED , __TASK_TRACED , EXIT_ZOMBIE , EXIT_DEAD";

    case 64:
      return "TASK_PARKED";

    case 96:
  		return "EXIT_ZOMBIE , EXIT_DEAD";

    case 128:
      return "TASK_DEAD";

    case 130:
  		return "TASK_WAKEKILL , TASK_UNINTERRUPTIBLE";

    case 132:
  		return "TASK_WAKEKILL , __TASK_STOPPED";

  	case 136:
  		return "TASK_WAKEKILL , __TASK_TRACED";

    case 256:
      return "TASK_WAKEKIL";

    case 512:
      return "TASK_WAKING";

    case 1024:
      return "TASK_NOLOAD";

    case 1026:
  		return "TASK_UNINTERRUPTIBLE , TASK_NOLOAD";

    case 2048:
      return "TASK_NEW";

    case 4096:
      return "TASK_STATE_MAX";

    default:
		return "INVALID";
  }
}

// called when module is installed
int __init init_module(void)
{

    int ret;
    printk(KERN_ALERT "process_list: Hello World!\n");
    ret = misc_register(&char_device);
    if(ret < 0)
    {
      printk(KERN_ALERT "process_list: Registration has been failed!");
		  return 0;
    }

    currentTask=next_task(&init_task);
    return 0;

}
//called last to deregister the process list device.
void __exit cleanup_module(void)
{
    printk(KERN_ALERT "process_list: Goodbye, cruel world!!\n");
    misc_deregister(&char_device);
}
