#include"headers.h"

void Downloading(int , struct sockaddr_in,char * );
void Uploading(int , struct sockaddr_in,char * );

int main(int argc , char **argv)
{
	int sockfd;
	int ch;

	struct sockaddr_in server_addr;
	char file[100];

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	perror("socket");
	if (sockfd < 0)
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = inet_addr(argv[2]);

	while(1)
	{
		printf("1.RRQ\t2.WRQ\n");
		__fpurge(stdin);
		scanf("%d",&ch);

		if(ch==1)
		{
			puts("enter the file name to download...\n");
			__fpurge(stdin);
			fgets(file,100,stdin);
			file[strlen(file)-1]=0;

			Downloading(sockfd, server_addr,file);
		}
		else if(ch==2)
		{
			puts("enter the file name to upload...\n");
			__fpurge(stdin);
			fgets(file,100,stdin);
			file[strlen(file)-1]=0;

			Uploading(sockfd, server_addr,file);

		}
		else
			puts("wrong choice");
	}

	close(sockfd);
	return 0;
}

void Downloading(int sockfd, struct sockaddr_in server_addr,char *file)

{
	union PACKET packet,buffer;
	FILE *fp;
	short int n,opc;

	packet.rrq.opcode=1;
	strcpy(packet.rrq.file,file);
	printf("%d\n",packet.rrq.opcode);
	puts(packet.rrq.file);	

	printf("Using port: %d\n",ntohs(server_addr.sin_port));
	printf("With ip: %s\n",inet_ntoa(server_addr.sin_addr));


	sendto(sockfd, &packet, sizeof(packet), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
	puts("packet sent");

	fp = fopen(packet.rrq.file, "w");
	puts("file opend");
	if (!fp)
	{
		perror("File creation failed");
		return;
	}

	socklen_t addr_len = sizeof(server_addr);
	short expected_block = 1;
	while (1)
	{
		bzero(buffer.data.data,sizeof(buffer.data.data));
		n = recvfrom(sockfd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, &addr_len);
		if (n < 0)
			break;

		opc=*(short int *)&buffer;

		if (opc==3)
		{
			if(strlen(buffer.data.data)==0)
				break;
			fwrite(buffer.data.data,1,strlen(buffer.data.data),fp);
			bzero((char *)&buffer,sizeof(buffer));
			buffer.ack.opcode=4;
			buffer.ack.block=expected_block;
			sendto(sockfd,&buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, addr_len);

			expected_block++;
		}
		else if(opc==5)
		{
			puts(buffer.err.msg);
			break;
		}

	}

	fclose(fp);


}
void Uploading(int sockfd , struct sockaddr_in server_addr, char *file )
{
	union  PACKET packet;
	socklen_t addr_len=sizeof(server_addr);
	FILE *fp = fopen(file, "r");
	char data[513];
	short block_num = 1;
	int bytes_read , size;
	short int opc;
	if (!fp)
	{
		perror("File open failed");
		return;
	}

	packet.wrq.opcode=2;
	strcpy(packet.wrq.file,file);

	sendto(sockfd, &packet,sizeof(packet), 0, (struct sockaddr *)&server_addr, addr_len);
	perror("send");
	recvfrom(sockfd, &packet, sizeof packet, 0, (struct sockaddr *)&server_addr, &addr_len);
	perror("recv");
	opc=*(short int *)&packet;
	if(opc!=4)
	{
		;
	}
	else
	{
		fseek(fp,0,2);
		size=ftell(fp);
		fseek(fp,0,0);

		while (1)
		{
			bzero((char *)&packet,sizeof(packet));
			
			if(size<512)
			{
				fread(packet.data.data,1,size,fp);
				size=0;
			}
			else
			{
				fread(packet.data.data,1,512,fp);
				size=size-512;
			}

			packet.data.opcode=3;
			packet.data.block=block_num;

			sendto(sockfd, &packet,sizeof(packet), 0, (struct sockaddr *)&server_addr, addr_len);

			recvfrom(sockfd, &packet, sizeof packet, 0, (struct sockaddr *)&server_addr, &addr_len);

			if(packet.ack.opcode==4)
				block_num++;
			else  if(packet.err.opcode==5)
			{
				puts("server not able to give ACK....\n");
				return;
				
			}
			if(size==0)
			{
				puts("Uploadded sucessfully....");
				break;
			}

		}
	}
	

	fclose(fp);
}


