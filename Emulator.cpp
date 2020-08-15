// Emulator.cpp

#include "stdafx.h"
#include "main.h"
#include "mainwindow.h"
#include "Emulator.h"
#include "emubase/Emubase.h"
#include "qsoundout.h"
#include <QTime>
#include <QFile>


//////////////////////////////////////////////////////////////////////


CMotherboard* g_pBoard = nullptr;
QSoundOut * g_sound = nullptr;

bool g_okEmulatorInitialized = false;
bool g_okEmulatorRunning = false;

int m_wEmulatorCPUBpsCount = 0;
int m_wEmulatorPPUBpsCount = 0;
quint16 m_EmulatorCPUBps[MAX_BREAKPOINTCOUNT + 1];
quint16 m_EmulatorPPUBps[MAX_BREAKPOINTCOUNT + 1];
quint16 m_wEmulatorTempCPUBreakpoint = 0177777;
quint16 m_wEmulatorTempPPUBreakpoint = 0177777;

bool m_okEmulatorSound = false;

long m_nFrameCount = 0;
QTime m_emulatorTime;
int m_nTickCount = 0;
quint32 m_dwEmulatorUptime = 0;  // UKNC uptime, seconds, from turn on or reset, increments every 25 frames
long m_nUptimeFrameCount = 0;

quint8* g_pEmulatorRam[3];  // RAM values - for change tracking
quint8* g_pEmulatorChangedRam[3];  // RAM change flags
quint16 g_wEmulatorCpuPC = 0177777;      // Current PC value
quint16 g_wEmulatorPrevCpuPC = 0177777;  // Previous PC value
quint16 g_wEmulatorPpuPC = 0177777;      // Current PC value
quint16 g_wEmulatorPrevPpuPC = 0177777;  // Previous PC value

const int KEYEVENT_QUEUE_SIZE = 32;
quint16 m_EmulatorKeyQueue[KEYEVENT_QUEUE_SIZE];
int m_EmulatorKeyQueueTop = 0;
int m_EmulatorKeyQueueBottom = 0;
int m_EmulatorKeyQueueCount = 0;


void CALLBACK Emulator_FeedDAC(unsigned short l, unsigned short r);


//////////////////////////////////////////////////////////////////////

bool Emulator_Init()
{
    ASSERT(g_pBoard == nullptr);

    ::memset(g_pEmulatorRam, 0, sizeof(g_pEmulatorRam));
    ::memset(g_pEmulatorChangedRam, 0, sizeof(g_pEmulatorChangedRam));
    CProcessor::Init();

    m_wEmulatorCPUBpsCount = m_wEmulatorPPUBpsCount = 0;
    for (int i = 0; i <= MAX_BREAKPOINTCOUNT; i++)
    {
        m_EmulatorCPUBps[i] = 0177777;
        m_EmulatorPPUBps[i] = 0177777;
    }

    g_pBoard = new CMotherboard();

    quint8 buffer[32768];

    // Load ROM file
    memset(buffer, 0, 32768);
    QFile romFile(":/uknc_rom.bin");
    if (!romFile.open(QIODevice::ReadOnly))
    {
        AlertWarning(QT_TRANSLATE_NOOP("Emulator", "Failed to load ROM file."));
        return false;
    }
    qint64 bytesRead = romFile.read((char*)buffer, 32256);
    romFile.close();
    if (bytesRead != 32256)
    {
        AlertWarning(QT_TRANSLATE_NOOP("Emulator", "Failed to load ROM file."));
        return false;
    }

    g_pBoard->LoadROM(buffer);

    //g_pBoard->SetNetStation((uint16_t)Settings_GetNetStation());

    g_pBoard->Reset();

    g_sound = new QSoundOut();
    if (m_okEmulatorSound)
    {
        g_pBoard->SetSoundGenCallback(Emulator_FeedDAC);
    }

    m_nUptimeFrameCount = 0;
    m_dwEmulatorUptime = 0;

    // Allocate memory for old RAM values
    for (int i = 0; i < 3; i++)
    {
        g_pEmulatorRam[i] = (quint8*) ::calloc(65536, 1);
        g_pEmulatorChangedRam[i] = (quint8*) ::calloc(65536, 1);
    }

    g_okEmulatorInitialized = true;
    return true;
}

