CONTIKI_PROJECT=controller
all: $(CONTIKI_PROJECT)

CONTIKI = contiki_multiple_interface
PROJECT_DEVICE_DIR = ./dev
# REST Engine shall use Erbium CoAP implementation
APPS += er-coap er-http
APPS += rest-engine
APPS += netctrl

# Http Request
APPS += webbrowser

# ----------------  Flags  ----------------
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
# Debug - TODO: retirar
CFLAGS += -g
# -----------------------------------------

# ----------------  Sources  ----------------
#
PROJECT_SOURCEFILES += coap_client.c

# ethernet
MODULES += core/net/eth

# tools
PROJECTDIRS += ./tools
PROJECT_SOURCEFILES += sicslow_ethernet.c

# enc28j60
PROJECTDIRS += PLATFORM_ROOT_DIR/common
PROJECT_SOURCEFILES += enc28j60_spi_arch.c
MODULES += dev/enc28j60_revb

# parser
PARSERS_DIR = ./parsers
PARSERS_FILES = $(notdir $(shell find $(PARSERS_DIR) -name '*.c'))

# Erro codes
PROJECT_SOURCEFILES += error_codes.c

# RESTful resources
REST_RESOURCES_DIR = ./resources
PROJECTDIRS += $(REST_RESOURCES_DIR)

# Http Request
PROJECT_SOURCEFILES += http_request.c
PROJECT_SOURCEFILES += http_request_test.c


ifndef TARGET
REST_RESOURCES_FILES = $(notdir $(shell find $(REST_RESOURCES_DIR) -name '*.c'))
else
ifeq ($(TARGET), native)
REST_RESOURCES_FILES = $(notdir $(shell find $(REST_RESOURCES_DIR) -name '*.c'))
else
REST_RESOURCES_FILES = $(notdir $(shell find $(REST_RESOURCES_DIR) -name '*.c' ! -name 'res-plugtest*'))
endif
endif
PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)


PROJECTDIRS += $(PARSERS_DIR)
PROJECT_SOURCEFILES += $(PARSERS_FILES)
# -----------------------------------------

CONTIKI_WITH_IPV6 = 1
CONTIKI_WITH_RPL = 0

# 0 - No Optimisation: -O0
# 1 - Optimisation:    -Os
# x - Optimisation:    -O2
SMALL=0


include $(CONTIKI)/Makefile.include
