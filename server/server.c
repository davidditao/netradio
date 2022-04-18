#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errorno.h>

#include <proto.h>

#inlcude "server_conf.h"

// -M       指定多播组
// -P       指定接收端口
// -F       前台运行
// -D       指定媒体库位置
// -I       指定网络设备/网卡
// -H       显示帮助

struct server_conf_st server_conf = {.rcvport = DEFAULT_RCVPORT,\
                                    .mgroup = DEFAULT_MGROUP,\
                                    .media_dir = DEFAULT_MEDIADIR,\
                                    .runmode = RUN_DAEMON,\
                                    .ifname = DEFAULT_IF};  // 初始化默认值

void printfhelp(void){
    printf("-M       指定多播组\n");
    printf("-P       指定接收端口\n");
    printf("-F       前台运行\n");
    printf("-D       指定媒体库位置\n");
    printf("-I       指定网络设备/网卡\n");
    printf("-H       显示帮助\n");
}

static void daemon_exit(int s){
	closelog();					// 关闭系统日志

	exit(0);
}

static int daemonize(void){
	pid_t pid;
	int fd;

	pid = fork();
	if(pid < 0){
		// perror("fork()");
		syslog(LOG_ERR, "fork():%s", strerror(errno));
		return -1;
	}

	if(pid > 0){		// 父进程直接退出
		exit(0);
	}

	fd = open("/dev/null/", O_RDWR);
	if(fd < 0){
		// perror("open()");
		syslog(LOG_WARNING, "open():%s", strerror(errno));
		return -2;
	} else {
		// 脱离控制终端
		dup2(fd, 0);
		dup2(fd, 1);
		dup2(fd, 2);
		
		if(fd > 2){
			close(fd);
		}
	}
	setsid();

	chdir("/");			// 将脱离控制终端运行的进程指定到一个合适的位置上
	umask(0);
}

static void socket_init(void){
	int serversd;
	struct ip_mreqn mreq;

	serversd = socket(AF_INET, SOCK_DGRAM, 0);
	if(serversd < 0){
		syslog(LOG_ERR, "socket():%s", sterror(errno));
		exit(1);
	}
	
	inet_pton(AF_INET, server_conf.mgroup, &mreq.imr_multiaddr);
	inet_pton(AF_INET, "0.0.0.0", mreq.imr_address);
	mreq.imr_ifindex = if_nametoindex(server_conf.ifname);

	if(setsockopt(serversd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0){		// 创建多播组
		syslog(LOG_ERR, "setsockopt(IP_MULTICAST_IF):%s", sterror(errno));
		exit(1);
	}
	
	// bind();


}


int main(int argc, char *argv){
    int c;

	struct sigaction sa;
	sa.sa_handler = daemon_exit;
	sigemptyset(&sa.sa_mask);
	sigaddset(sa.sa_mask, SIGTERM);
	sigaddset(sa.sa_mask, SIGINT);
	sigaddset(sa.sa_mask, SIGQUIT);

	// 定义三个常用信号，用于结束守护进程
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	
	// 由于守护进程是脱离控制终端的，所以我们需要用日志文件来记录程序的错误
	openlog("netradio", LOG_PID|LOG_PERROR, LOG_DAEMON);	// 打开日志文件
	
	// 1.命令行分析
    while(1){
        c = getopt(argc, argv, "M:P:FD:I:H");
        if(c < 0){
            break;
        }
        switch(c){
            case 'M':
                server_conf.mgroup = optarg;
                break;
            case 'P':
                server_conf.rcvport = optarg;
                break;
            case 'F':
                server_conf.runmode = RUN_FORGROUND;
                break;
            case 'D':
                server_conf.media_dir = optarg;
                break;
            case 'I':
                server_conf.ifname = optarg;
                break;
            case 'H':
                printfhelp();
                exit(0);
                break;
            default:
                abort();
                break;
        }
    }

    // 2.守护进程的实现
	if(server_conf.runmode == RUN_DAEMON){
		if(daemonize() != 0){
			exit(1);
		}
	} else if(server_conf.runmode == RUN_FOREGROUND){
		/* do something */
	} else {
		// fprintf(stderr, "EINVAL\N");
		syslog(LOG_ERR, "EINVAL server_conf.runmode.");
		exit(1);
	}

    // 3.socket初始化
	socket_init();


    // 4.获取频道信息
	struct mlib_listentry_st *list;
	int list_size;
	int err;
	err = mlib_getchnlist(&list, &list_size);
	if(err){

	}


    // 5.创建节目单线程
	thr_list_creat(list, list_size);
	/* if error */


    // 6.创建频道线程
	int i;
	for(i = 0; i < list_size; i++){
		thr_channel_create(list+i);
		/* if error */
	}

	syslog(LOG_DEGUG, "%d channel threads created.", i);

    while(1){			// main函数不能结束, 用pause等待
        pause();
    }


    exit(0);
}
