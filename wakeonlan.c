#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BROADCAST_IP "255.255.255.255"
#define PORT 9

int main(int argc, char *argv[]) {
    // Read MAC address from the command line arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <MAC address>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *mac_address = argv[1];

    // Convert MAC address to bytes
    int mac_bytes_int[6];
    unsigned char mac_bytes[6];
    if (sscanf(mac_address, "%02x:%02x:%02x:%02x:%02x:%02x", &mac_bytes_int[0],
               &mac_bytes_int[1], &mac_bytes_int[2], &mac_bytes_int[3],
               &mac_bytes_int[4], &mac_bytes_int[5]) != 6) {
        fprintf(stderr, "Invalid MAC address format.\n");
        return EXIT_FAILURE;
    };
    for (int i = 0; i < 6; i++) {
        mac_bytes[i] = (unsigned char)mac_bytes_int[i];
    }

    // Create a magic packet
    unsigned char magic_packet[102];
    memset(magic_packet, 0xFF, 6);
    for (int i = 0; i < 16; i++) {
        memcpy(&magic_packet[6 + i * 6], mac_bytes, 6);
    }

    // Debug: print the magic packet
    /*
    for (int i = 0; i < 102; i++) {
        printf("%02X ", magic_packet[i]);
        if (i % 6 == 5) {
            printf("\n");
        }
    }
    //*/

    // Create a socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Failed to create a socket");
        return EXIT_FAILURE;
    }

    // Enable broadcast option on the socket
    int opt = 1;
    int setresult =
        setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
    if (setresult < 0) {
        perror("Failed to set the broadcast option on the socket");
        return EXIT_FAILURE;
    }

    // Configure the broadcast address
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, BROADCAST_IP, &addr.sin_addr) <= 0) {
        perror("Failed to convert the broadcast address");
        close(sock);
        return EXIT_FAILURE;
    }

    // Send the magic packet
    if (sendto(sock, magic_packet, sizeof(magic_packet), 0,
               (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Failed to send the magic packet");
        close(sock);
        return EXIT_FAILURE;
    };

    printf("Magic packet sent to %s\n", mac_address);
    close(sock);

    return EXIT_SUCCESS;
}
