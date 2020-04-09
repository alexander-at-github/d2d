#pragma once

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>
#include <vtkXMLPolyDataWriter.h>

#include "d2d/io/dsv_reader.hpp"
#include "d2d/io/gmsh_reader.hpp"

namespace d2d { namespace io {
  template<typename numeric_type>
  class vtp_writer {

  public:
    static void
    write_disc_surface
    (d2d::io::dsv_reader<numeric_type>& dsvreader,
     std::string outfilename)
    {
      auto vertices = dsvreader.get_vertices();
      auto normals = dsvreader.get_normals();
      auto radii = dsvreader.get_sqrts_of_areas();
      auto polydata = create_disc_polydata(vertices, normals, radii);
      write(polydata, outfilename);
    }

    static void
    write_disc_surface
    (d2d::io::gmsh_reader<numeric_type>& gmshreader,
     std::string outfilename)
    {
      auto vertices = gmshreader.get_vertices();
      auto triangles = gmshreader.get_triangles();
      auto normals = create_disc_normals_from_triangles(vertices, triangles);
      auto radii = create_disc_radii_from_triangles(vertices, triangles);
      auto polydata = create_disc_polydata(vertices, normals, radii);
      write(polydata, outfilename);
    }

    static void
    write_triangle_surface
    (d2d::io::gmsh_reader<numeric_type>& gmshreader,
     std::string outfilename)
    {
      auto polydata = create_triangle_polydata(gmshreader);
      write(polydata, outfilename);
    }

  private:
    static vtkSmartPointer<vtkPolyData>
    create_disc_polydata
    (std::vector<d2d::util::triple<numeric_type> >& invertices,
     std::vector<d2d::util::triple<numeric_type> >& innormals,
     std::vector<numeric_type>& inradii)
    {
      auto numpoints = invertices.size();

      auto points = vtkSmartPointer<vtkPoints>::New();
      auto cells = vtkSmartPointer<vtkCellArray>::New();
      auto normals = vtkSmartPointer<vtkDoubleArray>::New();
      normals->SetNumberOfComponents(3); // 3 dimensions
      normals->SetNumberOfTuples(numpoints);
      auto radii = vtkSmartPointer<vtkDoubleArray>::New();
      radii->SetNumberOfComponents(1); // 1 dimension
      radii->SetNumberOfTuples(numpoints);

      // insert points into vtk data structure
      for (size_t pidx = 0; pidx < numpoints; ++pidx) {
        auto point = invertices[pidx];
        auto normal = innormals[pidx];
        auto radius = inradii[pidx];
        auto writePointId = points->InsertNextPoint(point.data());
        cells->InsertNextCell(1, &writePointId); // one cell for writePointId
        //normals.push_back(normal);
        normals->SetTuple(pidx, normal.data());
        //radii.push_back(point[3]);
        radii->SetTuple(pidx, &radius);
      }
      auto polydata = vtkSmartPointer<vtkPolyData>::New();
      polydata->SetPoints(points);
      polydata->SetVerts(cells);
      polydata->GetCellData()->SetNormals(normals);
      radii->SetName(radiusStr);
      polydata->GetCellData()->AddArray(radii);

      return polydata;
    }

    static vtkSmartPointer<vtkPolyData>
    create_triangle_polydata
    (d2d::io::gmsh_reader<numeric_type>& gmshreader)
    {
      auto inpoints = gmshreader.get_vertices();
      auto intriangles = gmshreader.get_triangles();
      auto numpoints = inpoints.size();
      auto numtriangles = intriangles.size();

      auto vtkpoints = vtkSmartPointer<vtkPoints>::New();
      auto vtkcells = vtkSmartPointer<vtkCellArray>::New();

      // Handle points
      for (size_t idx = 0; idx < numpoints; ++idx) {
        auto& point = inpoints[idx];
        auto writepointID =
          vtkpoints->InsertNextPoint(point[0], point[1], point[2]);
      }
      // Handle triangles
      for (size_t idx = 0; idx < numtriangles; ++idx) {
        auto& intriangle = intriangles[idx];
        auto outtriangle = vtkSmartPointer<vtkTriangle>::New();
        outtriangle->GetPointIds()->SetId (0, intriangle[0]);
        outtriangle->GetPointIds()->SetId (1, intriangle[1]);
        outtriangle->GetPointIds()->SetId (2, intriangle[2]);
        vtkcells->InsertNextCell(outtriangle);
      }
      auto polydata = vtkSmartPointer<vtkPolyData>::New();
      polydata->SetPoints(vtkpoints);
      polydata->SetPolys(vtkcells);
      return polydata;
    }

