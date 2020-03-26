// Emulator.h

#pragma once

#include "emubase/Board.h"

//////////////////////////////////////////////////////////////////////


extern CMotherboard* g_pBoard;

extern bool g_okEmulatorRunning;

extern quint8* g_pEmulatorRam[3];  // RAM values - for change tracking
extern quint8* g_pEmulatorChangedRam[3];  // RAM change flags
extern quint16 g_wEmulatorCpuPC;      // Current PC value
extern quint16 g_wEmulatorPrevCpuPC;  // Previous PC value
extern quint16 g_wEmulatorPpuPC;      // Current PC value
extern quint16 g_wEmulatorPrevPpuPC;  // Previous PC value


//////////////////////////////////////////////////////////////////////

void Emulator_SetSound(bool enable);
bool Emulator_Init();
void Emulator_Done();
void Emulator_SetCPUBreakpoint(quint16 address);
void Emulator_SetPPUBreakpoint(quint16 address);
bool Emulator_IsBreakpoint();
void Emulator_Start();
void Emulator_Stop();
void Emulator_Reset();
int Emulator_SystemFrame();
float Emulator_GetUptime();  // UKNC uptime, in seconds

void Emulator_PrepareScreenRGB32(void* pBits, const quint32* colors);
void Emulator_PrepareScreenToText(void* pBits, const quint32* colors);

void Emulator_KeyEvent(quint8 keyPressed, bool pressed);
quint16 Emulator_GetKeyEventFromQueue();
void Emulator_ProcessKeyEvent();

bool Emulator_LoadROMCartridge(int slot, LPCTSTR sFilePath);

// Update cached values after Run or Step
void Emulator_OnUpdate();
quint16 Emulator_GetChangeRamStatus(int addrtype, quint16 address);

bool Emulator_SaveImage(const QString &sFilePath);
bool Emulator_LoadImage(const QString &sFilePath);


//////////////////////////////////////////////////////////////////////
