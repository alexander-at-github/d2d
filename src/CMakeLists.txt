cmake_minimum_required (
  VERSION 3.11 FATAL_ERROR
  )
project (
  d2d
  VERSION 1.0
  LANGUAGES CXX
  )
find_package (
  VTK 8.2 REQUIRED
  PATHS ${VTK_DIR}
  NO_DEFAULT_PATH)
include(${VTK_USE_FILE}) # Necessary for CMake and VTK version < 8.90.0
# Gmsh does not provide a config of a module for find_package().
# We find the files ourselfs.
find_path(GMSH_INCLUDE_DIR NAMES gmsh.h
  PATHS "${GMSH_DIR}/include"
  NO_DEFAULT_PATH
  )
find_library(GMSH_LIBRARY NAMES gmsh libgmsh
  PATHS "${GMSH_DIR}/lib"
  NO_DEFAULT_PATH
)
# One needs to set the RPATH, otherwise the installed executable does not have
# dynamic link libraries set correctly.
# CMAKE_INSTALL_RPATH_USE_LINK_PATH needs to be set before the executables
# are added.
set (
  CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE
  )
add_executable (
  msh2vtp "d2d/msh2vtp.cpp"
  )
target_include_directories (
  msh2vtp
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${VTK_INCLUDE_DIRS}
  ${GMSH_INCLUDE_DIR}
  )
set_target_properties (
  msh2vtp
  PROPERTIES
  CXX_STANDARD 14)
target_link_libraries (
  msh2vtp
  PRIVATE
  ${VTK_LIBRARIES}
  ${GMSH_LIBRARY}
  )
# install (
#   TARGETS msh2vtp
#   RUNTIME
#   DESTINATION ${CMAKE_INSTALL_BINDIR}
#   )
add_executable (
  dsv2vtp "d2d/dsv2vtp.cpp"
  )
target_include_directories (
  dsv2vtp
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${VTK_INCLUDE_DIRS}
  ${GMSH_INCLUDE_DIR}
  )
set_target_properties (
  dsv2vtp
  PROPERTIES
  CXX_STANDARD 14
  )
target_link_libraries (
  dsv2vtp
  PRIVATE
  ${VTK_LIBRARIES}
  ${GMSH_LIBRARIES}
  )

install (
  TARGETS msh2vtp dsv2vtp
  RUNTIME
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
