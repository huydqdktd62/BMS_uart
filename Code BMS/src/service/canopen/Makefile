PROJ_ROOT=.
CC=arm-none-eabi-gcc
LIB=libCANopen

CPU=-mcpu=cortex-m4
OPTIMIZE=-Og
#FPU=-mfloat-abi=hard -mfpu=fpv4-sp-d16
#FPU=-mfloat-abi=softfp -mfpu=fpv4-sp-d16
FPU:=-mfloat-abi=hard -fsingle-precision-constant -mfpu=fpv4-sp-d16

# Option arguments for C compiler.
CFLAGS+=-mthumb ${CPU} ${FPU}
CFLAGS+= $(OPTIMIZE)
CFLAGS+=-fmessage-length=0
CFLAGS+=-fsigned-char
CFLAGS+=-Wall -Winline -ggdb -lm -MMD -MP -Wno-unused-function -Wextra -Wstrict-prototypes
CFLAGS+=-std=gnu11
CFLAGS+=--specs=nosys.specs
CFLAGS+=-ffunction-sections -fdata-sections
CFLAGS += -I.
CFLAGS += -finline-limit=500

SRCS=\
CO_CAN_Msg.c \
CO_NMT.c\
CO_Object.c \
CO_OD.c \
CO_PDO.c \
CO_RPDO.c \
CO_SDO.c\
CO_SDOclient.c\
CO_SDOserver.c\
CO_SYNC.c\
CO_TPDO.c \
CO.c\
CO_EMCY.c\
CO_FLASH.c\
CO_OD_sign.c\
CO_OD_storage.c\
CO_CRC.c\



SRCS:= $(addprefix $(PROJ_ROOT)/,$(SRCS))

OBJS = $(SRCS:.c=.o)

.PHONY:$(LIB).a

all: $(LIB).a

%.o : %.c
	@echo [Building $^ ...]
	$(CC) $(CFLAGS) -c -o $@ $^
	@echo [Done !]

$(LIB).a: $(OBJS)
	@echo [Linking $^ ...]
	$(AR) -rcsv $@  $^

clean:
	rm -f $(OBJS) $(LIB).a
	rm -f *.a
	rm -f *.o
	rm -f *.d