void Emulator_Done()
{
    ASSERT(g_pBoard != nullptr);

    CProcessor::Done();

    g_pBoard->SetSoundGenCallback(nullptr);
    if (g_sound)
    {
        delete g_sound;
        g_sound = nullptr;
    }

    delete g_pBoard;
    g_pBoard = nullptr;

    // Free memory used for old RAM values
    for (int i = 0; i < 3; i++)
    {
        ::free(g_pEmulatorRam[i]);
        ::free(g_pEmulatorChangedRam[i]);
    }

    g_okEmulatorInitialized = false;
}

void Emulator_Start()
{
    g_okEmulatorRunning = true;

    // Set title bar text
    Global_getMainWindow()->updateWindowText();

    m_nFrameCount = 0;
    m_emulatorTime.restart();
    m_nTickCount = 0;

    // For proper breakpoint processing
    if (m_wEmulatorCPUBpsCount != 0 || m_wEmulatorPPUBpsCount)
    {
        g_pBoard->GetCPU()->ClearInternalTick();
        g_pBoard->GetPPU()->ClearInternalTick();
    }
}
void Emulator_Stop()
{
    g_okEmulatorRunning = false;

    Emulator_SetTempCPUBreakpoint(0177777);
    Emulator_SetTempPPUBreakpoint(0177777);

    // Set title bar text
    Global_getMainWindow()->updateWindowText();

    // Reset FPS indicator
    Global_showFps(-1.0);

    Global_UpdateAllViews();
}

void Emulator_Reset()
{
    ASSERT(g_pBoard != nullptr);

    g_pBoard->Reset();

    m_nUptimeFrameCount = 0;
    m_dwEmulatorUptime = 0;
    Global_showUptime(0);

    Global_UpdateAllViews();
}

