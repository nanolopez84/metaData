#pragma once

#include "resource.h"

#include "Console.h"
#include "State.h"

#define KEY_MAP_FILE_NAME	"keyMapping.cfg"
#define PROGRAM_NAME		"metaData"

extern Console g_console;   // Console text
extern Context g_state;		// Program state

void Exit(int nExitCode);
void InvalidateScreen();