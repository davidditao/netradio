#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <net/if.h>

// #include "../include/proto.h"
#include <proto.h>
#include "client.h"

struct client_conf_st client_conf = {.rcvport = DEFAULT_RCVPORT,\
					.mgroup = DEFAULT_MGROUP,\
					.player_cmd = DEFAULT_PLAYERCMD}; // 默认的参数

static void printhelp(void){
	printf("命令行的参数：\n");
	printf("-M --mgroup		指定多播组\n");
	printf("-P --port		指定接收端口\n");
	printf("-p --player		指定播放器\n");
	printf("-H --help		显示帮助\n");
}

// APUE 第3版 14.7 中定义了这个函数：
// 因为一次write操作可能因为捕捉到一个信号而中途返回，这种情况应该继续写余下的数据。
static ssize_t writen(int fd, const char *buf, size_t len){
	int ret = 0;
	int pos = 0;
	while(len > 0){
		ret = write(fd, buf+pos, len);
		if(ret < 0){
			if(errno == EINTR){
				continue;
			}
			perror("write()");
			return -1;	
		}
		len -= ret;
		pos += ret;
	}
	return 0;
}


int main(int argc, char **argv){
	int index = 0;		// getopt_long 参数：longindex
	int c;				// getopt_long 返回值
	int len;			// recvfrom 返回值
	int ret;			// scanf 返回值
	int sd;				// 套接字描述符
	int pd[2];			// 管道
	pid_t pid;			// 进程id
	int chosenid;		// 选择的频道id

	// getopt_long 参数：长选项的结构体
	struct option argarr[] = {{"port", 1, NULL, 'P'},\
							{"mgroup", 1, NULL, 'M'},\
							{"player", 1, NULL, 'p'},\
							{"help", 1, NULL, 'H'},\
							{NULL, 0, NULL, 0}};		
	struct ip_mreqn mreq;		// 
	struct sockaddr_in clientaddr;	// 客户端套接字地址结构体
	struct sockaddr_in serveraddr;	// 服务端套接字地址结构体（节目单）
	struct sockaddr_in remoteaddr;	// 服务端套接字地址结构体（频道包）
	socklen_t serveraddr_len;		// 服务端套接字地址长度(节目单）
	socklen_t remoteaddr_len;		// 服务端套接字地址长度(频道包）


/*	初始化
*	级别: 默认值，配置文件，环境变量，命令行参数
**/
	while(1){
		// 库函数getopt_long：分析命令行，可以通过命令行修改多播组地址和端口号
		// getopt 只能处理短选项，getopt_long 都可以
		// 返回短选项字符 = getopt_long(argc, argv, 短选项字符串，长选项结构体，返回选项在argarr中的下标); 
		c = getopt_long(argc, argv, "M:P:p:H", argarr, &index);	
		if(c < 0) break;
		switch(c){
			case 'P':
				// optarg 是getopt中的全局变量，表示当前选项对应的参数值
				client_conf.rcvport = optarg;
				break;
			case 'M':
				client_conf.mgroup = optarg;
				break;
			case 'p':
				client_conf.player_cmd = optarg;
				break;
			case 'H':
				printhelp();
				exit(0);
				break;
			default:
				abort();
				break;
		}
	}
	
	// UDP 用的是 SOCK_DGRAM， protocol = 0 内核会自动选择协议
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0){
		perror("socket()");
		exit(1);
	}
	
	// 设置多播
	inet_pton(AF_INET, client_conf.mgroup, &mreq.imr_multiaddr);
	/* if error */
	inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
	mreq.imr_ifindex = if_nametoindex("eth0");
	// 加入多播组
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0){
		perror("setsockopt()");
		exit(1);
	}	
	
	int val = 1;
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof(val)) < 0){			// 这个设置可要可不要
		perror("setsockopt()");
		exit(1);
	}

	// 绑定套接字和地址
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(atoi(client_conf.rcvport));
	inet_pton(AF_INET, "0.0.0.0", &clientaddr.sin_addr.s_addr);
	if(bind(sd, (void *)&clientaddr, sizeof(clientaddr)) < 0){
		perror("bind()");
		exit(1);
	}
	
	// 设置管道
	if(pipe(pd) < 0){
		perror("pipe()");
		exit(1);
	}
	
	// 创建子进程
	pid = fork();
	if(pid < 0){
		perror("fork()");
		exit(1);
	}
	
	if(pid == 0){		// 子进程，调用解码器
		close(sd);		// 子进程不需要用到socket，所以把它关闭
		close(pd[1]);	// 子进程只需要从管道中读数据，不需要输i出，所以将管道输出端关闭
		dup2(pd[0],0);	// 标准输入重定向为pd[0]	
		if(pd[0] > 0){
			close(pd[0]);	// 如果pd[0]本身不是标准输入，那就关闭pd[0]
		}
		
		execl("/bin/sh/", "sh", "-c", client_conf.player_cmd, NULL);	// 利用shell来执行解码器
		perror("execl()");
		exit(1);
	} else {			// 父进程，从网络上收包，发送给子进程
		// 收节目单包
		struct msg_list_st *msg_list;		// 因为包的大小不确定，所以定义一个指针
		msg_list = malloc(MSG_LIST_MAX);	// 指针指向的空间的大小为最大包的大小
		if(msg_list == NULL){
			perror("malloc()");
			exit(1);
		}
		
		while(1){
			// UDP接收必须用recvfrom
			len = recvfrom(sd, msg_list, MSG_LIST_MAX, 0, (void *)&serveraddr, &serveraddr_len); 
			if(len < sizeof(struct msg_list_st)){	// 如果收到的包太小，就重新收包
				fprintf(stderr, "message is too small.\n");
				continue;			
			}
			if(msg_list->chnid != LISTCHNID){		// 如果收到的包不是节目单，也重新收包
				fprintf(stderr, "chnid is not match");
				continue;
			}
			break;
		}

		// 打印节目单并选择频道
		struct msg_listentry_st *pos;	// 指向节目单中的每个频道的内容结构体
		for(pos = msg_list->entry; (char *)pos < (((char*)msg_list) + len);pos = (void*)(((char *)pos)+ntohs(pos->len))){
			printf("channel %d : %s\n", pos->chnid, pos->desc);	
		}
		free(msg_list);	// 释放内存


		// 选择频道
		while(ret < 1){
			ret = scanf("%d", &chosenid); // scanf的返回值是输入个数
			if(ret != 1){
				exit(1);	
			}
		}

		// 收频道包，发送给子进程
		struct msg_channel_st *msg_channel; // 频道包
		msg_channel = malloc(MSG_CHANNEL_MAX);
		if(msg_channel == NULL){
			perror("malloc()");
			exit(1);
		}
		
		while(1){
			len = recvfrom(sd, msg_channel, MSG_CHANNEL_MAX, 0, (void *)&remoteaddr, &remoteaddr_len);
			if(remoteaddr.sin_addr.s_addr != serveraddr.sin_addr.s_addr || remoteaddr.sin_port != serveraddr.sin_port){		// 如果频道包地址与节目单地址不同，则忽略这个包
				fprintf(stderr, "Ignore: adreess not match");
				continue;
			}

			if(len < sizeof(struct msg_channel_st)){		// 如果这个包太小，也忽略这个包
				fprintf(stderr, "Ignore: message too small.\n");
			}
			if(msg_channel->chnid == chosenid){
				fprintf(stdout, "accepted msg: %d recieved.\n", msg_channel->chnid);
				if(writen(pd[1], msg_channel->data, len-sizeof(chnid_t)) < 0){	// 坚持写数据，持续写够一定字节数
					exit(1);
				}	
			}
		}
		
		// 下面的内容执行不到，可以考虑用信号，当接收到一个信号时，处理下面的内容，然后退出。
		free(msg_channel);
		close(sd);
	} 
	exit(0);
}