bool Emulator_AddCPUBreakpoint(quint16 address)
{
    if (m_wEmulatorCPUBpsCount == MAX_BREAKPOINTCOUNT - 1 || address == 0177777)
        return false;
    for (int i = 0; i < m_wEmulatorCPUBpsCount; i++)  // Check if the BP exists
    {
        if (m_EmulatorCPUBps[i] == address)
            return false;  // Already in the list
    }
    for (int i = 0; i < MAX_BREAKPOINTCOUNT; i++)  // Put in the first empty cell
    {
        if (m_EmulatorCPUBps[i] == 0177777)
        {
            m_EmulatorCPUBps[i] = address;
            break;
        }
    }
    m_wEmulatorCPUBpsCount++;
    return true;
}
bool Emulator_AddPPUBreakpoint(quint16 address)
{
    if (m_wEmulatorPPUBpsCount == MAX_BREAKPOINTCOUNT - 1 || address == 0177777)
        return false;
    for (int i = 0; i < m_wEmulatorPPUBpsCount; i++)  // Check if the BP exists
    {
        if (m_EmulatorPPUBps[i] == address)
            return false;  // Already in the list
    }
    for (int i = 0; i < MAX_BREAKPOINTCOUNT; i++)  // Put in the first empty cell
    {
        if (m_EmulatorPPUBps[i] == 0177777)
        {
            m_EmulatorPPUBps[i] = address;
            break;
        }
    }
    m_wEmulatorPPUBpsCount++;
    return true;
}
bool Emulator_RemoveCPUBreakpoint(quint16 address)
{
    if (m_wEmulatorCPUBpsCount == 0 || address == 0177777)
        return false;
    for (int i = 0; i < MAX_BREAKPOINTCOUNT; i++)
    {
        if (m_EmulatorCPUBps[i] == address)
        {
            m_EmulatorCPUBps[i] = 0177777;
            m_wEmulatorCPUBpsCount--;
            if (m_wEmulatorCPUBpsCount > i)  // fill the hole
            {
                m_EmulatorCPUBps[i] = m_EmulatorCPUBps[m_wEmulatorCPUBpsCount];
                m_EmulatorCPUBps[m_wEmulatorCPUBpsCount] = 0177777;
            }
            return true;
        }
    }
    return false;
}
bool Emulator_RemovePPUBreakpoint(quint16 address)
{
    if (m_wEmulatorPPUBpsCount == 0 || address == 0177777)
        return false;
    for (int i = 0; i < MAX_BREAKPOINTCOUNT; i++)
    {
        if (m_EmulatorPPUBps[i] == address)
        {
            m_EmulatorPPUBps[i] = 0177777;
            m_wEmulatorPPUBpsCount--;
            if (m_wEmulatorPPUBpsCount > i)  // fill the hole
            {
                m_EmulatorPPUBps[i] = m_EmulatorPPUBps[m_wEmulatorPPUBpsCount];
                m_EmulatorPPUBps[m_wEmulatorPPUBpsCount] = 0177777;
            }
            return true;
        }
    }
    return false;
}
void Emulator_SetTempCPUBreakpoint(quint16 address)
{
    if (m_wEmulatorTempCPUBreakpoint != 0177777)
        Emulator_RemoveCPUBreakpoint(m_wEmulatorTempCPUBreakpoint);
    if (address == 0177777)
    {
        m_wEmulatorTempCPUBreakpoint = 0177777;
        return;
    }
    for (int i = 0; i < MAX_BREAKPOINTCOUNT; i++)
    {
        if (m_EmulatorCPUBps[i] == address)
            return;  // We have regular breakpoint with the same address
    }
    m_wEmulatorTempCPUBreakpoint = address;
    m_EmulatorCPUBps[m_wEmulatorCPUBpsCount] = address;
    m_wEmulatorCPUBpsCount++;
}
void Emulator_SetTempPPUBreakpoint(quint16 address)
{
    if (m_wEmulatorTempPPUBreakpoint != 0177777)
        Emulator_RemovePPUBreakpoint(m_wEmulatorTempPPUBreakpoint);
    if (address == 0177777)
    {
        m_wEmulatorTempPPUBreakpoint = 0177777;
        return;
    }
    for (int i = 0; i < MAX_BREAKPOINTCOUNT; i++)
    {
        if (m_EmulatorPPUBps[i] == address)
            return;  // We have regular breakpoint with the same address
    }
    m_wEmulatorTempPPUBreakpoint = address;
    m_EmulatorPPUBps[m_wEmulatorPPUBpsCount] = address;
    m_wEmulatorPPUBpsCount++;
}
const quint16* Emulator_GetCPUBreakpointList() { return m_EmulatorCPUBps; }
const quint16* Emulator_GetPPUBreakpointList() { return m_EmulatorPPUBps; }
bool Emulator_IsBreakpoint()
{
    quint16 address = g_pBoard->GetCPU()->GetPC();
    if (m_wEmulatorCPUBpsCount > 0)
    {
        for (int i = 0; i < m_wEmulatorCPUBpsCount; i++)
        {
            if (address == m_EmulatorCPUBps[i])
                return true;
        }
    }
    address = g_pBoard->GetPPU()->GetPC();
    if (m_wEmulatorPPUBpsCount > 0)
    {
        for (int i = 0; i < m_wEmulatorPPUBpsCount; i++)
        {
            if (address == m_EmulatorPPUBps[i])
                return true;
        }
    }
    return false;
}
bool Emulator_IsBreakpoint(bool okCpuPpu, quint16 address)
{
    int bpsCount = okCpuPpu ? m_wEmulatorCPUBpsCount : m_wEmulatorPPUBpsCount;
    quint16* pbps = okCpuPpu ? m_EmulatorCPUBps : m_EmulatorPPUBps;
    if (bpsCount == 0)
        return false;
    for (int i = 0; i < bpsCount; i++)
    {
        if (address == pbps[i])
            return true;
    }
    return false;
}
void Emulator_RemoveAllBreakpoints(bool okCpuPpu)
{
    quint16* pbps = okCpuPpu ? m_EmulatorCPUBps : m_EmulatorPPUBps;
    for (int i = 0; i < MAX_BREAKPOINTCOUNT; i++)
        pbps[i] = 0177777;
    if (okCpuPpu)
        m_wEmulatorCPUBpsCount = 0;
    else
        m_wEmulatorPPUBpsCount = 0;
}

