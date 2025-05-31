#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define NETLINK_USER 31   // Константа для протокола Netlink
#define MAX_PAYLOAD 1024  // Максимальный размер полезной нагрузки

int main(int argc, char *argv[])
{
    int sock_fd;
    struct sockaddr_nl src_addr, dest_addr;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    struct msghdr msg;

    // Создаем Netlink-сокет
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0) {
        perror("Failed to create socket");
        return EXIT_FAILURE;
    }

    // Настраиваем адрес источника (текущий процесс)
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    if (bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
        perror("Failed to bind socket");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    // Настраиваем адрес получателя (ядро)
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;    // PID 0 для ядра
    dest_addr.nl_groups = 0; // Unicast

    // Выделяем память для сообщения
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    if (!nlh) {
        perror("Failed to allocate memory for message");
        close(sock_fd);
        return EXIT_FAILURE;
    }
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    // Записываем сообщение в буфер
    strcpy(NLMSG_DATA(nlh),argv[1] ? argv[1] : "Hello from user space!");

    // Настраиваем iovec для отправки данных
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;

    // Настраиваем msghdr
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;      // Указываем iovec для данных
    msg.msg_iovlen = 1;      // Количество элементов в iovec

    // Отправляем сообщение в ядро
    printf("Sending message to kernel: %s\n", (char *)NLMSG_DATA(nlh));
    if (sendmsg(sock_fd, &msg, 0) < 0) {
        perror("Failed to send message");
        free(nlh);
        close(sock_fd);
        return EXIT_FAILURE;
    }

    // Ожидаем ответа от ядра
    printf("Waiting for message from kernel\n");
    if (recvmsg(sock_fd, &msg, 0) < 0) {
        perror("Failed to receive message");
        free(nlh);
        close(sock_fd);
        return EXIT_FAILURE;
    }

    printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));

    free(nlh);
    close(sock_fd);
    return EXIT_SUCCESS;
}