#pragma once

#include <boost/filesystem.hpp>

#include <string>
#include <fstream>

#include "plot_file_params.h"
#include "utils.h"

// TODO this class could be greatly implemented using "state" pattern
class PlotFile
{
public:
    enum class PossibleStatuses
    {
        NotPresent,
        Valid,
        Corrupted,
        OptimizingInProgress
    };

    enum class Operation
    {
        Optimization
    };

    PlotFile( const boost::filesystem::path& filePath );

    PlotFile(const PlotFileParams& params,
        const boost::filesystem::path& directory );

    // Perform operations required before file building like file creation and space allocation
    void StartCreation( Operation op );

    // Finish file creation process
    // Currently renames file to an appropriate form
    void FinishCreation();

    /*
        Read data from file to buffer "data"
        Returns count of read bytes
    */
    uint64_t Read(uint64_t staggerNum, uint64_t scoopNum, char* data);

    /*
        Write data from buffer "data" to file
        Returns count of written bytes
    */
    void Write(uint64_t staggerNum, uint64_t scoopNum, const char* data);

    const PlotFileParams& Params()
    {
        return params_;
    }
    
    std::string FileNameWithPath()
    {
		return BuildFilePathWithSuffix();
	}
    
    PossibleStatuses Status() const
    {
        return status_;
    }
private:
    static constexpr const char* optimizationSuffix_ = ".optimizing";
    PlotFileParams params_;
    boost::filesystem::path filePathWithoutSuffix_;
    PossibleStatuses status_;
    std::fstream stream_;

    /*
        Common part of object initialization
    */
    void Initialize(const boost::filesystem::path& filePath );

    std::string BuildFileNameWithoutSuffix();

    std::string BuildFilePathWithSuffix( const std::string& suffix = "" );

    PlotFileParams ExtractParamsFromFilePath( const boost::filesystem::path& filePath );

    bool IsValid( const boost::filesystem::path& filePath, bool longValidation );
};
