#pragma once

#include <windows.h>
#include <filesystem>

// \note we abuse the fact that the project (CamStudioRecorder or CamStudioPlayerPlus) have a
//       resource.h file.
#include "resource.h"

static
void OnAudioVolume(HWND hwnd)
{
    // Ver 1.1
    if (waveInGetNumDevs() == 0)
    {
#ifdef IDS_STRING_NOINPUT1
        MessageOut(hwnd, IDS_STRING_NOINPUT1, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
#else
        CString msgstr;
        msgstr.Format(_T("Unable to detect audio input device. You need a sound card with microphone input."));
        MessageBox(hwnd, msgstr, _T("Note"), MB_OK | MB_ICONEXCLAMATION);
#endif
        return;
    }

    char dirx[300];
    GetWindowsDirectoryA(dirx, 300);
    std::string Windir(dirx);

    // Test Windows\sndvol32.exe
    std::string exeFileName("\\sndvol32.exe");
    std::string AppDir = Windir;
    std::string SubDir = "";
    std::string testLaunchPath = AppDir + SubDir + exeFileName;

    std::string launchPath;

    // Verify sndvol32.exe exists
    if (std::experimental::filesystem::exists(testLaunchPath))
    {
        launchPath = testLaunchPath;
    }


    if (launchPath.empty())
    {
        // Test Windows\system32\sndvol32.exe
        // Verify sndvol32.exe exists
        SubDir = "\\system32";
        testLaunchPath = AppDir + SubDir + exeFileName;
        if (std::experimental::filesystem::exists(testLaunchPath))
        {
            launchPath = testLaunchPath;
        }
    }

    if (launchPath.empty())
    {
        // Test Windows\system\sndvol32.exe
        // Verify sndvol32.exe exists
        SubDir = "\\system";
        testLaunchPath = AppDir + SubDir + exeFileName;
        if (std::experimental::filesystem::exists(testLaunchPath))
        {
            launchPath = testLaunchPath;
        }
    }

    if (launchPath.empty())
    { // launch Volume Control
        // not sure
        launchPath = launchPath + " /r /rec /record";

        if (WinExec(launchPath.c_str(), SW_SHOW) != 0)
        {
        }
        else
        {
            
#ifdef IDS_STRING_ERRVOLCTRL1
            MessageOut(hwnd, IDS_STRING_ERRVOLCTRL1, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
#else
            MessageBox(hwnd, _T("Error launching Volume Control!"), _T("Note"), MB_OK | MB_ICONEXCLAMATION);
#endif
        }
    }
    // Sound mixer moved in Windows Vista! check new exe name only if windows version matches
    OSVERSIONINFO osinfo;
    osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (GetVersionEx((LPOSVERSIONINFO)&osinfo))
    {
        if (osinfo.dwMajorVersion >= 6) // Vista
        {
            testLaunchPath = AppDir + SubDir + "\\SndVol.exe";
            if (std::experimental::filesystem::exists(testLaunchPath))
            {
                launchPath = testLaunchPath;
            }
        }
    }

    if (launchPath != "")
    { // launch Volume Control
        // not sure
        launchPath = launchPath + " /r /rec /record";

        if (WinExec(launchPath.c_str(), SW_SHOW) != 0)
        {
        }
        else
        {
            
#ifdef IDS_STRING_ERRVOLCTRL1
            MessageOut(hwnd, IDS_STRING_ERRVOLCTRL1, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
#else
            MessageBox(hwnd, _T("Error launching Volume Control!"),_T("Note"),MB_OK | MB_ICONEXCLAMATION);
#endif
        }
    }
}