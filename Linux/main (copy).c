#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "uart.h"
#include "speak_text.h"
void *pthread_run1(void *arg)
{
	int cfd = *(int *)arg;
	char buf[64] = {0};
	int ret = 0;
	while(1)
	{
		bzero(buf,sizeof(buf));
		fgets(buf,sizeof(buf)-1,stdin);
		buf[strlen(buf)-1] = '\0';
		ret = send(cfd,buf,strlen(buf),0);
		if(-1 == ret)
		{
		perror("send");
		exit(EXIT_FAILURE);			
		}
	}
}
int main(int argc, const char *argv[])
{
	int sockfd = socket(PF_INET,SOCK_STREAM,0);
	if(-1 == sockfd)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in saddr = {0};
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(argv[1]));
	saddr.sin_addr.s_addr = inet_addr("0.0.0.0");


	int ret = bind(sockfd,(struct sockaddr *)&saddr,sizeof(saddr));
	if(-1 == ret)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	ret = listen(sockfd,100);
	if(-1 == ret)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	char buf[64] = {0};
	printf("wait connect...\n");
	struct sockaddr_in caddr = {0};
	int clen = sizeof(caddr);
	int cfd = accept(sockfd,(struct sockaddr *)&caddr,&clen);
	if(-1 == cfd)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	printf("caddr port: %d ip: %s\n",
		ntohs(caddr.sin_port),inet_ntoa(caddr.sin_addr));
	pthread_t tid=0;
	ret = pthread_create(&tid,NULL,pthread_run1,&cfd);
	if(0 != ret)
	{
		printf("pthreat_create:%s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	ret = pthread_detach(tid);
	if(0 != ret)
	{
		printf("pthreat_detach:%s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	while(1)
	{	//为语音播报做准备
		int fd2 = 0;
		char buf2[64] = {0};
		fd2 = uart_open("/dev/ttyUSB0");
		if(-1 == fd2)
		{
			printf("open");
			exit(EXIT_FAILURE);
		}
		uart_init(fd2,9600,0,8,1,'N');

		bzero(buf,sizeof(buf));
		ret = recv(cfd,buf,sizeof(buf)-1,0);
		if(-1 == ret)
		{
			printf("recv");
			exit(EXIT_FAILURE);
		}
		else if(0 == ret)
		{
			printf("client quit...\n");	
			goto flag;
		}
			puts(buf);
		if(strcmp(buf,"ledon")==0)
		{
			uart_send(fd2,buf_speak_ledon,sizeof(buf_speak_ledon));
		}
		else if(strcmp(buf,"ledoff")==0)
		{
			uart_send(fd2,buf_speak_ledoff,sizeof(buf_speak_ledoff));
		}
		else if(strcmp(buf,"fanoff")==0)
		{
			uart_send(fd2,buf_speak_fanoff,sizeof(buf_speak_fanoff));
		}
		else if(strcmp(buf,"fanon")==0)
		{
			uart_send(fd2,buf_speak_fanon,sizeof(buf_speak_fanon));
		}
		else if(strcmp(buf,"videooff")==0)
		{
			uart_send(fd2,buf_speak_videooff,sizeof(buf_speak_videooff));
		}
		else if(strcmp(buf,"videoon")==0)
		{
			uart_send(fd2,buf_speak_videoon,sizeof(buf_speak_videoon));
		}
		else if(strcmp(buf,"recordingoff")==0)
		{
			uart_send(fd2,buf_speak_recordingoff,sizeof(buf_speak_recordingoff));
		}
		else if(strcmp(buf,"recordingon")==0)
		{
			uart_send(fd2,buf_speak_recordingon,sizeof(buf_speak_recordingon));
		}
		uart_recv(fd2,buf,sizeof(buf2),1);
		//printf("%s\n",buf2); 

		uart_close(fd2);
	}
	flag:
	close(sockfd);
	close(cfd);
	return 0;
}

