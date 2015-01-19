#ifndef NET_H
#define NET_H
#include "types.h"

#define SV_TICKRATE 20
#define CL_CMDRATE 20
#define CL_UPDATERATE 20
#define CL_INTERP 0.01
#define SV_LISTEN_PORT 27050
#define SV_MAX_CONNECTIONS 8
#define SV_TIMEOUT_SECONDS 10

struct NetClient
{
	union
	{
		struct
		{
			uint8  ip0, ip1, ip2, ip3; // IP address in form ip0.ip1.ip2.ip3
		};
		uint32 ip_bytes;
	};
    uint16 port;			   // Which port the client is listening to
};

void   NetSetPreferredListenPort(uint16 port);
bool   NetSend(NetClient *destination, uint8 *data, uint32 data_length);
uint32 NetRead(uint8 *data, uint32 max_packet_size, NetClient *sender);
void   NetClose();

#endif