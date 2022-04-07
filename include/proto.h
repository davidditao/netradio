// 协议头文件
#ifndef PROTO_H_ 
#define PROTO_H_

#include "site_type.h"									// 类型文件

#define DEFAULT_MGROUP		"224.2.2.2"					// 多播地址
#define DEFAULT_RCVPORT		"1989"						// 默认接收端口号

#define CHNNR				100							// 总的频道个数

#define LISTCHNID			0							// 节目单的频道号
#define MINCHNID			1							// 最小的频道号
#define MAXCHNID			(MINCHNID+CHNNR-1)			// 最大的频道号

#define MSG_CHANNEL_MAX		(65536-20-8)				// 一个频道包的大小:65535 - ip 报头 - udp报头
#define MAX_DATA			(MSG_CHANNEL_MAX-sizeof(chnid_t)	// 一个频道包中数据的大小

#define MSG_LIST_MAX		(65536-20-8)				// 一个节目单包的大小
#define MAX_ENTRY			(MSG_LIST_MAX-sizeof(chnid_t)		// 一个节目单包中数据的大小

struct msg_channel_st{									// 频道包	
	chnid_t chnid;										// 频道号
	uint8_t data[1];									// 包中的数据
}__attribute__((packed));								// 不需要对齐


struct msg_listentry_st{								// 节目单中的内容
	chnid_t chnid;										// 频道号
	uint16_t len;										// 每个频道内容的大小，用来区分一个节目单中不同频道的内容，不然所有内容连在一起，区分不开。
	uint8_t desc[1];									// 节目的描述信息
}__attribute__((packed));

struct msg_list_st{										// 节目单包
	chnid_t chnid;										// 频道号，必须为 LISTCHNID
	struct msg_listentry_st entry[1];					// 节目单中的内容
}__attribute__((packed));


#endif
