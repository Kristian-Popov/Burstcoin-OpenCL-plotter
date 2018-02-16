#pragma once

#include <string>
#include <boost/log/trivial.hpp>

#include "plot_file.h"
#include "plot_file_math.h"
#include "utils.h"

class Optimizer
{
public:
    static void Optimize(const std::string& inputFilePath, const std::string& outputPath)
    {
        std::string outputFilePath = outputPath; // TODO if outputPath points to a directory, add a filename
        //EXCEPTION_ASSERT( PlotFileMath::CalcStaggerCount( ExtractParams( outputFilePath ) ) == 1 ); // TODO is it useful?

        BOOST_LOG_TRIVIAL(info) << "Starting optimization of file " << inputFilePath << " into file " << outputFilePath;

        PlotFile inputFile( inputFilePath ), outputFile( outputFilePath );

        const uint64_t availableMemoryInBytes = 8 * 1024 * 1024 * 1024; // 8 GiB
        EXCEPTION_ASSERT( Utils::CalcAmountOfFreeRAMInBytes() >= availableMemoryInBytes );
        EXCEPTION_ASSERT( PlotFileMath::CalcScoopRegionSizeInBytes( outputFile.Params() ) <= availableMemoryInBytes ); // Verify that entire scoop region of output file fits into RAM
        // TODO remove this limitation
        std::vector<char> memoryBuffer( availableMemoryInBytes );

        outputFile.StartCreation( PlotFile::Operation::Optimization );

        for (uint64_t scoopNum = minScoopNum; scoopNum <= maxScoopNum; scoopNum++)
        {
            BOOST_LOG_TRIVIAL(info) << "Copying scoop #" << ScoopNumInHumanConvenientForm( scoopNum ) << " of " << ScoopNumInHumanConvenientForm( maxScoopNum );

            uint64_t offset = 0;
            for (uint64_t staggerNum = 0; staggerNum < PlotFileMath::CalcStaggerCount(inputFile.Params()); ++staggerNum)
            {
                 uint64_t dataRead = inputFile.Read( staggerNum, scoopNum, memoryBuffer.data() + offset );
                EXCEPTION_ASSERT( 0 != dataRead );
                offset += dataRead;
            }
            outputFile.Write( 0, scoopNum, memoryBuffer.data() );
        }
        BOOST_LOG_TRIVIAL(info) << "Renaming file";

        outputFile.FinishCreation();
        // TODO rename file to exclude suffix

        BOOST_LOG_TRIVIAL(info) << "Optimization finished";
    }
private:
    static const uint64_t minScoopNum = 0, maxScoopNum = 4095;

    static uint64_t ScoopNumInHumanConvenientForm(uint64_t scoopNum)
    {
        return scoopNum + 1;
    }
};






