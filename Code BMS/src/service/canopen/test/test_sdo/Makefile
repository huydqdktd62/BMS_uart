PROJ_ROOT=../..
TEST=test_sdo

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

SRCS= CAN_Msg.c CO_Object.c CO_PDO.c CO_RPDO.c CO_TPDO.c  CO_OD.c CO_SDO.c CO.c
SRCS:= $(addprefix $(PROJ_ROOT)/,$(SRCS))

OBJS = $(SRCS:.c=.o)

.PHONY:$(LIB).a

all: $(PROJ_ROOT)/$(LIB).a $(TEST).o
	$(CC) $(TEST).o -o $(TEST) $(LDFLAGS) 

%.o : %.c
	@echo [Building $^ ...]
	$(CC) $(CFLAGS) -c -o $@ $^
	@echo [Done !]

$(PROJ_ROOT)/$(LIB).a: $(OBJS)
	@echo [Linking $^ to $@ ...]
	$(AR) -rcsv $@  $^

clean:
	rm -f $(OBJS) $(LIB).a
