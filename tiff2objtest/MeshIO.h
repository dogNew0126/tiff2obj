#pragma once

#include "Mesh.h"
//#include "FileFormat.h"
#include "File.h"
#include <memory>

//bool write_mesh_to_file(const char* filename, const Mesh& m, const FileFormat& f);

//std::unique_ptr<Mesh> load_mesh_from_obj(const char* filename);
bool write_mesh_as_obj(const char* filename, const Mesh& m);
bool write_mesh_as_obj(FileLike& f, const Mesh& m);

//std::unique_ptr<Mesh> load_mesh_from_off(const char* filename);
//std::unique_ptr<Mesh> load_mesh_from_off(FileLike& f);
//
//bool write_mesh_as_off(FileLike& f, const Mesh& m);
//bool write_mesh_as_off(const char* filename, const Mesh& m);
//
//bool write_mesh_as_geojson(const char* filename, const Mesh& m);
//bool write_mesh_as_geojson(FileLike& out_file, const Mesh& m);