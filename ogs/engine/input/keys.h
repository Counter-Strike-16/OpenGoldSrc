#pragma once

#include "public/FileSystem.h"

typedef enum
{
	key_game = 0,
	//key_console was here
	key_message = 1,
	key_menu = 2,
} keydest_t;

void Key_Init();

int Key_CountBindings();
void Key_WriteBindings(FileHandle_t f);