bool Emulator_SystemFrame()
{
    Emulator_ProcessKeyEvent();

    g_pBoard->SetCPUBreakpoints(m_wEmulatorCPUBpsCount > 0 ? m_EmulatorCPUBps : nullptr);
    g_pBoard->SetPPUBreakpoints(m_wEmulatorPPUBpsCount > 0 ? m_EmulatorPPUBps : nullptr);

    if (!g_pBoard->SystemFrame())  // Breakpoint hit
    {
        Emulator_SetTempCPUBreakpoint(0177777);
        Emulator_SetTempPPUBreakpoint(0177777);
        return false;
    }

    // Calculate frames per second
    m_nFrameCount++;
    int nCurrentTicks = m_emulatorTime.elapsed();
    long nTicksElapsed = nCurrentTicks - m_nTickCount;
    if (nTicksElapsed >= 1200)
    {
        double dFramesPerSecond = m_nFrameCount * 1000.0 / nTicksElapsed;
        Global_showFps(dFramesPerSecond);

        m_nFrameCount = 0;
        m_nTickCount = nCurrentTicks;
    }

    // Calculate emulator uptime (25 frames per second)
    m_nUptimeFrameCount++;
    if (m_nUptimeFrameCount >= 25)
    {
        m_dwEmulatorUptime++;
        m_nUptimeFrameCount = 0;

        Global_showUptime(m_dwEmulatorUptime);
    }

    return true;
}

float Emulator_GetUptime()
{
    return (float)m_dwEmulatorUptime + float(m_nUptimeFrameCount) / 25.0f;
}

// Update cached values after Run or Step
void Emulator_OnUpdate()
{
    // Update stored PC value
    g_wEmulatorPrevCpuPC = g_wEmulatorCpuPC;
    g_wEmulatorCpuPC = g_pBoard->GetCPU()->GetPC();
    g_wEmulatorPrevPpuPC = g_wEmulatorPpuPC;
    g_wEmulatorPpuPC = g_pBoard->GetPPU()->GetPC();

    // Update memory change flags
    for (int plane = 0; plane < 3; plane++)
    {
        quint8* pOld = g_pEmulatorRam[plane];
        quint8* pChanged = g_pEmulatorChangedRam[plane];
        quint16 addr = 0;
        do
        {
            quint8 newvalue = g_pBoard->GetRAMByte(plane, addr);
            quint8 oldvalue = *pOld;
            *pChanged = (newvalue != oldvalue) ? 255 : 0;
            *pOld = newvalue;
            addr++;
            pOld++;  pChanged++;
        }
        while (addr < 65535);
    }
}

// Get RAM change flag for RAM word
//   addrtype - address mode - see ADDRTYPE_XXX constants
quint16 Emulator_GetChangeRamStatus(int addrtype, quint16 address)
{
    switch (addrtype)
    {
    case ADDRTYPE_RAM0:
    case ADDRTYPE_RAM1:
    case ADDRTYPE_RAM2:
        return *((quint16*)(g_pEmulatorChangedRam[addrtype] + address));
    case ADDRTYPE_RAM12:
        if (address < 0170000)
            return MAKEWORD(
                    *(g_pEmulatorChangedRam[1] + address / 2),
                    *(g_pEmulatorChangedRam[2] + address / 2));
        else
            return 0;
    default:
        return 0;
    }
}