    static void
    write(vtkSmartPointer<vtkPolyData>& polydata, std::string outfilename)
    {
      auto vtkwriter = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
      vtkwriter->SetFileName(outfilename.c_str());
      vtkwriter->SetInputData(polydata);
      vtkwriter->SetDataModeToAscii(); // human readable XML output
      vtkwriter->Write();
    }

  private:
    static std::vector<d2d::util::triple<numeric_type> >
    create_disc_normals_from_triangles
    (std::vector<d2d::util::triple<numeric_type> >& vertices,
     std::vector<d2d::util::triple<size_t> >& triangles)
    {
      auto numvertices = vertices.size();
      auto p2tmap = create_point_indices_to_set_of_triangle_indices_map
        (numvertices, triangles);
      auto normals = std::vector<d2d::util::triple<numeric_type> > (vertices.size());
      for (size_t vidx = 0; vidx < numvertices; ++vidx) {
        normals[vidx] = compute_average_normal
          (vertices, triangles, p2tmap[vidx]);
      }
      return normals;
    }

    static std::vector<numeric_type>
    create_disc_radii_from_triangles
    (std::vector<d2d::util::triple<numeric_type> >& vertices,
     std::vector<d2d::util::triple<size_t> >& triangles)
    {
      auto numvertices = vertices.size();
      auto p2tmap = create_point_indices_to_set_of_triangle_indices_map(numvertices, triangles);
      auto radii = std::vector<numeric_type> (vertices.size());
      for (size_t vidx = 0; vidx < numvertices; ++vidx) {
        radii[vidx] = compute_radius(vertices, triangles, vidx, p2tmap[vidx]);
      }
      return radii;
    }

    static d2d::util::triple<numeric_type>
    compute_average_normal
    (std::vector<d2d::util::triple<numeric_type> >& vertices,
     std::vector<d2d::util::triple<size_t> >& triangles,
     std::vector<size_t>& adjtriangles)
    {
      auto result = d2d::util::triple<numeric_type> {0, 0, 0};
      for (auto const& tidx: adjtriangles) {
        auto pidcs = triangles[tidx];
        auto tridata =
          d2d::util::triple<d2d::util::triple<numeric_type> >
          {vertices[pidcs[0]], vertices[pidcs[1]], vertices[pidcs[2]]};
        auto normal = d2d::util::compute_normal(tridata);
        result = d2d::util::sum(result, normal);
      }
      d2d::util::normalize(result);
      return result;
    }

    static numeric_type
    compute_radius
    (std::vector<d2d::util::triple<numeric_type> >& vertices,
     std::vector<d2d::util::triple<size_t> >& triangles,
     size_t pidx,
     std::vector<size_t>& adjtriangles)
    {
      auto result = (numeric_type) 0;
      for (auto const& tidx: adjtriangles) {
        auto pidcs = triangles[tidx];
        auto tridata =
          d2d::util::triple<d2d::util::triple<numeric_type> >
          {vertices[pidcs[0]], vertices[pidcs[1]], vertices[pidcs[2]]};
        auto centroid = d2d::util::get_centroid(tridata);
        auto distance = d2d::util::distance(vertices[pidx], centroid);
        if (result < distance)
          result = distance;
      }
      return result;
    }

    static std::vector<std::vector<size_t> >
    create_point_indices_to_set_of_triangle_indices_map
    (size_t numpoints, std::vector<d2d::util::triple<size_t> >& intriangles)
    {
      auto map = std::vector<std::vector<size_t> > (numpoints);
      for (size_t tidx = 0; tidx < intriangles.size(); ++tidx) {
        for (auto const& pnt : intriangles[tidx]) {
          map[pnt].push_back(tidx);
        }
      }
      return map;
    }

  private:
    static constexpr char const* radiusStr = "radius";
  };
}}
