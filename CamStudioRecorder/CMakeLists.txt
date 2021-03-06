# Copyright (C) 2018  Steven Hoving
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(CamStudioRecorder)

set(RECORDER_SOURCE
    AviWriter.cpp
    AviWriter.h
    AudioFormatDlg.cpp
    AudioFormatDlg.h
    AudioMixer.cpp
    AudioMixer.h
    AudioSpeakersDlg.cpp
    AudioSpeakersDlg.h
    AutopanSpeedDlg.cpp
    AutopanSpeedDlg.h
    AutoSearchDlg.cpp
    AutoSearchDlg.h
    AVI.cpp
    AVI.h
    AVICompressorFilter.cpp
    AVICompressorFilter.h
    BasicMessageDlg.cpp
    BasicMessageDlg.h
    CamCursor.cpp
    CamCursor.h
    CamStudioCommandLineInfo.cpp
    CursorOptionsDlg.cpp
    CursorOptionsDlg.h
    EditImageDlg.cpp
    EditImageDlg.h
    EditTransparencyDlg.cpp
    EditTransparencyDlg.h
    FixedRegionDlg.cpp
    FixedRegionDlg.h
    FlashingWnd.cpp
    FolderDlg.cpp
    FolderDlg.h
    FrameGrabber.cpp
    FrameGrabber.h
    GdiPlusInitializer.cpp
    HotKey.cpp
    KeyshortcutsDlg.cpp
    KeyshortcutsDlg.h
    LayeredWindowHelperST.cpp
    LayoutList.cpp
    ListManager.cpp
    MainFrm.cpp
    MCI.cpp
    MouseCaptureWnd.cpp
    MP4Converter.cpp
    NewShapeDlg.cpp
    NewShapeDlg.h
    PresetTimeDlg.cpp
    PresetTimeDlg.h
    Profile.cpp
    ProgressDlg.cpp
    Recorder.cpp
    RecorderDoc.cpp
    RecorderView.cpp
    ResizeDlg.cpp
    Screen.cpp
    Screen.h
    ScreenAnnotationsDlg.cpp
    ScreenAnnotationsDlg.h
    SpeakersImpl.cpp
    stdafx.cpp
    stdafx.h
    SyncDlg.cpp
    SyncDlg.h
    TextDlg.cpp
    TextDlg.h
    TransparentWnd.cpp
    TransRateDlg.cpp
    TransRateDlg.h
    TroubleShootDlg.cpp
    TroubleShootDlg.h
    VideoOptionsDlg.cpp
    VideoOptionsDlg.h
    VideoWnd.cpp
    CamStudioCommandLineInfo.h
    FlashingWnd.h
    GdiPlusInitializer.h
    HotKey.h
    LayeredWindowHelperST.h
    LayoutList.h
    ListManager.h
    MainFrm.h
    MCI.h
    MouseCaptureWnd.h
    MP4Converter.h
    Profile.h
    ProgressDlg.h
    Recorder.h
    RecorderDoc.h
    RecorderVersionReleaseInfo.h
    RecorderView.h
    ResizeDlg.h
    resource.h
    targetver.h
    TransparentWnd.h
    VideoWnd.h
    string_convert.h
)

set(RECORDER_VFW_SOURCE
    vfw/ACM.cpp
    vfw/ACM.h
    vfw/VCM.cpp
    vfw/VCM.h
)

set(RECORDER_ADDONS_SOURCE
    addons/AnnotationEffectsOptionsDlg.cpp
    addons/AnnotationEffectsOptionsDlg.h
    addons/EffectsOptionsDlg.cpp
    addons/EffectsOptionsDlg.h
    addons/EffectsOptions2Dlg.cpp
    addons/EffectsOptions2Dlg.h
    addons/ImageAttributes.h
    addons/TextAttributes.h
)

set(RECORDER_RESOURCE
    Recorder.rc
)

set(CMAKE_MFC_FLAG 2)

add_executable(CamStudioRecorder WIN32
    ${RECORDER_SOURCE}
    ${RECORDER_RESOURCE}
    ${RECORDER_VFW_SOURCE}
    ${RECORDER_ADDONS_SOURCE}
)

source_group(vfw FILES
    ${RECORDER_VFW_SOURCE}
)

source_group(addons FILES
    ${RECORDER_ADDONS_SOURCE}
)

source_group(src FILES
    ${RECORDER_SOURCE}
    ${RECORDER_RESOURCE}
)

target_include_directories(CamStudioRecorder
  PUBLIC
    .
    ${libconfig_SOURCE_DIR}/lib
)

target_compile_definitions(CamStudioRecorder
  PRIVATE
    -DNOMINMAX
    -D_AFXDLL=1
    -D_UNICODE
    -DUNICODE
    # silence codecvt deprication warning for now
    -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)

target_compile_options(CamStudioRecorder
  PRIVATE
    /experimental:external
    /external:W0
    /external:anglebrackets
)

set_target_properties(CamStudioRecorder PROPERTIES
  COMPILE_FLAGS "/Yustdafx.h")

set_source_files_properties(stdafx.cpp PROPERTIES
    COMPILE_FLAGS "/Ycstdafx.h")

target_link_libraries(CamStudioRecorder
    CamLib
    CamHook
    CamAudio
    libconfig++
    CxImage
    localization
    fmt
    gdi32.lib
    winmm.lib
    vfw32.lib
    Gdiplus.lib
    strmiids.lib
    Msacm32.lib
)

set_target_properties(CamStudioRecorder PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$(Configuration)
)

install(TARGETS CamStudioRecorder
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