bool Emulator_LoadROMCartridge(int slot, LPCTSTR sFilePath)
{
    // Open file
    FILE* fpFile = ::fopen(sFilePath, "rb");
    if (fpFile == nullptr)
    {
        AlertWarning(QT_TRANSLATE_NOOP("Emulator", "Failed to load ROM cartridge image."));
        return false;
    }

    // Allocate memory
    quint8* pImage = (quint8*) ::calloc(24 * 1024, 1);
    if (pImage == nullptr)
    {
        ::fclose(fpFile);
        return false;
    }
    size_t dwBytesRead = ::fread(pImage, 1, 24 * 1024, fpFile);
    if (dwBytesRead != 24 * 1024)
    {
        AlertWarning(QT_TRANSLATE_NOOP("Emulator", "Failed to load ROM cartridge image."));
        return false;
    }

    g_pBoard->LoadROMCartridge(slot, pImage);

    // Free memory, close file
    ::free(pImage);
    ::fclose(fpFile);

    //TODO: Save the file name for a future SaveImage() call

    return true;
}

void Emulator_PrepareScreenRGB32(void* pImageBits, const quint32* colors)
{
    if (pImageBits == nullptr) return;
    if (!g_okEmulatorInitialized) return;

    // Tag parsing loop
    quint8 cursorYRGB = 0;
    bool okCursorType = false;
    quint8 cursorPos = 128;
    bool cursorOn = false;
    quint8 cursorAddress = 0;  // Address of graphical cursor
    quint16 address = 0000270;  // Tag sequence start address
    bool okTagSize = false;  // Tag size: true - 4-word, false - 2-word (first tag is always 2-word)
    bool okTagType = false;  // Type of 4-word tag: true - set palette, false - set params
    int scale = 1;           // Horizontal scale: 1, 2, 4, or 8
    quint32 palette = 0;       // Palette
    qint32 palettecurrent[8];  memset(palettecurrent, 0, sizeof(palettecurrent)); // Current palette; update each time we change the "palette" variable
    quint8 pbpgpr = 0;         // 3-bit Y-value modifier
    for (int yy = 0; yy < 307; yy++)
    {
        if (okTagSize)  // 4-word tag
        {
            quint16 tag1 = g_pBoard->GetRAMWord(0, address);
            address += 2;
            quint16 tag2 = g_pBoard->GetRAMWord(0, address);
            address += 2;

            if (okTagType)  // 4-word palette tag
            {
                palette = ((quint32)tag1) | ((quint32)tag2 << 16);
            }
            else  // 4-word params tag
            {
                scale = (tag2 >> 4) & 3;  // Bits 4-5 - new scale value
                pbpgpr = (quint8)((7 - (tag2 & 7)) << 4);  // Y-value modifier
                cursorYRGB = (quint8)(tag1 & 15);  // Cursor color
                okCursorType = ((tag1 & 16) != 0);  // true - graphical cursor, false - symbolic cursor
                //ASSERT(okCursorType==0);  //DEBUG
                cursorPos = (quint8)(((tag1 >> 8) >> scale) & 0x7f);  // Cursor position in the line
                cursorAddress = (quint8)((tag1 >> 5) & 7);
                scale = 1 << scale;
            }
            for (uint8_t c = 0; c < 8; c++)  // Update palettecurrent
            {
                quint8 valueYRGB = (uint8_t) (palette >> (c << 2)) & 15;
                palettecurrent[c] = colors[pbpgpr | valueYRGB];
                //if (pbpgpr != 0) DebugLogFormat("pbpgpr %02x\r\n", pbpgpr | valueYRGB);
            }
        }

        quint16 addressBits = g_pBoard->GetRAMWord(0, address);  // The word before the last word - is address of bits from all three memory planes
        address += 2;

        // Calculate size, type and address of the next tag
        quint16 tagB = g_pBoard->GetRAMWord(0, address);  // Last word of the tag - is address and type of the next tag
        okTagSize = (tagB & 2) != 0;  // Bit 1 shows size of the next tag
        if (okTagSize)
        {
            address = tagB & ~7;
            okTagType = (tagB & 4) != 0;  // Bit 2 shows type of the next tag
        }
        else
            address = tagB & ~3;
        if ((tagB & 1) != 0)
            cursorOn = !cursorOn;

        // Draw bits into the bitmap, from line 20 to line 307
        if (yy < 19 /*|| yy > 306*/)
            continue;

        // Loop thru bits from addressBits, planes 0,1,2
        // For each pixel:
        //   Get bit from planes 0,1,2 and make value
        //   Map value to palette; result is 4-bit value YRGB
        //   Translate value to 24-bit RGB
        //   Put value to m_bits; repeat using scale value

        int xr = 640;
        int y = yy - 19;
        quint32* pBits = (static_cast<quint32*>(pImageBits)) + y * 640;
        int pos = 0;
        for (;;)
        {
            // Get bit from planes 0,1,2
            quint8 src0 = g_pBoard->GetRAMByte(0, addressBits);
            quint8 src1 = g_pBoard->GetRAMByte(1, addressBits);
            quint8 src2 = g_pBoard->GetRAMByte(2, addressBits);
            // Loop through the bits of the byte
            int bit = 0;
            for (;;)
            {
                quint32 valueRGB;
                if (cursorOn && (pos == cursorPos) && (!okCursorType || (okCursorType && bit == cursorAddress)))
                    valueRGB = colors[cursorYRGB];  // 4-bit to 32-bit color
                else
                {
                    // Make 3-bit value from the bits
                    quint8 value012 = (src0 & 1) | ((src1 & 1) << 1) | ((src2 & 1) << 2);
                    valueRGB = palettecurrent[value012];  // 3-bit to 32-bit color
                }

                // Put value to m_bits; repeat using scale value
                //WAS: for (int s = 0; s < scale; s++) *pBits++ = valueRGB;
                switch (scale)
                {
                case 8:
                    *pBits++ = valueRGB;
                    *pBits++ = valueRGB;
                    *pBits++ = valueRGB;
                    *pBits++ = valueRGB;
                case 4:
                    *pBits++ = valueRGB;
                    *pBits++ = valueRGB;
                case 2:
                    *pBits++ = valueRGB;
                case 1:
                    *pBits++ = valueRGB;
                default:
                    break;
                }

                xr -= scale;

                if (bit == 7)
                    break;
                bit++;

                // Shift to the next bit
                src0 >>= 1;
                src1 >>= 1;
                src2 >>= 1;
            }
            if (xr <= 0)
                break;  // End of line
            addressBits++;  // Go to the next byte
            pos++;
        }
    }
}

