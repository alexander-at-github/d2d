# d2d &ndash; File Format Conversion Tools:

`msh2vtp` converts Gmsh files to VTK Polydata files.
````
  Usage: ./bin/msh2vtp [options] --outfile <value> --infile <value>

  Options:
      --convert-to-discs  or  -c
         convert input to disc-based surface
      --outfile <value>  or  -o <value>
         specifies the name of the output file
      --infile <value>  or  -i <value>
         specifies the name of the input file
````

`dsv2vtp` converts delimiter-separated values files to VTK Polydata files.

````
  Usage: ./bin/dsv2vtp [options] --write <value> --infile <value>

  Options:
      --filter-covered
         turns filtering of covered points on
      --write <value>  or  --outfile <value>
         specifies the name of the output file
      --infile <value>
         spacifies the name of the input file
````

### Build Instructions

````
cd <d2d-base-directory>
mkdir build
cd build
cmake ..
cmake --build . --target d2d
````
