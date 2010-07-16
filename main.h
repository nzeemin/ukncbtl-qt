#ifndef MAIN_H
#define MAIN_H

//////////////////////////////////////////////////////////////////////

class QApplication;
class MainWindow;
class QSettings;


QApplication* Global_getApplication();
MainWindow* Global_getMainWindow();
QSettings* Global_getSettings();
void Global_UpdateAllViews();
void Global_UpdateMenu();
void Global_SetCurrentProc(bool);


//////////////////////////////////////////////////////////////////////
// Settings

void Settings_SetFloppyFilePath(int slot, LPCTSTR sFilePath);
QString Settings_GetFloppyFilePath(int slot);
void Settings_SetCartridgeFilePath(int slot, LPCTSTR sFilePath);
QString Settings_GetCartridgeFilePath(int slot);
void Settings_SetHardFilePath(int slot, LPCTSTR sFilePath);
QString Settings_GetHardFilePath(int slot);


//////////////////////////////////////////////////////////////////////
#endif // MAIN_H
