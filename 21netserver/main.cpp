/*
https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking
http://www.gabrielgambetta.com/fpm2.html
http://www.gabrielgambetta.com/fpm3.html
http://www.gabrielgambetta.com/fpm4.html
*/

#include "net.h"
#include "SDL.h"

struct PacketIncomingConnection
{
	uint16 protocol_id;
	uint32 hail;
	uint16 footer;
};

struct PacketPlayerUpdate
{
	uint16 protocol_id;
	uint32 key_state;
	uint16 footer;
};

struct NetPlayer
{
	int32 position_x;
	int32 position_y;
	uint32 key_state;
	uint64 last_tick;
	int32 timed_out;
};

bool IsLeftKeyDown(NetPlayer &player)
{
	#define LEFT_KEY_BIT 0
	return (player.key_state & (1 << LEFT_KEY_BIT)) != 0;
}

bool IsUpKeyDown(NetPlayer &player)
{
	#define UP_KEY_BIT 1
	return (player.key_state & (1 << UP_KEY_BIT)) != 0;
}

bool IsRightKeyDown(NetPlayer &player)
{
	#define RIGHT_KEY_BIT 2
	return (player.key_state & (1 << RIGHT_KEY_BIT)) != 0;
}

bool IsDownKeyDown(NetPlayer &player)
{
	#define DOWN_KEY_BIT 3
	return (player.key_state & (1 << DOWN_KEY_BIT)) != 0;
}

struct GameState
{
	NetPlayer players[SV_MAX_CONNECTIONS];
};

void HandleIncomingConnection(NetClient &sender, PacketIncomingConnection &packet)
{

}

void HandlePlayerUpdate(NetClient &sender, PacketPlayerUpdate &packet)
{

}

void UpdatePlayer(NetPlayer &player, float timestep)
{
	if (IsLeftKeyDown(player))
	{
		player.position_x -= 1;
	}
	else if (IsRightKeyDown(player))
	{
		player.position_x += 1;
	}

	if (IsUpKeyDown(player))
	{
		player.position_y -= 1;
	}
	else if (IsDownKeyDown(player))
	{
		player.position_y += 1;
	}
}

double GetElapsedTime(uint64 begin, uint64 end)
{
	uint64 f = SDL_GetPerformanceFrequency();
	return (double)(end - begin) / (double)f;
}

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		return EXIT_FAILURE;

	NetSetPreferredListenPort(SV_LISTEN_PORT);

	GameState game = {};

	bool running = true;
	double target_update_time = 1.0 / (double)SV_TICKRATE; 
	float  timestep = 1.0f / (float)SV_TICKRATE;

	uint64 sv_begin = SDL_GetPerformanceCounter();
	uint64 update_begin = SDL_GetPerformanceCounter();
	while (running)
	{
		uint64 sv_tick = SDL_GetPerformanceCounter() - sv_begin;

		NetClient src;
        uint8 buffer[256];
        int bytes_read = NetRead(buffer, 256, &src);
        while (bytes_read > 0)
        {
            printf("Read %d bytes from %d.%d.%d.%d:%d:\n", bytes_read, src.ip0, src.ip1, src.ip2, src.ip3, src.port);
            for (int i = 0; i < bytes_read; i++)
                printf("%04x ", buffer[i]);
            printf("\n");
        }

		//for (int i = 0; i < SV_MAX_CONNECTIONS; i++)
		//{
		//	double time_since_last_ping = GetElapsedTime(game.players[i].last_tick, update_begin);
		//	if (time_since_last_ping > (double)SV_TIMEOUT_SECONDS)
		//		game.players[i].timed_out = 1;
		//}

		uint64 update_end = SDL_GetPerformanceCounter();
		double elapsed_update_time = GetElapsedTime(update_begin, update_end);
		if (elapsed_update_time < target_update_time)
		{
			SDL_Delay(uint32(1000.0 * (target_update_time - elapsed_update_time)));
			update_end = SDL_GetPerformanceCounter();
			elapsed_update_time = GetElapsedTime(update_begin, update_end);
		}
		update_begin = update_end;
	}

	return EXIT_SUCCESS;
}