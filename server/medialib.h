// 获取频道信息
#ifndef MEDIALIB_H__
#define MEDIALIB_H__



struct mlib_listentry_st{
	chnid_t chnid;
	char *desc;
};


int mlib_getchnlist(struct mlib_listentry_st **, int *);		// 获得频道信息

int mlib_frechnlist(struct mlib_listentry_st *);				// 释放空间

int mlib_readchn(chnid, void *, size_t );						// 读取频道


#endif
