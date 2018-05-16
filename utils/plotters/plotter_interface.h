#pragma once

class PlotterInterface
{
public:
    virtual void Plot( const std::string& directory, uint64_t accountNumericId, const NonceNumRange& range ) = 0;
};