void Emulator_PrepareScreenToText(void* pImageBits, const quint32* colors)
{
    if (pImageBits == nullptr) return;
    if (!g_okEmulatorInitialized) return;

    // Tag parsing loop
    quint16 address = 0000270;  // Tag sequence start address
    bool okTagSize = false;  // Tag size: TRUE - 4-word, false - 2-word (first tag is always 2-word)
    bool okTagType = false;  // Type of 4-word tag: TRUE - set palette, false - set params
    int scale = 1;           // Horizontal scale: 1, 2, 4, or 8
    for (int yy = 0; yy < 307; yy++)
    {
        if (okTagSize)  // 4-word tag
        {
            //WORD tag1 = g_pBoard->GetRAMWord(0, address);
            address += 2;
            quint16 tag2 = g_pBoard->GetRAMWord(0, address);
            address += 2;

            if (okTagType)  // 4-word palette tag
            {
                //palette = MAKELONG(tag1, tag2);
            }
            else  // 4-word params tag
            {
                scale = (tag2 >> 4) & 3;  // Bits 4-5 - new scale value
                scale = 1 << scale;
            }
        }

        quint16 addressBits = g_pBoard->GetRAMWord(0, address);  // The word before the last word - is address of bits from all three memory planes
        address += 2;

        // Calculate size, type and address of the next tag
        quint16 tagB = g_pBoard->GetRAMWord(0, address);  // Last word of the tag - is address and type of the next tag
        okTagSize = (tagB & 2) != 0;  // Bit 1 shows size of the next tag
        if (okTagSize)
        {
            address = tagB & ~7;
            okTagType = (tagB & 4) != 0;  // Bit 2 shows type of the next tag
        }
        else
            address = tagB & ~3;

        // Draw bits into the bitmap, from line 20 to line 307
        if (yy < 19 /*|| yy > 306*/)
            continue;

        // Loop thru bits from addressBits, planes 0,1,2
        int xr = 640;
        int y = yy - 19;
        quint32* pBits = (static_cast<quint32*>(pImageBits)) + y * 640;
        int pos = 0;
        for (;;)
        {
            // Get bit from planes 0,1,2
            quint8 src0 = g_pBoard->GetRAMByte(0, addressBits);
            quint8 src1 = g_pBoard->GetRAMByte(1, addressBits);
            quint8 src2 = g_pBoard->GetRAMByte(2, addressBits);
            // Loop through the bits of the byte
            int bit = 0;
            for (;;)
            {
                // Make 3-bit value from the bits
                quint8 value012 = (src0 & 1) | ((src1 & 1) << 1) | ((src2 & 1) << 2);
                quint32 valueRGB = colors[value012];  // 3-bit to 32-bit color

                // Put value to m_bits; (do not repeat using scale value)
                *pBits++ = valueRGB;
                xr -= scale;

                if (bit == 7)
                    break;
                bit++;

                // Shift to the next bit
                src0 >>= 1;
                src1 >>= 1;
                src2 >>= 1;
            }
            if (xr <= 0)
                break;  // End of line
            addressBits++;  // Go to the next byte
            pos++;
        }
    }
}

