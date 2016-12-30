#pragma once

qboolean Voice_RecordStart(const char  * pUncompressedFile, const char  * pDecompressedFile, const char  * pMicInputFile);
qboolean Voice_IsRecording();

void Voice_RegisterCvars();

void Voice_Deinit();

void Voice_Idle(float frametime);

qboolean Voice_RecordStop();