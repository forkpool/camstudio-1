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

add_subdirectory(zlib)
add_subdirectory(libpng)
add_subdirectory(minilzo)
add_subdirectory(CxImage)
add_subdirectory(localization)
add_subdirectory(fmt)

set(BUILD_EXAMPLES OFF CACHE BOOL "Disable libconfig examples" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "Disable libconfig examples" FORCE)
add_subdirectory(libconfig)

set_target_properties(libconfig libconfig++
  PROPERTIES
    FOLDER "External/libconfig"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(fmt
  PROPERTIES
    FOLDER "External/fmt"
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/bin/
    DESTINATION bin
)