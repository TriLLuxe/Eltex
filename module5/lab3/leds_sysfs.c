#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>          
#include <linux/kd.h>           
#include <linux/vt.h>
#include <linux/console_struct.h> 
#include <linux/vt_kern.h>
#include <linux/timer.h>
#include <linux/kobject.h>      
#include <linux/sysfs.h>        
#include <linux/string.h>

MODULE_DESCRIPTION("Модуль для управления миганием светодиодов клавиатуры через sysfs.");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Glazunov Kirill");

struct timer_list my_timer;
struct tty_driver *my_driver;
static struct kobject *kbleds_kobject;
static int led_mask = 0; 
static int kbledstatus = 0; /* Текущее состояние светодиодов (переключается между led_mask и RESTORE_LEDS) */

#define BLINK_DELAY   HZ/5
#define ALL_LEDS_ON   0x07
#define RESTORE_LEDS  0xFF

/* Функция чтения sysfs: отображает текущее значение led_mask */
static ssize_t led_mask_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", led_mask);
}

/* Функция обновляет led_mask на основе пользовательского ввода */
static ssize_t led_mask_store(struct kobject *kobj, struct kobj_attribute *attr,
                              const char *buf, size_t count)
{
    int new_mask;
    if (sscanf(buf, "%d", &new_mask) != 1)
        return -EINVAL;
    if (new_mask < 0 || new_mask > ALL_LEDS_ON)
        return -EINVAL;
    led_mask = new_mask;
    return count;
}


static struct kobj_attribute led_mask_attribute = __ATTR(led_mask, 0660, led_mask_show, led_mask_store);

/* Функция таймера для переключения светодиодов */
static void my_timer_func(struct timer_list *ptr)
{
    if (kbledstatus == led_mask)
        kbledstatus = RESTORE_LEDS; /* Выключить светодиоды */
    else
        kbledstatus = led_mask;     /* Включить светодиоды согласно маске */
    
    /* Выполнить ioctl для обновления состояния светодиодов */
    if (my_driver && vc_cons[fg_console].d && vc_cons[fg_console].d->port.tty)
        (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, kbledstatus);
    
    /* Перепланировать таймер */
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
}

static int __init kbleds_init(void)
{
    int i, error;

    printk(KERN_INFO "kbleds: загрузка\n");
    printk(KERN_INFO "kbleds: fgconsole is %x\n", fg_console);
    
    /* Сканирование для поиска активного tty */
    for (i = 0; i < MAX_NR_CONSOLES; i++) {
        if (!vc_cons[i].d)
            break;
        printk(KERN_INFO "kbleds: console[%i/%i] #%i, tty %lx\n", i,
               MAX_NR_CONSOLES, vc_cons[i].d->vc_num,
               (unsigned long)vc_cons[i].d->port.tty);
    }
    printk(KERN_INFO "kbleds: завершено сканирование консолей\n");

    /* Получение драйвера для консоли */
    if (!vc_cons[fg_console].d || !vc_cons[fg_console].d->port.tty) {
        printk(KERN_ERR "kbleds: не найден валидный tty для fgconsole %d\n", fg_console);
        return -ENODEV;
    }
    my_driver = vc_cons[fg_console].d->port.tty->driver;
    printk(KERN_INFO "kbleds: tty driver initialized\n");

    /* Инициализация интерфейса sysfs */
    kbleds_kobject = kobject_create_and_add("kbleds", kernel_kobj);
    if (!kbleds_kobject) {
        printk(KERN_ERR "kbleds: не удалось создать kobject\n");
        return -ENOMEM;
    }

    error = sysfs_create_file(kbleds_kobject, &led_mask_attribute.attr);
    if (error) {
        printk(KERN_ERR "kbleds: не удалось создать файл sysfs\n");
        kobject_put(kbleds_kobject);
        return error;
    }

    /* Инициализация таймера для мигания светодиодов */
    timer_setup(&my_timer, my_timer_func, 0);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);

    return 0;
}

static void __exit kbleds_cleanup(void)
{
    printk(KERN_INFO "kbleds: выгрузка...\n");
    
    /* Остановка таймера и восстановление светодиодов */
    del_timer(&my_timer);
    if (my_driver && vc_cons[fg_console].d && vc_cons[fg_console].d->port.tty)
        (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);

    kobject_put(kbleds_kobject);
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);