#pragma once

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

#include "d2d/util/clo.hpp"
#include "d2d/util/utils.hpp"

namespace d2d { namespace io {
  template<typename numeric_type>
  class dsv_reader {
  public:

    dsv_reader(std::string infilename, bool filtercovered) :
      infilename(infilename),
      filtercovered(filtercovered) {
      readfile();
    }

    std::string
    get_input_file_path()
    {
      return infilename;
    }

    std::vector<d2d::util::triple<numeric_type> >
    get_vertices()
    {
      return vertices;
    }

    std::vector<d2d::util::triple<numeric_type> >
    get_normals()
    {
      return normals;
    }

    std::vector<numeric_type>
    get_areas()
    {
      return areas;
    }

    std::vector<numeric_type>
    get_sqrts_of_areas()
    {
      return d2d::util::foldl<std::vector<numeric_type>, numeric_type>
        ([](auto& acc, auto const& ee) {acc.push_back(std::sqrt(ee));
          return acc;},
          std::vector<numeric_type>{},
          areas);
    }

  private:
    void readfile()
    {
      auto lefttrim =
        [](auto str){
          str.erase(str.begin(),
                    std::find_if(str.begin(), str.end(),
                                 [](int ch){return !std::isspace(ch);}));};
      std::ifstream filestream(infilename.c_str());
      std::string line;
      while (std::getline(filestream, line)) {
        lefttrim(line);
        if (line[0] == '#') {
          // The string in the variable line represents a comment
          continue;
        }
        std::stringstream linestream;
        linestream << line;
        double xx, yy, zz;
        double nx, ny, nz;
        int32_t mid;
        double area;
        int32_t cover;
        linestream >> xx >> yy >> zz >> nx >> ny
                   >> nz >> mid >> area >> cover;
        if (filtercovered && cover != 0) {
          // Skip that point. It is covered by another point on a finer level
          continue;
        }
        vertices.push_back({xx, yy, zz});
        normals.push_back({nx, ny, nz});
        matIds.push_back(mid);
        areas.push_back(area);
        coverflags.push_back(cover);
      }
      filestream.close();
    }

  private:
    std::string infilename;
    bool filtercovered;
    std::vector<d2d::util::triple<double> > vertices;
    std::vector<d2d::util::triple<double> > normals;
    std::vector<int32_t> matIds;
    std::vector<double> areas;
    std::vector<int32_t> coverflags;
  };
}}
