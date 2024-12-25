/* macchanger.c */
#define _GNU_SOURCE
#define __USE_MISC

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef unsigned char int8;
typedef unsigned long long int int64;

struct s_mac {
    int64 addr;
};

typedef struct s_mac Mac;

bool change_mac(const int8 *, Mac);
Mac generate_mac(void);
bool set_interface_state(const int8 *, bool);

Mac generate_mac() {
    int64 a, b;
    Mac mac;

    a = (int64)rand();
    b = (int64)rand();
    mac.addr = ((a * b) & 0xFFFFFFFFFFFFULL); // Mask to 48 bits

    // Ensure it's a valid locally administered unicast MAC address
    mac.addr = (mac.addr & 0xFEFFFFFFFFFFFFULL) | 0x020000000000ULL;

    return mac;
}

bool set_interface_state(const int8 *If, bool up) {
    char command[128];
    snprintf(command, sizeof(command), "ip link set %s %s", If, up ? "up" : "down");
    int ret = system(command);
    return (ret == 0);
}

bool change_mac(const int8 *If, Mac mac) {
    struct ifreq ir;
    int fd, ret;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return false;
    }

    strncpy(ir.ifr_name, (const char *)If, IF_NAMESIZE - 1);
    ir.ifr_name[IF_NAMESIZE - 1] = '\0';

    ir.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    for (int i = 0; i < 6; i++) {
        ir.ifr_hwaddr.sa_data[i] = (mac.addr >> (8 * (5 - i))) & 0xFF;
    }

    ret = ioctl(fd, SIOCSIFHWADDR, &ir);
    if (ret < 0) {
        perror("ioctl");
        close(fd);
        return false;
    }

    close(fd);
    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s INTERFACE\n", argv[0]);
        return EXIT_FAILURE;
    }

    const int8 *If = (const int8 *)argv[1];
    Mac mac;

    srand((unsigned int)getpid());
    mac = generate_mac();

    if (!set_interface_state(If, false)) {
        fprintf(stderr, "Failed to bring down interface %s.\n", If);
        return EXIT_FAILURE;
    }

    if (change_mac(If, mac)) {
        printf("MAC address changed successfully to: %02llx:%02llx:%02llx:%02llx:%02llx:%02llx\n",
               (mac.addr >> 40) & 0xFF,
               (mac.addr >> 32) & 0xFF,
               (mac.addr >> 24) & 0xFF,
               (mac.addr >> 16) & 0xFF,
               (mac.addr >> 8) & 0xFF,
               mac.addr & 0xFF);
    } else {
        fprintf(stderr, "Failed to change MAC address.\n");
        set_interface_state(If, true); // Try to bring the interface up
        return EXIT_FAILURE;
    }

    if (!set_interface_state(If, true)) {
        fprintf(stderr, "Failed to bring up interface %s.\n", If);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
