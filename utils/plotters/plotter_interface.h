#pragma once

#include "utils/plot_file_params.h"

class PlotterInterface
{
public:
    virtual NonceNumRange Plot(
        const std::string& directory,
        const PlotFileParams& params ) = 0;
};
