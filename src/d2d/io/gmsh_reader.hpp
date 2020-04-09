#pragma once

#include <algorithm>
#include <cassert>

#include <gmsh.h>

#include "d2d/util/utils.hpp"

namespace d2d { namespace io {
  template<typename numeric_type>
  class gmsh_reader {
  public:

    gmsh_reader(std::string& pFilePath):
      mMshFilePath(pFilePath) {
      gmsh::initialize();
      gmsh::option::setNumber("General.Terminal", 1);
      gmsh::open(pFilePath);
      this->mVertices = read_vertices();
      this->mTriangles = read_triangles();
      gmsh::finalize();
    }

    std::vector<d2d::util::triple<numeric_type> >
    get_vertices()
    {
      return this->mVertices;
    }

    std::vector<d2d::util::triple<std::size_t> >
    get_triangles()
    {
      return this->mTriangles;
    }

    std::string get_input_file_path()
    {
      return this->mMshFilePath;
    }

  private:
    std::string mMshFilePath;
    std::vector<d2d::util::triple<numeric_type> > mVertices;
    std::vector<d2d::util::triple<std::size_t> > mTriangles;

    std::vector<d2d::util::triple<numeric_type> >
    read_vertices()
    {
      std::vector<std::size_t> vvtags;
      std::vector<double> vvxyz;
      std::vector<double> vvuvw;
      gmsh::model::mesh::getNodes(vvtags, vvxyz, vvuvw);
      assert(vvxyz.size() == 3 * vvtags.size() && "Vertex data missmatch");

      // In Gmsh's world node tags start from 1. In our world vertex tags
      // start from 0. We adjust it here. Subtract one from each vertex
      // identifier.
      std::for_each(vvtags.begin(), vvtags.end(), [](auto &tt) {--tt;});
      // assert(*std::min_element(std::begin(vvtags), std::end(vvtags)));
      assert(*std::min_element(vvtags.begin(), vvtags.end()) == 0 &&
             "Vertex tag assumption not met");
      assert(*std::max_element(vvtags.begin(), vvtags.end()) == vvtags.size()-1
             && "Vertex tag assumption not met");

      std::vector<d2d::util::triple<numeric_type> > result(vvtags.size());
      for (size_t idx = 0; idx < vvtags.size(); ++idx) {
        size_t xyzidx = 3 * idx;
        //assert(std::is_unsigned<decltype(xyzidx)>::value); // not necessary; unsigned type
        assert(xyzidx < vvxyz.size() && "Error in reading spatial data");
        size_t vvtag = vvtags[idx];
        //assert(std::is_unsigned<decltype(vvtag)>::value); // not necessary; unsigned type
        assert(vvtag < vvtags.size() && "Error in tag/index computation");
        d2d::util::triple<numeric_type> rr
          {(numeric_type)vvxyz[xyzidx],
           (numeric_type)vvxyz[xyzidx+1],
           (numeric_type)vvxyz[xyzidx+2]};
        result[vvtag] = std::move(rr);
      }
      return result;
    }

    std::vector<d2d::util::triple<size_t> >
    read_triangles()
    {
      std::vector<int> eetypes;
      std::vector<std::vector<std::size_t> > eetags;
      std::vector<std::vector<std::size_t> > nntags;
			// element types are selected by the number of dimensions of that elements.
      // E.g., integer 2 for triangles.
      int selecttriangles = 2; // dimensions
      gmsh::model::mesh::getElements(
                                     eetypes,
                                     eetags,
                                     nntags,
                                     selecttriangles, // dimension
                                     -1); // select all elements with respect to their tag
      // When calling gmsh::getElements() with a dimension argument, then
      // the vectors eetypes, eetags and nntags are of size 1.
      assert(eetypes.size() == 1 && eetags.size() == 1 && nntags.size() == 1 &&
             "Assumptions not met");
      int selectresult = 0;
      // Sanity check
      size_t numTriangles = eetags[selectresult].size();
      assert(nntags[selectresult].size() == 3 * numTriangles &&
             "Size missmatch in triangle data");

      std::vector<std::size_t> selected = nntags[selectresult];
      // Again, like in the get_vertices function, adjust the tags of the
      // vertices to start from 0 instead of 1.
			std::for_each(selected.begin(), selected.end(), [](auto &nn) {--nn;});
      // Some sanity checks
      // Not needed because of unsigned type
      //assert(*std::min_element(selected.begin(), selected.end()) >= 0 &&
      //       "Vertex tag assumption not met");
      if (this->mVertices.size() > 0) {
        // We can verify this property only if the vertices are set in mVertices.
        assert(*std::max_element(selected.begin(), selected.end()) < this->mVertices.size() &&
               "Vertex tag assumption not met");
      }

      // Note: we do not consider the element tags (eetags) from Gmsh here.
      // That is, the tags/ids of the triangels may be different than in Gmsh.

      std::vector<d2d::util::triple<size_t> > result(numTriangles);
      for (size_t idx = 0; idx < numTriangles; ++idx) {
        size_t ntidx = 3 * idx;
        //assert(0 <= ntidx); // not needed; unsigned type
        assert(ntidx <= selected.size() && "Index out of bounds");
        d2d::util::triple<size_t> rr
          {selected[ntidx], selected[ntidx+1], selected[ntidx+2]};
        result[idx] = std::move(rr);
      }
      return result;
    }
  };
}}
