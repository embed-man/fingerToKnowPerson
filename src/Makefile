

TARGET := lb_zhiwen
CSRCS := $(wildcard *.c)   #找通配规则为 *.c 的所有文件名
CSRCS += $(wildcard ./sub/*.c)
#OBJS := main.o a.o b.o c.o d.o 20150813.o
OBJS := $(patsubst  %.c, %.o, $(CSRCS))

CC := arm-linux-gcc

CFLAGS += -I../lib/sqlite/include -I ../include
LIBS += -L../lib/sqlite/lib -lsqlite3 -lm

$(TARGET): $(OBJS)
	$(CC)  -o $@  $+  $(CFLAGS) $(LIBS)
	#cp $(TARGET) ./

%.o:%.c
	$(CC) -c $< -o $@ $(CFLAGS)


	
clean:
	rm  $(TARGET) $(OBJS)
