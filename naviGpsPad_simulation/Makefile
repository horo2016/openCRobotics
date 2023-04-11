##### Make sure all is the first target.
all:

CXX ?= g++
CC  ?= gcc
#操作开关
CXX_OPTS = #-DOPEN_CPUDETECT_PTHREAD #-DWITH_OPENCV #-DMPU_DEBUGOFF  OPEN_STM32_PTHREAD
#是否用远程mqtt server 
#CXX_OPTS += -DMQTT_REMOTE_SERVER 
#是否将终端作为mqtt server 
CXX_OPTS +=-DMQTT_TERMINAL_SERVER
RTIMULIBPATH  = ./RTIMULib

CFLAGS  += -g -pthread -Wall 
CFLAGS  += -rdynamic -funwind-tables

CFLAGS = -O2 -I./include -I./Mqtt
INCPATH   += -I. -I$(RTIMULIBPATH) 
CFLAGS  += -I./inc    
CFLAGS  += -I./Mqtt  
CFLAGS  += -I$(RTIMULIBPATH)

DIR_LIBMQTT = Mqtt

DIR_OBJ = obj

DIRS = 	$(DIR_LIBMQTT)
		
		
FILES = $(foreach dir, $(DIRS),$(wildcard $(dir)/*.c))	

SRCS = $(wildcard gps_srcs/*.cpp)  
HEDS = $(wildcard inc/*.h)  
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))  
DEPS = $(patsubst %.o, %.d, $(OBJS))

CFLAGS  += -I./gps_srcs
vpath %.c gps_srcs
CXXDIRS = 	$(RTIMULIBPATH)				\
		    $(RTIMULIBPATH)/IMUDrivers
CXXFILES =	$(foreach dir, $(CXXDIRS),$(wildcard $(dir)/*.cpp))
CXXFLAGS  += -I./inc  
CXXFLAGS  += -I$(RTIMULIBPATH)/IMUDrivers
CFLAGS += -D__unused="__attribute__((__unused__))"
CFLAGS +=  $(CXX_OPTS)

#LDFLAGS += -L./usr/lib/gpac
LDFLAGS += -ldl
#LDFLAGS += -L./usr/lib/
LDFLAGS +=  -L./Mqtt/lib -lmosquitto
ifdef  RASPBERRY
LDFLAGS +=  -ldl -lm  -lstdc++
LDFLAGS += -lrt -lpthread -pthread -lm -ldl 
endif
C_SRC=

C_SRC+=src/main.c
C_SRC+=src/osp_proc_data.c
C_SRC+=src/osp_syslog.c
C_SRC+=src/mqtt_main.c

C_SRC+= $(FILES)



#stm32 control
C_SRC+=src/socket_tcp.c
C_SRC+=src/stm32_control.c
C_SRC+=src/Uart_comm.c
C_SRC+=src/gps_hal.c 
C_SRC+=src/cJSON.c
C_SRC+= src/config_conf.c
C_SRC+= src/md5.c



CXX_SRC=
CXX_SRC +=src/kalman.cpp
#ifdef RASPBERRY
CXX_SRC +=  src/dwa.cpp
CXX_SRC += src/dwa_demo.cpp
#endif 
CXX_SRC +=src/PID_v1.cpp
CXX_SRC +=src/geocoords.cpp
CXX_SRC+=src/gps.cpp
CXX_SRC+=src/navi_manage.cpp
CXX_SRC+= src/cpu_sys.cpp
CXX_SRC+= $(CXXFILES)
CXX_SRC+=$(SRCS)
CXX_SRC +=src/method_parse.cpp

CXX_SRC +=  src/SimpleKalmanFilter.cpp
CXX_SRC +=  src/online_client.cpp
OBJ=
DEP=

# Files

CXXFLAGS += -std=c++11 $(CFLAGS)
#LDFLAGS+= -lcamera

OBJ_CAM_SRV = src/main.o
TARGETS    += gpscarbot
$(TARGETS): $(OBJ_CAM_SRV)
TARGET_OBJ += $(OBJ_CAM_SRV)

FILE_LIST := files.txt
COUNT := ./make/count.sh

OBJ=$(CXX_SRC:.cpp=.o) $(C_SRC:.c=.o)
DEP=$(OBJ:.o=.d) $(TARGET_OBJ:.o=.d)

CXXFLAGS += -std=c++11 -g 
CXXFLAGS += -lc -lm -pthread
#include ./common.mk
.PHONY: all clean distclean

all: $(TARGETS)

clean:
	rm -f $(TARGETS) $(FILE_LIST)
	find . -name "*.o" -delete
	find . -name "*.d" -delete

distclean:
	rm -f $(TARGETS) $(FILE_LIST)
	find . -name "*.o" -delete
	find . -name "*.d" -delete

-include $(DEP)

%.o: %.c 
	@[ -f $(COUNT) ] && $(COUNT) $(FILE_LIST) $^ || true
	@$(CC) -c $< -MM -MT $@ -MF $(@:.o=.d) $(CFLAGS) $(LIBQCAM_CFLAGS)
	$(CC) -c $< $(CFLAGS) -o $@ $(LIBQCAM_CFLAGS) $(INCPATH)


%.o: %.cpp 
	@$(CXX) -c $< -MM -MT $@ -MF $(@:.o=.d) $(CXXFLAGS)
	$(CXX) -c $< $(CXXFLAGS) -o $@   $(INCPATH)
	

$(TARGETS): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
	@[ -f $(COUNT) -a -n "$(FILES)" ] && $(COUNT) $(FILE_LIST) $(FILES) || true
	@[ -f $(COUNT) ] && $(COUNT) $(FILE_LIST) || true
