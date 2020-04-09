#include "d2d/io/gmsh_reader.hpp"
#include "d2d/io/vtp_writer.hpp"
#include "d2d/util/clo.hpp"
#include "d2d/util/utils.hpp"

int main(int argc, char* argv[])
{
  auto optman = d2d::util::clo::manager {};
  optman.addCmlParam(d2d::util::clo::string_option
                     {"INPUT_FILE", {"--infile", "-i"},
                        "specifies the name of the input file", true});
  optman.addCmlParam(d2d::util::clo::string_option
                     {"OUTPUT_FILE", {"--outfile", "-o"},
                        "specifies the name of the output file", true});
  optman.addCmlParam(d2d::util::clo::bool_option
                     {"CONVERT_TO_DISCS", {"--convert-to-discs", "-c"},
                        "convert input to disc-based surface"});
  auto succ = optman.parse_args(argc, argv);
  if (!succ) {
    std::cout << optman.get_usage_msg();
    return EXIT_FAILURE;
  }
  auto infilename = optman.get_string_option_value("INPUT_FILE");
  auto outfilename = optman.get_string_option_value("OUTPUT_FILE");

  auto transferobject = d2d::io::gmsh_reader<double> {infilename};
  if (optman.get_bool_option_value("CONVERT_TO_DISCS")) {
    std::cout << "Writing disc-based surface to " << outfilename << std::endl;
    d2d::io::vtp_writer<double>::write_disc_surface(transferobject, outfilename);
  } else {
    std::cout << "Writing triangle mesh to " << outfilename << std::endl;
    d2d::io::vtp_writer<double>::write_triangle_surface(transferobject, outfilename);
  }
  return EXIT_SUCCESS;
}
