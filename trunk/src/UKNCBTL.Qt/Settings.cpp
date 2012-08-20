#include "stdafx.h"
#include <QSettings>
#include "main.h"


//////////////////////////////////////////////////////////////////////


void Settings_SetFloppyFilePath(int slot, LPCTSTR sFilePath)
{
    TCHAR bufValueName[] = _T("Floppy0");
    bufValueName[6] = slot + _T('0');
    Global_getSettings()->setValue(bufValueName, sFilePath);
}

QString Settings_GetFloppyFilePath(int slot)
{
    TCHAR bufValueName[] = _T("Floppy0");
    bufValueName[6] = slot + _T('0');
    QVariant value = Global_getSettings()->value(bufValueName, _T(""));
    return value.toString();
}

void Settings_SetCartridgeFilePath(int slot, LPCTSTR sFilePath)
{
    TCHAR bufValueName[] = _T("Cartridge1");
    bufValueName[9] = slot + _T('0');
    Global_getSettings()->setValue(bufValueName, sFilePath);
}

QString Settings_GetCartridgeFilePath(int slot)
{
    TCHAR bufValueName[] = _T("Cartridge1");
    bufValueName[9] = slot + _T('0');
    QVariant value = Global_getSettings()->value(bufValueName, _T(""));
    return value.toString();
}

void Settings_SetHardFilePath(int slot, LPCTSTR sFilePath)
{
    TCHAR bufValueName[] = _T("Hard1");
    bufValueName[4] = slot + _T('0');
    Global_getSettings()->setValue(bufValueName, sFilePath);
}

QString Settings_GetHardFilePath(int slot)
{
    TCHAR bufValueName[] = _T("Hard1");
    bufValueName[4] = slot + _T('0');
    QVariant value = Global_getSettings()->value(bufValueName, _T(""));
    return value.toString();
}

void Settings_SetSound(bool flag)
{
    Global_getSettings()->setValue(_T("Sound"), flag);
}

bool Settings_GetSound()
{
    QVariant value = Global_getSettings()->value(_T("Sound"), false);
    return value.toBool();
}



//////////////////////////////////////////////////////////////////////
