#pragma once

#include "Raster.h"
#include <vector>
#include <string>
#include "geometrix.h"

struct raster_tools //just a namespace
{
    static RasterFloat integer_downsample_mean(const RasterFloat& src, int window_size);

    static RasterFloat convolution_filter(const RasterFloat& src,
        std::vector<double> kernel,
        int size);

    static RasterFloat max_filter(const RasterFloat& src, int size, double pos, double factor);

    static void flip_data_x(RasterFloat& r);

    static void flip_data_y(RasterFloat& r);

    static void find_minmax(const RasterFloat& raster, double& min, double& max);

    static BBox3D get_bounding_box3d(const RasterFloat& raster);

    static double sample_nearest_valid_avg(const RasterFloat& src,
        const unsigned int row,
        const unsigned int column,
        int min_averaging_samples = 1);
};
