
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


struct WRQ
{
	uint16_t opcode;
	char file[100];
	char type[10];
};

struct RRQ
{
	uint16_t opcode;
	char file[100];
	char type[10];
};
struct DATA
{
	uint16_t opcode;
	uint16_t block;
	char data[513];
	
};
struct ACK
{
	uint16_t opcode;
	uint16_t block;
};

struct ERROR
{
	uint16_t opcode;
	char msg[100];
};



union PACKET
{
	struct WRQ wrq;
	struct RRQ rrq;
	struct DATA data;
	struct ACK ack;
	struct ERROR err;
};
#define BUF_SIZE 512
#define OP_RRQ 1
#define OP_WRQ 2
#define OP_DATA 3
#define OP_ACK 4
#define OP_ERROR 5
