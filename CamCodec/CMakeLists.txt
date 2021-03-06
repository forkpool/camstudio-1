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

project(CamCodec)

set(CODEC_SOURCE
    camcodec.cpp
    camcodec.h
    camcodec.def
    camcodec.rc
    drvproc.cpp
    resource.h
)

source_group(CamCodec FILES
    ${CODEC_SOURCE}
)

add_library(CamCodec SHARED
    ${CODEC_SOURCE}
)

target_compile_definitions(CamCodec
  PRIVATE
    -D_UNICODE
    -DUNICODE
)

target_link_libraries(CamCodec
    libminilzo
    zlibstatic
    winmm.lib
    odbc32.lib
    odbccp32.lib
)

target_include_directories(CamCodec
  PUBLIC
    .
)

set_target_properties(CamCodec PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(TARGETS CamCodec
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
  FILES
    CamCodec.inf
  DESTINATION
    bin
)