void Emulator_KeyEvent(quint8 keyscan, bool pressed)
{
    if (m_EmulatorKeyQueueCount == KEYEVENT_QUEUE_SIZE) return;  // Full queue

    quint16 keyevent = MAKEWORD(keyscan, pressed ? 128 : 0);

    m_EmulatorKeyQueue[m_EmulatorKeyQueueTop] = keyevent;
    m_EmulatorKeyQueueTop++;
    if (m_EmulatorKeyQueueTop >= KEYEVENT_QUEUE_SIZE)
        m_EmulatorKeyQueueTop = 0;
    m_EmulatorKeyQueueCount++;
}

quint16 Emulator_GetKeyEventFromQueue()
{
    if (m_EmulatorKeyQueueCount == 0) return 0;  // Empty queue

    quint16 keyevent = m_EmulatorKeyQueue[m_EmulatorKeyQueueBottom];
    m_EmulatorKeyQueueBottom++;
    if (m_EmulatorKeyQueueBottom >= KEYEVENT_QUEUE_SIZE)
        m_EmulatorKeyQueueBottom = 0;
    m_EmulatorKeyQueueCount--;

    return keyevent;
}

void Emulator_ProcessKeyEvent()
{
    // Process next event in the keyboard queue
    quint16 keyevent = Emulator_GetKeyEventFromQueue();
    if (keyevent != 0)
    {
        bool pressed = ((keyevent & 0x8000) != 0);
        quint8 ukncscan = (quint8)(keyevent & 0xff);
        g_pBoard->KeyboardEvent(ukncscan, pressed);
    }
}

void CALLBACK Emulator_FeedDAC(unsigned short l, unsigned short r)
{
    if (g_sound)
    {
        if (m_okEmulatorSound)
            g_sound->FeedDAC(l, r);
    }
}

