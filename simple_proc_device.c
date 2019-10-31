#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sazonov Pavel");
MODULE_DESCRIPTION("A simple proc device");
MODULE_VERSION("0.1");

#define EXAMPLE_MSG "Hello world\n"
#define MSG_BUF_LEN 15

static ssize_t simple_read(struct file *, char *, size_t, loff_t *);
static ssize_t simple_write(struct file *, const char *, size_t, loff_t *); 

static int 	simple_major_num = 101;
static int	simple_size = MSG_BUF_LEN;
static char	simple_data[MSG_BUF_LEN];
struct proc_dir_entry *entry;

static const struct file_operations file_ops = {
    .owner	 = THIS_MODULE,
    .read	 = simple_read,
    .write	 = simple_write
};

static ssize_t simple_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    ssize_t ret;
    ret = simple_read_from_buffer(buf, len, offset, simple_data, simple_size);
    return ret;
}

static ssize_t simple_write(struct file *file, const char *buffer, size_t len, loff_t *offset)
{
    ssize_t ret;
    char *buf;
    buf = kmalloc(len + 1, GFP_KERNEL);
    if(!buf)
        return -ENOMEM;
    ret = simple_write_to_buffer(buf, len, offset, buffer, len);
    buf[len] = 0;
    if(ret > 0){
        printk(KERN_INFO "buf: %s", buf);
    }
    kfree(buf);
    return ret;
}

static int __init simple_device_init(void)
{
    printk(KERN_INFO "Loading simple device module\n");
    strncpy(simple_data, EXAMPLE_MSG, MSG_BUF_LEN);
    entry = proc_create("simple_device", 0666, NULL, &file_ops);
    if(!entry)
    {
        printk(KERN_ERR "simple device error proc create\n"); 
        return -ENOMEM;
    }
    printk(KERN_INFO "simple device module loaded with device major number: %d\n", simple_major_num);
    printk(KERN_INFO "Load simple device module successful\n");
    return 0;
}

static void __exit simple_device_exit(void)
{
    proc_remove(entry);
    printk(KERN_INFO "Simple device module unload\n");
}

module_init(simple_device_init);
module_exit(simple_device_exit);
