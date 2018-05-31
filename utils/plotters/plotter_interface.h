#pragma once

#include "utils/plot_file_params.h"

enum class PlottingMode
{
    Buffer,
    Direct
};

class PlotterInterface
{
public:
    virtual NonceNumRange Plot(
        const std::string& directory,
        const PlotFileParams& params,
        PlottingMode mode
    ) = 0;
};
