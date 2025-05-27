#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
 
//Константы 
#define PROC_NAME "CAT"
#define MSG_SIZE 10

static int len,temp;
static char *msg;
 
ssize_t read_proc(struct file *filp, char *buf, size_t count, loff_t *offp ) {
    if(count > temp) {
        count = temp;
    }
    temp = temp - count;
    copy_to_user(buf, msg, count);
    if(count == 0)
        temp = len;
    return count;
}
 
ssize_t write_proc(struct file *filp, const char *buf, size_t count, loff_t *offp) {
    copy_from_user(msg, buf, count);
    len = count;
    temp = len;
    return count;
}
 
static const struct proc_ops proc_fops = {
    proc_read: read_proc,
    proc_write: write_proc,
};
 
void create_new_proc_entry(void) { //use of void for no arguments is compulsory now
    proc_create(PROC_NAME, 0, NULL, &proc_fops);
    msg = kmalloc(MSG_SIZE * sizeof(char), GFP_KERNEL);

}
 
int proc_init (void) {
    create_new_proc_entry();
    pr_info("proc module loaded\n");  // Сообщение при загрузке модуля
    return 0;
}
 
void proc_cleanup(void) {
    remove_proc_entry("hello", NULL);
    kfree(msg);
    pr_info("proc module removed\n");  // Сообщение при выгрузке модуля
}
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Glazunov Kirill");
module_init(proc_init);
module_exit(proc_cleanup);
