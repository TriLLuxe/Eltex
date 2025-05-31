#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/net_namespace.h>

#define NETLINK_USER 31 // Константа для протокола Netlink

static struct sock * netlink_socket = NULL; // Сокет Netlink

// Функция для обработки входящих сообщений от пользовательского пространства
static void handle_incoming_message(struct sk_buff *skb){
    struct nlmsghdr *nlh;
    int sender_pid; // PID отправителя
    struct sk_buff * reply_skb;
    int msg_size;
    char *reply_msg = "Hello from kernel space!";
    int res;

    printk(KERN_INFO,"Entering: %s\n", __FUNCTION__);

    msg_size = strlen(reply_msg);

    //Извлекаем заголовок Netlink сообщения
    nlh = (struct nlmsghdr *)skb->data;

    // Получаем PID отправителя
    sender_pid = nlh->nlmsg_pid; 

    printk(KERN_INFO "Netlink received msg payload from process %d: %s\n", 
           sender_pid, (char *)nlmsg_data(nlh));
    
    // Создаем ответный буфер
    reply_skb = nlmsg_new(msg_size, 0);
    if (!reply_skb) {
        printk(KERN_ERR "Failed to allocate skb for reply\n");
        return;
    }

    // Заполняем заголовок Netlink сообщения
    nlh = nlmsg_put(reply_skb, 0, 0, NLMSG_DONE, msg_size, 0);
    if (!nlh) {
        printk(KERN_ERR "Failed to put nlmsghdr in skb\n");
        kfree_skb(reply_skb);
        return;
    }

    //Флаг для unicast сообщения
    NETLINK_CB(reply_skb).dst_group = 0;

    // Копируем ответное сообщение в буфер
    memcpy(nlmsg_data(nlh), reply_msg, msg_size);

    //Отправка
    res = nlmsg_unicast(netlink_socket, reply_skb, sender_pid);
    if (res < 0) {
        printk(KERN_INFO "Error while sending reply to user: %d\n", res);
    }
}

// Конфигурация для создания Netlink-сокета
struct netlink_kernel_cfg cfg = {
    .groups = 1,  
    .input = handle_incoming_message,  // Функция обработки входящих сообщений
};

static int __init _init(void)
{
    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);
    
    // Создаем Netlink-сокет
    netlink_socket = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!netlink_socket) {
        printk(KERN_ALERT "Error creating Netlink socket.\n");
        return -EIO;  // Возвращаем ошибку ввода/вывода
    }
    
    return 0;
}

static void __exit _exit(void)
{
    printk(KERN_INFO "Exiting: %s\n", __FUNCTION__);
    
    // Удаляем Netlink-сокет
    if (netlink_socket) {
        netlink_kernel_release(netlink_socket);
        netlink_socket = NULL;
    }
}

module_init(_init);
module_exit(_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Glazunov Kirill");
