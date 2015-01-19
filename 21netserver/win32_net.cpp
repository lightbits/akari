#include "net.h"
#include <winsock2.h>
#pragma comment(lib, "wsock32.lib")

static uint32 g_socket = 0;
static bool   g_initialized = 0;
static uint16 g_preferred_port = 0;
static uint16 g_default_port = 27050;

void NetSetPreferredListenPort(uint16 port)
{
    g_preferred_port = port;
}

void NetClose()
{
    WSACleanup();
}

static
bool NetInitialize()
{
    WSADATA WsaData;
    if (WSAStartup(MAKEWORD(2, 2), &WsaData) != NO_ERROR)
    {
        printf("Windows failure\n");
        return false;
    }

    g_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (g_socket <= 0)
    {
        printf("Failed to create a socket\n");
        return false;
    }

    // Bind socket to a port
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    if (g_preferred_port)
        address.sin_port = htons(g_preferred_port);
    else
        address.sin_port = g_default_port;

    if (bind(g_socket, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0)
    {
        printf("Failed to bind socket\n");
        return false;
    }
    
    // Set port to not block when calling recvfrom
    DWORD non_blocking = 1;
    if (ioctlsocket(g_socket, FIONBIO, &non_blocking) != 0)
    {
        printf("Failed to set non-blocking\n");
        return false;
    }

    return true;
}

bool NetSend(NetClient *destination, uint8 *data, uint32 data_length)
{
    if (!g_initialized)
    {
        if (NetInitialize())
        {
            g_initialized = true;
        }
        else
        {
            return false;
        }
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl((destination->ip0 << 24) | 
                                    (destination->ip1 << 16) | 
                                    (destination->ip2 <<  8) | 
                                    (destination->ip3));
    address.sin_port = htons(destination->port);

    int sent_bytes = sendto(g_socket, (const char*)data,
                            data_length, 0, (sockaddr*)&address,
                            sizeof(sockaddr_in));

    return sent_bytes == data_length;
}

uint32 NetRead(uint8 *data, uint32 max_packet_size, NetClient *sender)
{
    if (!g_initialized)
    {
        if (NetInitialize())
        {
            g_initialized = true;
        }
        else
        {
            return 0;
        }
    }

    sockaddr_in from;
    int from_length = sizeof(from);
    int bytes_read = recvfrom(g_socket, (char*)data, max_packet_size, 0, (sockaddr*)&from, &from_length);
    if (bytes_read <= 0)
        return 0;

    uint32 from_address = ntohl(from.sin_addr.s_addr);
    sender->ip0 = (from_address >> 24) & 0xff;
    sender->ip1 = (from_address >> 16) & 0xff;
    sender->ip2 = (from_address >>  8) & 0xff;
    sender->ip3 = (from_address >>  0) & 0xff;
    sender->port = ntohs(from.sin_port);
    return bytes_read;
}
