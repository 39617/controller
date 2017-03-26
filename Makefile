CONTIKI_PROJECT=gateway
all: MKDIR_P $(CONTIKI_PROJECT)

MKDIR_P:
	mkdir -p obj_$(TARGET)
	mkdir -p obj_$(TARGET)/tools
	mkdir -p obj_$(TARGET)/$(CONTIKI)
	mkdir -p obj_$(TARGET)/$(CONTIKI)/dev
	mkdir -p obj_$(TARGET)/$(CONTIKI)/dev/enc28j60



CONTIKI=contiki_multiple_interface
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
# TODO: retirar
CFLAGS += -g
PROJECT_SOURCEFILES += tools/sicslow_ethernet.c enc28j60_spi_arch.c $(CONTIKI)/dev/enc28j60/enc28j60.c ethernet-drv.c ethernet-dev.c


CONTIKI_WITH_IPV6 = 1
CONTIKI_WITH_RPL = 0


# 0 - Sem optimização (-O0)
# 1 - Optimização (-Os)
# x - Optimização (-O2)
SMALL=0






# automatically build RESTful resources
REST_RESOURCES_DIR = ./resources
ifndef TARGET
REST_RESOURCES_FILES = $(notdir $(shell find $(REST_RESOURCES_DIR) -name '*.c'))
else
ifeq ($(TARGET), native)
REST_RESOURCES_FILES = $(notdir $(shell find $(REST_RESOURCES_DIR) -name '*.c'))
else
REST_RESOURCES_FILES = $(notdir $(shell find $(REST_RESOURCES_DIR) -name '*.c' ! -name 'res-plugtest*'))
endif
endif

PROJECTDIRS += $(REST_RESOURCES_DIR)
PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)

# REST Engine shall use Erbium CoAP implementation
APPS += er-coap er-http
APPS += rest-engine

include $(CONTIKI)/Makefile.include


