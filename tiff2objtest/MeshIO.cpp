#define FMT_HEADER_ONLY
#include "MeshIO.h"
#include "File.h"

#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ostream.h"
#include <iostream>
#include <fstream>
#include <set>
#include <iterator>

bool write_mesh_as_obj(const char* filename, const Mesh& m)
{
    File f;
    if (!f.open(filename, File::OM_RWCF))
    {
        return false;
    }
    return write_mesh_as_obj(f, m);
}

bool write_mesh_as_obj(FileLike& out_file, const Mesh& m)
{
    if (!m.has_decomposed())
    {
        std::cout << "mesh is not in decomposed format, please decompose first" << std::endl;
        //TNTN_LOG_ERROR("mesh is not in decomposed format, please decompose first");
        return false;
    }

    auto faces_range = m.faces();
    auto vertices_range = m.vertices();

    fmt::memory_buffer line_buffer;
    line_buffer.reserve(128);

    File::position_type write_pos = 0;
    for (const Vertex* v = vertices_range.begin; v != vertices_range.end; v++)
    {
        line_buffer.resize(0);
        fmt::format_to(std::back_inserter(line_buffer), "v {:.18f} {:.18f} {:.18f}\n", v->x, v->y, v->z);
        if (!out_file.write(write_pos, line_buffer.data(), line_buffer.size()))
        {
            return false;
        }
        write_pos += line_buffer.size();
    }

    for (const Face* f = faces_range.begin; f != faces_range.end; f++)
    {
        line_buffer.resize(0);
        fmt::format_to(std::back_inserter(line_buffer), "f {} {} {}\n", (*f)[0] + 1, (*f)[1] + 1, (*f)[2] + 1);
        if (!out_file.write(write_pos, line_buffer.data(), line_buffer.size()))
        {
            return false;
        }
        write_pos += line_buffer.size();
    }

    return out_file.is_good();
}
