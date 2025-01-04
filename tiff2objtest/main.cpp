#include <iostream>
#include <fstream>
#include <vector>
#include <gdal.h>
#include <gdal_priv.h>
#include <cpl_conv.h>
#include <ogr_spatialref.h>
#include <gdalwarper.h>
#include <gdal_alg.h>

#include "Raster.h"
#include "Mesh.h"
#include "TerraMesh.h"

struct TransformationMatrix
{
    union
    {
        double matrix[6];
        struct
        {
            double origin_x;
            double scale_x;
            double padding_0;
            double origin_y;
            double padding_1;
            double scale_y;
        };
    };
};

void flip_data_x(RasterFloat& raster)
{
    const int height = raster.get_height();
    const int width = raster.get_width();

    for (int row = 0; row < height; row++)
    {
        float* begin = raster.get_ptr(row);
        float* end = begin + width;
        std::reverse(begin, end);
    }
}

void flip_data_y(RasterFloat& raster)
{
    int row = 0;
    const int height = raster.get_height();
    const int width = raster.get_width();
    const int half_height = height / 2;

    while (row < half_height)
    {
        float* a_begin = raster.get_ptr(row);
        float* a_end = a_begin + width;
        float* b_begin = raster.get_ptr_ll(row);

        std::swap_ranges(a_begin, a_end, b_begin);
        row++;
    }
}

std::unique_ptr<Mesh> generate_tin_terra(std::unique_ptr<RasterFloat> raster, double max_error)
{
    TerraMesh g;
    g.load_raster(std::move(raster));
    g.greedy_insert(max_error);
    return g.convert_to_mesh();
}



int main() {

    const char* inputGeoTIFF = "D:\\xqb_workspace\\tiff2obj\\input.tif";
    const char* outputOBJ = "D:\\xqb_workspace\\tiff2obj\\output.obj";

    GDALAllRegister();

    GDALDataset* poDataset = (GDALDataset*)GDALOpenEx(inputGeoTIFF, GDAL_OF_RASTER, nullptr, nullptr, nullptr);
    if (poDataset == nullptr) {
        std::cerr << "Failed to open GeoTIFF file: " << inputGeoTIFF << std::endl;
        return 1;
    }

    TransformationMatrix gt;
    if (poDataset->GetGeoTransform(gt.matrix) != CPLErr::CE_None) {
		std::cerr << "Failed to read GeoTransform matrix" << std::endl;
		GDALClose(poDataset);
		return 1;
	}

    if (fabs(gt.scale_x) != fabs(gt.scale_y)) {
		std::cerr << "X and Y scale are not equal" << std::endl;
		GDALClose(poDataset);
		return 1;
	}

    int bandNum = poDataset->GetRasterCount();

    if (bandNum == 0) {
        std::cerr << "No raster band found in the GeoTIFF file" << std::endl;
        GDALClose(poDataset);
        return 1;
    }
    else if (bandNum > 1) {
		std::cerr << "Multiple raster bands found in the GeoTIFF file, using the first one" << std::endl;
        GDALClose(poDataset);
        return 1;
	}

    GDALRasterBand* raster_band = poDataset->GetRasterBand(1);

    int raster_width = raster_band->GetXSize();
    int raster_height = raster_band->GetYSize();

    auto raster = std::make_unique<RasterFloat>();

    raster->set_cell_size(fabs(gt.scale_x));
    raster->allocate(raster_width, raster_height);
    raster->set_no_data_value(raster_band->GetNoDataValue());

    if (raster_band->RasterIO(GF_Read,
        0,
        0,
        raster_width,
        raster_height,
        raster->get_ptr(),
        raster_width,
        raster_height,
        GDT_Float32,
        0,
        0) != CE_None)
    {
        std::cerr << "Failed to read raster data" << std::endl;
        return false;
    }
    double x1 = gt.origin_x;
    double y1 = gt.origin_y;
    double x2 = gt.origin_x + raster_width * gt.scale_x;
    double y2 = gt.origin_y + raster_height * gt.scale_y;

    // Ensure raster's origin is exactly at the lower left corner
    raster->set_pos_x(std::min(x1, x2));
    raster->set_pos_y(std::min(y1, y2));

    if (gt.scale_x < 0)
    {
        flip_data_x(*raster);
    }

    if (gt.scale_y > 0)
    {
        flip_data_y(*raster);
    }

    std::unique_ptr<Mesh> mesh;
    double max_error = raster->get_cell_size();

    mesh = generate_tin_terra(std::move(raster), 2.0);

    if (!mesh || mesh->empty())
    {
        std::cout << "mesh empty or null, meshing failed" << std::endl;
    }

    mesh->generate_decomposed();

    write_mesh_as_obj(outputOBJ, *mesh);

    return 0;


    //// 获取图像大小
    //int cols = poDataset->GetRasterXSize();
    //int rows = poDataset->GetRasterYSize();

    //// 读取高程数据
    //std::vector<double> elevationData(rows * cols, -9999);  // 用一维数组代替二维数组，初始化为 -9999（假设为无效值）
    //GDALRasterBand* poBand = poDataset->GetRasterBand(1);  // 假设我们使用第一个波段

    //// 获取每个栅格单元的大小
    //double adfGeoTransform[6];
    //poDataset->GetGeoTransform(adfGeoTransform);
    //double cellSize = adfGeoTransform[1];  // 获取像元大小（假设为正方形）

    //// 读取每个像元的值
    //CPLErr err = poBand->RasterIO(GF_Read, 0, 0, cols, rows, elevationData.data(), cols, rows, GDT_Float64, 0, 0);
    //if (err != CE_None) {
    //    std::cerr << "Failed to read raster data, error code: " << err << std::endl;
    //    GDALClose(poDataset);
    //    return 1;
    //}

    //// 转换为 OBJ 格式并写入
    //writeOBJ(outputOBJ, elevationData, rows, cols, cellSize);

    //// 清理
    //GDALClose(poDataset);

    //std::cout << "Conversion completed! OBJ file saved to " << outputOBJ << std::endl;
    //return 0;
}