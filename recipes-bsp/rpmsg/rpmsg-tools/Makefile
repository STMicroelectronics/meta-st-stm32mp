EPT := rpmsg_create_ept

SRCS := rpmsg_create_ept.c
OBJS := $(SRCS:.c=.o)

all: $(EPT)

$(EPT): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EPT) $(OBJS)
