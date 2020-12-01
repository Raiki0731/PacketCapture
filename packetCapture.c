#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "networkheader.h"

void dsp_ether_header(ETHERHEADER ether);
void dsp_arp_protocol(ARPDATA arp);
void dsp_ip_header(IPHEADER ip);
void dsp_icmp_protocol(ICMPDATA icmp);
void dsp_udp_protocol(UDPHEADER udp);
void dsp_tcp_protocol(TCPHEADER tcp);

int main(int argc, char *argv[])
{
    int sock;
    int byte;
    char buf[1024 * 128];
    int etherSize;
    char tmp[sizeof(int)];

    struct sockaddr_ll sll;
    ETHERHEADER ether;
    IPHEADER ip;
    ARPDATA arp;
    ICMPDATA icmp;
    UDPHEADER udp;
    TCPHEADER tcp;

    if ((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    {
        perror("socket");
        exit(1);
    }

    memset(&sll, 0, sizeof(sll));
    sll.sll_family = PF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL);
    if (bind(sock, (struct sockaddr *)&sll, sizeof(sll)) < 0)
    {
        perror("bind");
        exit(1);
    }

    int i;
    while (1)
    {
        if ((byte = read(sock, buf, sizeof(buf))) < 0)
        {
            perror("read");
            exit(1);
        }

        etherSize = sizeof(ETHERHEADER);
        memcpy(&ether, buf, etherSize);

        if (ether.type[0] == 0x08 && ether.type[1] == 0x06)
        {
            dsp_ether_header(ether);
            printf("    %4d ", byte);
            printf("%-4s ", "ARP");

            memcpy(&arp, buf + etherSize, sizeof(ARPDATA));
            dsp_arp_protocol(arp);

            printf("\n");
        }
        else if (ether.type[0] == 0x08 && ether.type[1] == 0x00)
        {
            for (i = 0; i < 4; i++)
            {
                ip.srcIP[i] = buf[26 + i];
                ip.destIP[i] = buf[30 + i];
            }
            ip.protocol = buf[23];
            dsp_ip_header(ip);
            printf("  ");

            switch (ip.protocol)
            {
            case 0x01:
                printf("  %4d ", byte);
                printf("%-4s ", "ICMP");

                icmp.type = buf[34];
                tmp[0] = buf[38];
                tmp[1] = buf[39];
                memcpy(&icmp.id, tmp, sizeof(icmp.id));
                icmp.seq = buf[41];

                dsp_icmp_protocol(icmp);

                break;

            case 0x11:
                printf("  %4d ", byte);
                printf("%-4s ", "UDP");

                tmp[0] = buf[34];
                tmp[1] = buf[35];
                memcpy(&udp.srcPort, tmp, sizeof(udp.srcPort));

                tmp[0] = buf[36];
                tmp[1] = buf[37];
                memcpy(&udp.destPort, tmp, sizeof(udp.destPort));

                tmp[0] = buf[38];
                tmp[1] = buf[39];
                memcpy(&udp.length, tmp, sizeof(udp.length));

                dsp_udp_protocol(udp);

                break;

            case 0x06:
                printf("  %4d ", byte);
                printf("%-4s ", "TCP");

                tmp[0] = buf[34];
                tmp[1] = buf[35];
                memcpy(&tcp.srcPort, tmp, sizeof(tcp.srcPort));

                tmp[0] = buf[36];
                tmp[1] = buf[37];
                memcpy(&tcp.destPort, tmp, sizeof(tcp.destPort));

                tcp.signal = buf[47];

                for (i = 0; i < 4; i++)
                {
                    tmp[i] = buf[38 + i];
                }
                memcpy(&tcp.seq, tmp, sizeof(tcp.seq));

                for (i = 0; i < 4; i++)
                {
                    tmp[i] = buf[42 + i];
                }
                memcpy(&tcp.ack, tmp, sizeof(tcp.ack));

                memset(tcp.data, '\0', sizeof(tcp.data));
                for (i = 0; i < 14; i++)
                {
                    tcp.data[i] = buf[66+i];
                }
                
                dsp_tcp_protocol(tcp);

                break;

            default:
                break;
            }
        }
    }

    return 0;
}

void dsp_ether_header(ETHERHEADER ether)
{
    int i;
    printf("%02x", ether.srcMAC[0]);
    for (i = 1; i < 5; i++)
    {
        printf(":%02x", ether.srcMAC[i]);
    }
    printf(" > ");
    printf("%02x", ether.destMAC[i]);
    for (i = 1; i < 5; i++)
    {
        printf(":%02x", ether.destMAC[i]);
    }

    fflush(stdout);
}

void dsp_arp_protocol(ARPDATA arp)
{
    int i;
    if (ntohs(arp.opcode) == 1)
    {
        printf("Who has %d", arp.targetIP[0]);
        for (i = 0; i < 3; i++)
        {
            printf(".%d", arp.targetIP[i]);
        }
    }
    else if (ntohs(arp.opcode) == 2)
    {
        printf("%d", arp.senderIP[0]);
        for (i = 0; i < 3; i++)
        {
            printf(".%d", arp.senderIP[i]);
        }
        printf(" is at %02x", arp.senderMAC[0]);
        for (i = 1; i < 5; i++)
        {
            printf(":%02x", arp.senderMAC[i]);
        }
    }

    fflush(stdout);
}

void dsp_ip_header(IPHEADER ip)
{
    int i;
    printf("%d", ip.srcIP[0]);
    for (i = 0; i < 3; i++)
    {
        printf(".%d", ip.srcIP[i]);
    }
    printf(" > %d", ip.destIP[0]);
    for (i = 0; i < 3; i++)
    {
        printf(".%d", ip.destIP[i]);
    }

    fflush(stdout);
}

void dsp_icmp_protocol(ICMPDATA icmp)
{
    switch (icmp.type)
    {
    case 0x08:
        printf("Echo (ping) request id=0x%04x, seq=%d\n", ntohs(icmp.id), icmp.seq);
        break;
    case 0x00:
        printf("Echo (ping) reply id=0x%04x, seq=%d\n", ntohs(icmp.id), icmp.seq);
        break;
    case 0x03:
        printf("Destination Unreachable id=0x%04x, seq=%d\n", ntohs(icmp.id), icmp.seq);
        break;
    default:
        break;
    }
}

void dsp_udp_protocol(UDPHEADER udp)
{
    printf("%d -> %d Len=%d\n", ntohs(udp.srcPort), ntohs(udp.destPort), ntohs(udp.length));
}

void dsp_tcp_protocol(TCPHEADER tcp)
{
    printf("%d -> %d ", ntohs(tcp.srcPort), ntohs(tcp.destPort));

    switch (tcp.signal)
    {
    case 0x10:
        printf("[ACK] seq=%08x ack=%08x  %s\n", ntohl(tcp.seq), ntohl(tcp.ack), tcp.data);
        break;

    case 0x02:
        printf("[SYN] seq=%08x ack=%08x  %s\n", ntohl(tcp.seq), ntohl(tcp.ack), tcp.data);
        break;

    case 0x01:
        printf("[FIN] seq=%08x ack=%08x  %s\n", ntohl(tcp.seq), ntohl(tcp.ack), tcp.data);
        break;

    case 0x12:
        printf("[SYN, ACK] seq=%08x ack=%08x  %s\n", ntohl(tcp.seq), ntohl(tcp.ack), tcp.data);
        break;
    
    case 0x18:
        printf("[PSH, ACK] seq=%08x ack=%08x  %s\n", ntohl(tcp.seq), ntohl(tcp.ack), tcp.data);
        break;

    case 0x11:
        printf("[FIN, ACK] seq=%08x ack=%08x  %s\n", ntohl(tcp.seq), ntohl(tcp.ack), tcp.data);
        break;

    default:
        printf("\n");
        break;
    }
}