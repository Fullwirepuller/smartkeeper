#ifndef _UART_H_
#define _UART_H_ 

extern int uart_open(char * serial_port);
extern void uart_close(int fd);
extern int uart_init(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity);
extern int uart_recv(int fd, char *rcv_buf,int data_len, int timeout);
extern int uart_send(int fd, char *send_buf,int data_len);
#endif 
