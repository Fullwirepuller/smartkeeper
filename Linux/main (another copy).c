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

void *pthread_run(void *arg)
{
	int cfd = *(int *)arg;
	unsigned int width = 640,height = 480,size;
	unsigned int index,ismjpeg;
	int n = 0;
	char buf[16] = {0};
	int ret = 0;
	
	while(1)
	{
		bzero(buf,sizeof(buf));
		ret = recv(cfd,buf,sizeof(buf)-1,0);
		if(-1 == ret)
		{
			perror("recv");
			exit(EXIT_FAILURE);
		}
		ret = strcmp(buf,"takepic");
		if(0 == ret)
			break;
	}

#if 0
	int imagefd = open("001.jpg",O_RDWR|O_CREAT|O_TRUNC,0666);
	if(-1 == imagefd)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
#endif
	//打开摄像设备
	int fd = camera_init("/dev/video0",&width,&height,&size,&ismjpeg);
	if(-1 == fd)
	{
		printf("camera_init error...\n");
		exit(EXIT_FAILURE);
	}

	//开始采集数据
	ret = camera_start(fd);
	if(-1 == ret)
	{
		printf("camera_start error...\n");
	}

	char *jpeg_ptr = NULL;

for(n = 0;;)
{

	//获取图片数据
	ret = camera_dqbuf(fd,(void **)&jpeg_ptr,&size,&index);
	if(-1 == ret)
	{
		printf("camera_dqbuf error...\n");
		exit(EXIT_FAILURE);
	}


	//send image to client start
	bzero(buf,sizeof(buf));
	sprintf(buf,"%d",size);
	ret = send(cfd,buf,sizeof(buf),0);
	if(-1 == ret)
	{
		perror("send");
		exit(EXIT_FAILURE);
	}

	ret = send(cfd,jpeg_ptr,size,0);
	if(-1 == ret)
	{
		perror("send");
		exit(EXIT_FAILURE);
	}

	//send image to client end


	//告诉系统图片已取走
	ret = camera_eqbuf(fd,index);
	if(-1 == ret)
	{
		printf("camera_eqbuf error...\n");
		exit(EXIT_FAILURE);
	}
}
	//停止数据采集
	ret = camera_stop(fd);
	if(-1 == ret)
	{
		printf("camera_stop error...\n");
		exit(EXIT_FAILURE);
	}

	//关闭摄像头
	camera_exit(fd);
	close(cfd);
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


	pthread_t tid = 0;
	ret = pthread_create(&tid,NULL,pthread_run,&cfd);
	if(0 != ret)
	{
		printf("pthread_create error:%s\n",strerror(ret));
		exit(EXIT_FAILURE);
	}
	
	pthread_detach(tid);

	


	bzero(&caddr,sizeof(caddr));
	int cfd2 = accept(sockfd,(struct sockaddr *)&caddr,&clen);
	if(-1 == cfd2)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	while(1)
	{	//为语音播报做准备
		int fd2 = 0,fd3=0;
		char buf2[64] = {0};
		char buf3[64] = {0};
		fd2 = uart_open("/dev/ttyUSB2");
		fd3 = uart_open("/dev/ttyUSB3");
		if(-1 == fd2)
		{
			printf("open");
			exit(EXIT_FAILURE);
		}
		if(-1 == fd3)
		{
			printf("open");
			exit(EXIT_FAILURE);
		}
		uart_init(fd2,9600,0,8,1,'N');
		uart_init(fd3,115200,0,8,1,'N');
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
			uart_send(fd3,"1",sizeof(char));
		}
		else if(strcmp(buf,"ledoff")==0)
		{
			uart_send(fd2,buf_speak_ledoff,sizeof(buf_speak_ledoff));
			uart_send(fd3,"2",sizeof(char));
		}
		else if(strcmp(buf,"fanoff")==0)
		{
			uart_send(fd2,buf_speak_fanoff,sizeof(buf_speak_fanoff));
			uart_send(fd3,"3",sizeof(char));
		}
		else if(strcmp(buf,"fanon")==0)
		{
			uart_send(fd2,buf_speak_fanon,sizeof(buf_speak_fanon));
			uart_send(fd3,"4",sizeof(char));
		}
		else if(strcmp(buf,"videooff")==0)
		{
			uart_send(fd2,buf_speak_videooff,sizeof(buf_speak_videooff));
			uart_send(fd3,"5",sizeof(char));
		}
		else if(strcmp(buf,"videoon")==0)
		{
			uart_send(fd2,buf_speak_videoon,sizeof(buf_speak_videoon));
			uart_send(fd3,"6",sizeof(char));
		}
		else if(strcmp(buf,"weatheron")==0)
		{
			uart_send(fd2,buf_speak_weatheron,sizeof(buf_speak_weatheron));
		}
		else if(strcmp(buf,"recordingoff")==0)
		{
			uart_send(fd2,buf_speak_recordingoff,sizeof(buf_speak_recordingoff));
		}
		else if(strcmp(buf,"recordingon")==0)
		{
			uart_send(fd2,buf_speak_recordingon,sizeof(buf_speak_recordingon));
		}
		uart_recv(fd2,buf2,sizeof(buf2),1);
		uart_recv(fd3,buf3,sizeof(buf2),1);
		//printf("%s\n",buf2); 
		uart_close(fd2);
		uart_close(fd3);
	}
	flag:
	close(sockfd);
	close(cfd);
	return 0;
}

