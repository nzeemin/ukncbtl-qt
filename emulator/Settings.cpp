#include "stdafx.h"
#include <QSettings>
#include "main.h"


//////////////////////////////////////////////////////////////////////
// Options

bool Option_ShowHelp = false;
int Option_AutoBoot = 0;


//////////////////////////////////////////////////////////////////////


void Settings_SetFloppyFilePath(int slot, const QString & sFilePath)
{
    char bufValueName[] = "Floppy0";
    bufValueName[6] = slot + '0';
    Global_getSettings()->setValue(bufValueName, sFilePath);
}
QString Settings_GetFloppyFilePath(int slot)
{
    char bufValueName[] = "Floppy0";
    bufValueName[6] = slot + '0';
    QVariant value = Global_getSettings()->value(bufValueName, "");
    return value.toString();
}

void Settings_SetCartridgeFilePath(int slot, const QString &sFilePath)
{
    char bufValueName[] = "Cartridge1";
    bufValueName[9] = slot + '0';
    Global_getSettings()->setValue(bufValueName, sFilePath);
}
QString Settings_GetCartridgeFilePath(int slot)
{
    char bufValueName[] = "Cartridge1";
    bufValueName[9] = slot + '0';
    QVariant value = Global_getSettings()->value(bufValueName, "");
    return value.toString();
}

void Settings_SetHardFilePath(int slot, const QString &sFilePath)
{
    char bufValueName[] = "Hard1";
    bufValueName[4] = slot + '0';
    Global_getSettings()->setValue(bufValueName, sFilePath);
}
QString Settings_GetHardFilePath(int slot)
{
    char bufValueName[] = "Hard1";
    bufValueName[4] = slot + '0';
    QVariant value = Global_getSettings()->value(bufValueName, "");
    return value.toString();
}

void Settings_SetAutostart(bool flag)
{
    Global_getSettings()->setValue("Autostart", flag);
}
bool Settings_GetAutostart()
{
    QVariant value = Global_getSettings()->value("Autostart", false);
    return value.toBool();
}

void Settings_SetSound(bool flag)
{
    Global_getSettings()->setValue("Sound", flag);
}
bool Settings_GetSound()
{
    QVariant value = Global_getSettings()->value("Sound", false);
    return value.toBool();
}

void Settings_SetSoundAY(bool flag)
{
    Global_getSettings()->setValue("SoundAY", flag);
}
bool Settings_GetSoundAY()
{
    QVariant value = Global_getSettings()->value("SoundAY", false);
    return value.toBool();
}

void Settings_SetDebugCpuPpu(bool flag)
{
    Global_getSettings()->setValue("DebugCpuPpu", flag);
}
bool Settings_GetDebugCpuPpu()
{
    QVariant value = Global_getSettings()->value("DebugCpuPpu", false);
    return value.toBool();
}

void Settings_SetDebugMemoryMode(quint16 mode)
{
    Global_getSettings()->setValue("DebugMemoryMode", mode);
}
quint16 Settings_GetDebugMemoryMode()
{
    QVariant value = Global_getSettings()->value("DebugMemoryMode", 3);
    return (quint16)value.toUInt();
}

void Settings_SetDebugMemoryAddress(quint16 mode)
{
    Global_getSettings()->setValue("DebugMemoryAddress", mode);
}
quint16 Settings_GetDebugMemoryAddress()
{
    QVariant value = Global_getSettings()->value("DebugMemoryAddress", 0);
    return (quint16)value.toUInt();
}

void Settings_SetDebugMemoryByte(bool flag)
{
    Global_getSettings()->setValue("DebugMemoryByte", flag);
}
bool Settings_GetDebugMemoryByte()
{
    QVariant value = Global_getSettings()->value("DebugMemoryByte", false);
    return value.toBool();
}

void Settings_SetDebugMemoryNumeral(quint16 mode)
{
    Global_getSettings()->setValue("DebugMemoryNumeral", mode);
}
quint16 Settings_GetDebugMemoryNumeral()
{
    QVariant value = Global_getSettings()->value("DebugMemoryNumeral", 0);
    return (quint16)value.toUInt();
}


//////////////////////////////////////////////////////////////////////
