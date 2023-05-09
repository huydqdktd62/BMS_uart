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

SRCS= #CO_CAN_Msg.c #CO_Object.c CO_OD.c CO_PDO.c CO_RPDO.c CO_TPDO.c CO_SDO.c CO.c
SRCS:= $(addprefix $(PROJ_ROOT)/,$(SRCS))

OBJS = $(SRCS:.c=.o)

.PHONY:$(LIB).a

all: $(LIB).a
	$(CC) $(CFLAGS) -L. -lCANopen test_can_msg.c -o test_can_msg

%.o : %.c
	@echo [Building $^ ...]
	$(CC) $(CFLAGS) -c -o $@ $^
	@echo [Done !]

$(LIB).a: $(OBJS)
	@echo [Linking $^ ...]
	$(AR) -rcsv $@  $^

clean:
	rm -f $(OBJS) $(LIB).a
