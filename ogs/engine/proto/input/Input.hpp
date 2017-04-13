#pragma once

constexpr auto WM_MOUSEWHEEL = 0x020A;

extern qboolean	mouseinitialized;

void PollDInputDevices();

void IN_UpdateClipCursor();

void IN_Activate(qboolean active);
void IN_MouseEvent(int mstate);