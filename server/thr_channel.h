// 频道线程
#ifndef THR_CHANNEL_H__
#define THR_CHANNEL_H__

#include "medialib.h"

int thr_channel_create(struct mlib_listentry_st *);

int chr_channel_destroy(struct mlib_listentry_st *);	// 销毁单个频道

int thr_channel_destroyall(void);


#endif
