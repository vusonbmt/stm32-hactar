// Copyright 2011 Christoph Reiter
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//

#ifndef HACTAR_STARTUP_H__
#define HACTAR_STARTUP_H__

#include <stdint.h>

uint32_t hactarGetSystemClock(void);
uint32_t hactarGetSystickClock(void);
void hactarSystemInit(void);

#endif
