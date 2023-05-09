PROJ_ROOT=../..

export CCPREFIX =
export CC      	= $(CCPREFIX)gcc
export CP       = $(CCPREFIX)objcopy
export LD      	= ${CCPREFIX}ld
export DUMP	= $(CCPREFIX)objdump
export AS       = $(CCPREFIX)gcc -x assembler-with-cpp
export AR      	= $(CCPREFIX)ar
export GDBTUI   = $(CCPREFIX)gdb --tui
export SIZE	= $(CCPREFIX)size
export HEX      = $(CP) -O ihex
export BIN      = $(CP) -O binary

LIB=libCANopen
CFLAGS  = -g -O0 -Wall
CFLAGS += -I$(PROJ_ROOT)
LDFLAGS = -L$(PROJ_ROOT) -lCANopen 

SRCS= CO_CAN_interface.c \
CO_CAN_Msg.c\
CO_NMT.c\
CO_Object.c\
CO_OD.c\
CO_PDO.c\
CO_SDO.c\
CO_SDOclient.c\
CO_SDOserver.c\
CO_SYNC.c\
CO_TPDO.c\
CO.c

SRCS+= app_co_init.c \
app_co_od.c

SRCS:= $(addprefix $(PROJ_ROOT)/,$(SRCS))

OBJS = $(SRCS:.c=.o)

.PHONY:$(LIB).a

all: $(PROJ_ROOT)/$(LIB).a test1.o
	$(CC) test1.o -o test1 $(LDFLAGS) 

%.o : %.c
	@echo [Building $^ ...]
	$(CC) $(CFLAGS) -c -o $@ $^
	@echo [Done !]

$(PROJ_ROOT)/$(LIB).a: $(OBJS)
	@echo [Linking $^ to $@ ...]
	$(AR) -rcsv $@  $^

clean:
	rm -f $(OBJS) $(LIB).a
