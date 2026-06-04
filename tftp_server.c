#include"headers.h"
union PACKET packet;

void handle_request(int , struct sockaddr_in , socklen_t , union PACKET);
void read_req(struct RRQ ,int  , struct sockaddr_in ,socklen_t );
void write_req(struct WRQ ,int , struct sockaddr_in ,socklen_t );

int main(int argc , char **argv)
{
	int n,s,sockfd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_len = sizeof(client_addr);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	perror("socket");
	if (sockfd < 0)
	{
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	server_addr.sin_port = htons(atoi(argv[1]));

	s=bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
	perror("bind");
	if(s<0)
	{
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	printf("TFTP Server started on port %d...\n", atoi(argv[1]));


	while (1)
	{
		n = recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, &addr_len);

		printf("got message from client:\n");
		printf("Using port: %d\n",ntohs(client_addr.sin_port));
		printf("With ip: %s\n",inet_ntoa(client_addr.sin_addr));

		if (n > 0)
		{
			handle_request(sockfd, client_addr, addr_len, packet);
		}
	}

	close(sockfd);
	return 0;
}

void handle_request(int sockfd, struct sockaddr_in client_addr, socklen_t addr_len, union PACKET buffer) 
{
	short int opc=*(short int*)&buffer;
	if (opc == 1)
	{
		printf("read request received.\n");
		read_req(buffer.rrq,sockfd,client_addr,addr_len);
	}
       	else if (opc == 2)
       	{
		printf("Write request received.\n");
		write_req(buffer.wrq,sockfd,client_addr,addr_len);
		
	}
       	else
       	{
		printf("Unknown request.\n");
	}
}
void read_req(struct RRQ buffer,int sockfd , struct sockaddr_in client_addr,socklen_t addr_len )
{
	struct ACK  ack;
	union  PACKET packet;
	struct DATA Data;
	FILE *file = fopen(buffer.file, "r");
	char data[512];
	short block_num = 1;
	int bytes_read ;

	if (!file)
	{
		perror("File open failed");
		return;
	}

	Data.opcode=3;
	while (!feof(file))
	{
		bzero(Data.data,513);
		bytes_read = fread(Data.data, 1, BUF_SIZE , file);
		Data.block=block_num;
again:
		sendto(sockfd, &Data,512, 0, (struct sockaddr *)&client_addr, addr_len);

		recvfrom(sockfd, &packet, sizeof buffer, 0, (struct sockaddr *)&client_addr, &addr_len);
		if(packet.ack.opcode==4)
			block_num++;
		else  if(packet.err.opcode==5)
		{
			if(strcmp(packet.err.msg,"not received")==0)
				goto again;
			else
			{
				puts("client terminated abnormally \n");
				return;
			}
		}
	}
	bzero(Data.data,513);
	sendto(sockfd, &Data,512, 0, (struct sockaddr *)&client_addr, addr_len);


	fclose(file);
}


void write_req(struct WRQ buffer,int sockfd , struct sockaddr_in client_addr,socklen_t addr_len )
{
        union PACKET packet;
        FILE *fp;
        short int n,opc;
	
	bzero((char *)&packet,sizeof(packet));

        packet.ack.opcode=4;
	packet.ack.block=0;
        //socklen_t addr_len = sizeof(client_addr);

        sendto(sockfd, &packet, sizeof(packet), 0, (const struct sockaddr *)&client_addr, sizeof(client_addr));

        fp = fopen(buffer.file, "w");
        if (!fp)
        {
                perror("File creation failed");
                return;
        }

        short int expected_block = 1,f=0;
        while (1)
        {
		if(f==1)
			break;
                bzero((char *)&packet,sizeof(packet));
                n = recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, &addr_len);
                if (n < 0)
                        break;

                opc=*(short int *)&packet;

                if (opc==3)
                {
                        if(strlen(packet.data.data)==0) 
                                break;
                        fwrite(packet.data.data,1,strlen(packet.data.data),fp);
                        
                        if(strlen(packet.data.data)<512) 
                                f=1;
			bzero((char *)&packet,sizeof(packet));

                        packet.ack.opcode=4;
                        packet.ack.block=expected_block;
                        sendto(sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&client_addr,addr_len);

                        expected_block++;
                }
                else if(opc==5)
                {
                        puts(packet.err.msg);
                        break;
                }

        }

        fclose(fp);


}


