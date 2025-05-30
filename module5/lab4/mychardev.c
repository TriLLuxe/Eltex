#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/atomic.h>

#define DEVICE_NAME "mychardev"  // Имя устройства
#define BUF_LEN 80               


static int major;// Старший номер устройства
static atomic_t already_open = ATOMIC_INIT(0);  
static char user_msg[BUF_LEN]; // Буфер для пользовательских данных
static char full_msg[BUF_LEN * 2];       
static struct class *cls;               


static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);


static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};


static int __init chardev_init(void) {
      // Регистрация символьного устройства
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_alert("Registering char device failed with %d\n", major);
        return major;
    }
    pr_info("I was assigned major number %d.\n", major);

  
    cls = class_create(DEVICE_NAME);
    if (IS_ERR(cls)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(cls);
    }

    // Создание файла устройства в /dev
    if (IS_ERR(device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME))) {
        class_destroy(cls);
        unregister_chrdev(major, DEVICE_NAME);
        return -1;
    }
    pr_info("Device created on /dev/%s\n", DEVICE_NAME);
    return 0;
}


static void __exit chardev_exit(void) {
    device_destroy(cls, MKDEV(major, 0));    
    class_destroy(cls);                      
    unregister_chrdev(major, DEVICE_NAME);   
    pr_info("Device unregistered\n");
}


static int device_open(struct inode *inode, struct file *file) {
    if (atomic_cmpxchg(&already_open, 0, 1)) {
        return -EBUSY;  // Устройство уже открыто
    }

    
    
    try_module_get(THIS_MODULE);  
    return 0;
}


static int device_release(struct inode *inode, struct file *file) {
    atomic_set(&already_open, 0);  
    module_put(THIS_MODULE);      
    return 0;
}


static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
    static int counter = 0;
    if (*offset != 0)
        return 0;

    counter++;
    snprintf(full_msg, sizeof(full_msg), "Read %d times: %s", counter, user_msg);
    size_t msg_len = strlen(full_msg);

    size_t to_copy = min(length, msg_len);

    if (copy_to_user(buffer, full_msg, to_copy))
        return -EFAULT;

    *offset += to_copy;
    return to_copy;
}

static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off) {
    if (len > BUF_LEN - 1) {
        len = BUF_LEN - 1;
    }

    int ret = copy_from_user(user_msg, buff, len);
    if (ret) {
        return -EFAULT;  // Ошибка копирования
    }

    user_msg[len] = '\0';  
    return len;            
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Glazunov Kirill");