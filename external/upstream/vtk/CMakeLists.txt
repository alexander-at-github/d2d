externalproject_add(vtk-external
  GIT_REPOSITORY
    https://gitlab.kitware.com/vtk/vtk.git
  # Hash of tags/v8.2.0
  GIT_TAG
    e3de2c35c9f44fd6d16ad4c6b6527de7c4f677c7
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
    -DCMAKE_CXX_EXTENSIONS=${CMAKE_CXX_EXTENSIONS}
    -DCMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED}
    #-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DBUILD_TESTING=OFF
    -DBUILD_EXAMPLES=OFF
    #-DBOOST_ROOT=$ENV{HPTCAD_BOOST_INSTALL_DIR}
    -DVTK_WRAP_PYTHON=OFF
    -DVTK_PYTHON_VERSION=3.5
  BUILD_COMMAND
     make -j 4
  # No install needed
  INSTALL_COMMAND
    ""
  UPDATE_COMMAND
    ""
  USES_TERMINAL_DOWNLOAD
    1
  USES_TERMINAL_UPDATE
    1
  GIT_PROGRESS
    1
)

# Get install and build directory
externalproject_get_property(vtk-external INSTALL_DIR BINARY_DIR)

set(
  VTK_DIR "${BINARY_DIR}"
  CACHE PATH "Path to VTK installation"
  FORCE
  )

