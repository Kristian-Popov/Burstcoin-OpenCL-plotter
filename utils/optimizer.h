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
        // This code is meant to be compiled in 64-bit mode only
        static_assert( sizeof( size_t ) == 8, "This code is meant to be compiled in 64-bit mode only" );

        std::string outputFilePath = outputPath; // TODO if outputPath points to a directory, add a filename
        //EXCEPTION_ASSERT( PlotFileMath::CalcStaggerCount( ExtractParams( outputFilePath ) ) == 1 ); // TODO is it useful?

        PlotFile inputFile( inputFilePath );
        const PlotFileParams& inputFileParams = inputFile.Params();
        if (inputFile.Status() != PlotFile::PossibleStatuses::Valid)
        {
            BOOST_LOG_TRIVIAL(error) << "Input file is not a valid plot file or is already fully optimized";
            return;
        }
        PlotFileParams optimizedFileParams( inputFileParams.accountNumericId_, inputFileParams.startNonceNum_, inputFileParams.sizeInNonce_, inputFileParams.sizeInNonce_ );
        PlotFile outputFile( optimizedFileParams, outputFilePath );
        if (outputFile.Status() != PlotFile::PossibleStatuses::NotPresent)
        {
            BOOST_LOG_TRIVIAL(error) << "Output file already exists, not going to overwrite it";
            return;
        }

        BOOST_LOG_TRIVIAL(info) << "Starting optimization of file " << inputFilePath << " into file " << outputFile.FileNameWithPath();

        const uint64_t availableMemoryInBytes = Utils::CalcAmountOfFreeRAMInBytes();
        EXCEPTION_ASSERT( PlotFileMath::CalcScoopRegionSizeInBytes( outputFile.Params() ) <= availableMemoryInBytes ); // Verify that entire scoop region of output file fits into RAM
        // TODO remove this limitation
        std::vector<char> memoryBuffer( availableMemoryInBytes );

        outputFile.StartCreation( PlotFile::Operation::Optimization );

        for (uint64_t scoopNum = minScoopNum; scoopNum <= maxScoopNum; scoopNum++)
        {
            BOOST_LOG_TRIVIAL(debug) << "Copying scoop #" << ScoopNumInHumanConvenientForm( scoopNum ) << " of " << ScoopNumInHumanConvenientForm( maxScoopNum );

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

        BOOST_LOG_TRIVIAL(info) << "Optimization finished";
    }
private:
    static const uint64_t minScoopNum = 0, maxScoopNum = 4095;

    static uint64_t ScoopNumInHumanConvenientForm(uint64_t scoopNum)
    {
        return scoopNum + 1;
    }
};