void Emulator_SetSound(bool enable)
{
    m_okEmulatorSound = enable;
    if (g_pBoard != nullptr)
    {
        if (enable)
            g_pBoard->SetSoundGenCallback(Emulator_FeedDAC);
        else
            g_pBoard->SetSoundGenCallback(nullptr);
    }
}


//////////////////////////////////////////////////////////////////////
//
// Emulator image format - see CMotherboard::SaveToImage()
// Image header format (32 bytes):
//   4 bytes        UKNC_IMAGE_HEADER1
//   4 bytes        UKNC_IMAGE_HEADER2
//   4 bytes        UKNC_IMAGE_VERSION
//   4 bytes        UKNC_IMAGE_SIZE
//   4 bytes        UKNC uptime
//   12 bytes       Not used
//TODO: 256 bytes * 2 - Cartridge 1..2 path
//TODO: 256 bytes * 4 - Floppy 1..4 path
//TODO: 256 bytes * 2 - Hard 1..2 path

bool Emulator_SaveImage(const QString& sFilePath)
{
    QFile file(sFilePath);
    if (! file.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        AlertWarning(QT_TRANSLATE_NOOP("Emulator", "Failed to save image file."));
        return false;
    }

    // Allocate memory
    quint8* pImage = (quint8*) ::calloc(UKNCIMAGE_SIZE, 1);
    if (pImage == nullptr)
    {
        file.close();
        return false;
    }
    // Prepare header
    quint32* pHeader = (quint32*) pImage;
    *pHeader++ = UKNCIMAGE_HEADER1;
    *pHeader++ = UKNCIMAGE_HEADER2;
    *pHeader++ = UKNCIMAGE_VERSION;
    *pHeader++ = UKNCIMAGE_SIZE;
    // Store emulator state to the image
    g_pBoard->SaveToImage(pImage);
    *(quint32*)(pImage + 16) = m_dwEmulatorUptime;

    // Save image to the file
    qint64 bytesWritten = file.write((const char *)pImage, UKNCIMAGE_SIZE);
    if (bytesWritten != UKNCIMAGE_SIZE)
    {
        AlertWarning(QT_TRANSLATE_NOOP("Emulator", "Failed to save image file data."));
        return false;
    }

    // Free memory, close file
    ::free(pImage);
    file.close();

    return true;
}

bool Emulator_LoadImage(const QString &sFilePath)
{
    Emulator_Stop();

    QFile file(sFilePath);
    if (! file.open(QIODevice::ReadOnly))
    {
        AlertWarning(QT_TRANSLATE_NOOP("Emulator", "Failed to load image file."));
        return false;
    }

    // Read header
    quint32 bufHeader[UKNCIMAGE_HEADER_SIZE / sizeof(quint32)];
    qint64 bytesRead = file.read((char*)bufHeader, UKNCIMAGE_HEADER_SIZE);
    if (bytesRead != UKNCIMAGE_HEADER_SIZE)
    {
        file.close();
        return false;
    }

    //TODO: Check version and size

    // Allocate memory
    quint8* pImage = (quint8*) ::malloc(UKNCIMAGE_SIZE);
    if (pImage == nullptr)
    {
        file.close();
        return false;
    }

    // Read image
    file.seek(0);
    bytesRead = file.read((char*)pImage, UKNCIMAGE_SIZE);
    if (bytesRead != UKNCIMAGE_SIZE)
    {
        ::free(pImage);
        file.close();
        AlertWarning(QT_TRANSLATE_NOOP("Emulator", "Failed to load image file data."));
        return false;
    }
    else
    {
        // Restore emulator state from the image
        g_pBoard->LoadFromImage(pImage);

        m_dwEmulatorUptime = *(quint32*)(pImage + 16);
    }

    // Free memory, close file
    ::free(pImage);
    file.close();

    return true;
}


//////////////////////////////////////////////////////////////////////
