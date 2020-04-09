## external project base directory for all projects in this hierarchy
set_property(DIRECTORY PROPERTY EP_BASE ${M_DEPENDENCIES_DIR})
add_subdirectory(oce)
add_subdirectory(gmsh)
add_subdirectory(vtk)
