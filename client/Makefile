CFLAGS += -I ../include/ # 对于#include <file>, gcc 会到-I指定的目录中查找，如果找不到，在到系统的缺省的头文件目录查找

all:client

client:client.o
	gcc $^ -o $@ $(CFLAGS)

clean:
	rm -rf *.o client

