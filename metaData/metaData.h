#pragma once

#include "resource.h"

#include "Console.h"
#include "State.h"

#define PROGRAM_NAME "metaData"

extern Console g_console;   // Console text
extern Context g_state;		// Program state

void Exit(int nExitCode);
void InvalidateScreen();