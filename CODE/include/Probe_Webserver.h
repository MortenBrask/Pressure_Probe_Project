#ifndef PROBE_WEBSERVER_H_
#define PROBE_WEBSERVER_H_

#include "main.h"

extern void probe_server_init(void);
extern void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

#endif