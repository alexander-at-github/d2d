#include "d2d/io/dsv_reader.hpp"
#include "d2d/io/vtp_writer.hpp"

int main(int argc, char* argv[]) {

  auto optman = d2d::util::clo::manager {};
  optman.addCmlParam(d2d::util::clo::bool_option
    {"FILTER_COVERED", {"--filter-covered"},
       "turns filtering of covered points on"});
  // optman.addCmlParam(d2d::util::clo::bool_option
  //   {"RENDER", {"--render"},
  //      "render the input on GUI"});
  optman.addCmlParam(d2d::util::clo::string_option
    {"INPUT_FILE", {"--infile"},
       "spacifies the name of the input file", true});
  optman.addCmlParam(d2d::util::clo::string_option
    {"OUTPUT_FILE", {"--write", "--outfile"},
       "specifies the name of the output file", true});
  bool succ = optman.parse_args(argc, argv);
  if (!succ) {
    std::cout << optman.get_usage_msg();
    return EXIT_FAILURE;
  }
  std::string infilename = optman.get_string_option_value("INPUT_FILE");
  std::string outfilename = optman.get_string_option_value("OUTPUT_FILE");
  bool filtercovered = optman.get_bool_option_value("FILTER_COVERED");
  // bool render = optman.get_bool_option_value("RENDER");

  if (filtercovered) {
    std::cout
      << "Filtering points with cover flag set to a value not equal zero."
      << std::endl;
  }

  auto transferobject = d2d::io::dsv_reader<double> {infilename, filtercovered};
  std::cout << "Writing surface to " << outfilename << std::endl;
  d2d::io::vtp_writer<double>::write_disc_surface(transferobject, outfilename);
}
