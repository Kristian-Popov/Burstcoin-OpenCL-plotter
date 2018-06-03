#pragma once

#include <string>
#include <boost/log/trivial.hpp>

#include "plot_file.h"
#include "plot_file_math.h"
#include "utils.h"

class Optimizer
{
public:
    static void Optimize( const boost::filesystem::path& inputFilePath, const boost::filesystem::path& outputPath )
    {
        auto inputFile = std::make_shared<PlotFile>( inputFilePath );
        Optimize( inputFile, outputPath );
    }

    static void Optimize( const std::shared_ptr<PlotFile>& inputFile, const boost::filesystem::path& outputPath )
    {
        // This code is meant to be compiled in 64-bit mode only
        static_assert( sizeof( size_t ) == 8, "This code is meant to be compiled in 64-bit mode only" );

        //EXCEPTION_ASSERT( PlotFileMath::CalcStaggerCount( ExtractParams( outputPath ) ) == 1 ); // TODO is it useful?

        const PlotFileParams& inputFileParams = inputFile->Params();
        if ( inputFile->Status() != PlotFile::PossibleStatuses::Valid )
        {
            BOOST_LOG_TRIVIAL(error) << "Input file is not a valid plot file or is already fully optimized";
            return;
        }
        PlotFileParams optimizedFileParams( inputFileParams.accountNumericId_,
            inputFileParams.nonceNumRange_.StartNonceNum(),
            inputFileParams.nonceNumRange_.SizeInNonce(),
            inputFileParams.nonceNumRange_.SizeInNonce() );
        PlotFile outputFile( optimizedFileParams, outputPath );
        if (outputFile.Status() != PlotFile::PossibleStatuses::NotPresent)
        {
            BOOST_LOG_TRIVIAL(error) << "Output file already exists, not going to overwrite it";
            return;
        }

        BOOST_LOG_TRIVIAL(info) << "Starting optimization of file " << inputFile->FileNameWithPath() <<
            " into file " << outputFile.FileNameWithPath();

        const uint64_t availableMemoryInBytes = Utils::CalcAmountOfFreeRAMInBytes();
        uint64_t memoryNeededInBytes = PlotFileMath::CalcScoopRegionSizeInBytes( outputFile.Params() );
        EXCEPTION_ASSERT( memoryNeededInBytes <= availableMemoryInBytes ); // Verify that entire scoop region of output file fits into RAM
        // TODO remove this limitation
        std::vector<char> memoryBuffer( memoryNeededInBytes );

        outputFile.StartCreation( PlotFile::Operation::Optimization );

        for (uint64_t scoopNum = minScoopNum; scoopNum <= maxScoopNum; scoopNum++)
        {
            BOOST_LOG_TRIVIAL(debug) << "Copying scoop #" << ScoopNumInHumanConvenientForm( scoopNum ) << " of " << ScoopNumInHumanConvenientForm( maxScoopNum );

            uint64_t offset = 0;
            for (uint64_t staggerNum = 0; staggerNum < PlotFileMath::CalcStaggerCount( inputFileParams ); ++staggerNum)
            {
                uint64_t dataRead = inputFile->Read( staggerNum, scoopNum, memoryBuffer.data() + offset );
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






