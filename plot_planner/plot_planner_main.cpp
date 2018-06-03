#include <cstdlib>

#include <vector>
#include <string>
#include <iostream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>

#include <boost/filesystem.hpp>

#include "plot_planner.h"
#include "plotters/cryo_gpu_stub_plotter.h"
#include "plot_file_math.h"

uint64_t CalcMaxStaggerSizeInNonces( uint64_t staggerSizeInBytes )
{
    uint64_t staggerSizeInNonces = PlotFileMath::CalcNonceCountInBytes( staggerSizeInBytes );
    if ( staggerSizeInBytes == 0 || staggerSizeInNonces == 0  )
    {
        BOOST_LOG_TRIVIAL( fatal ) << "Stagger size is either 0 or too small to host at least one nonce, it should be at least " <<
            PlotFileMath::GetNonceSizeInBytes() <<
            " bytes, but preferrably 70-80% of available RAM";
        throw std::logic_error( "Supplied stagger size is too small" );
    }
    else
    {
        BOOST_LOG_TRIVIAL( debug ) << "Stagger size is  " << staggerSizeInNonces << " nonces";
    }
    return staggerSizeInNonces;
}

std::shared_ptr<PlotterInterface> PrepareCryoStubPlotter()
{
    std::shared_ptr<CryoGPUStubPlotter> plotter = std::make_shared<CryoGPUStubPlotter>();
    return plotter;
}

int main( int argc, char** argv )
{
    namespace log = boost::log::trivial;
    namespace po = boost::program_options;
    const static uint64_t defaultMaxBytesVal = ULLONG_MAX;

    boost::program_options::options_description desc( "Allowed options" );

    uint64_t maxBytes = 0;
    std::string tempDirectory;

    desc.add_options()
        ( "help,h", "produce help message" )
        ( "directory,d", po::value<std::vector<std::string>>()->composing(), "directory where plot files can be found" )
        ( "id,k", po::value<uint64_t>(), "account numeric ID" )
        ( "max-bytes,b", po::value<uint64_t>( &maxBytes )->default_value( defaultMaxBytesVal ),
            "how much bytes to fill in total in all folders. If not given, all free space will be filled" )
        ( "stagger-size,s", po::value<uint64_t>(), "stagger size in bytes (determines used RAM to a large extent)" )
        ( "force-direct-mode", "force direct plotting mode (as in Cryo's GPU miner). Causes a lot "
        " of IO operations on disk, but doesn't require temporary storage and optimization." )
        ( "temp-dir,t", po::value<std::string>( &tempDirectory ), "temporary storage for unoptimized plots. "
        "Ignored if direct mode is forced" )
        ( "verbose,v", "write debug output. If given, much more output is written" )
        ;

    po::positional_options_description p;
    p.add("directory", -1);

    po::variables_map vm;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    }
    catch( boost::program_options::invalid_command_line_syntax& e )
    {
        BOOST_LOG_TRIVIAL( fatal ) << "Wrong command line arguments: " << e.what();
        std::cout << desc;
        return EXIT_FAILURE;
    }
    catch (std::exception& e)
    {
        BOOST_LOG_TRIVIAL( fatal ) << "Caught exception when parsing command line arguments: " << e.what();
        std::cout << desc;
        return EXIT_FAILURE;
    }
    boost::program_options::notify( vm );

    if( vm.count( "help" ) )
    {
        std::cout << desc;
        return EXIT_FAILURE;
    }

    boost::log::core::get()->set_filter
    (
        log::severity >= ( vm.count( "verbose" ) > 0 ? log::severity_level::trace : log::severity_level::info )
    );

    if( vm.count( "directory" ) == 0 )
    {
        BOOST_LOG_TRIVIAL( fatal ) << "No directories to fill are given, exiting";
        return EXIT_FAILURE;
    }
    std::vector<std::string> directories = vm["directory"].as<std::vector<std::string>>();
    EXCEPTION_ASSERT( !directories.empty() );

    if ( maxBytes == defaultMaxBytesVal )
    {
        BOOST_LOG_TRIVIAL(info) << "Filling all available space";
    }
    else
    {
        // TODO improve to print megabytes or something
        BOOST_LOG_TRIVIAL(info) << "Filling up to " << maxBytes << " bytes of space";
    }

    if( vm.count( "id" ) != 1 )
    {
        BOOST_LOG_TRIVIAL( fatal ) << "id parameter is missing or duplicated";
        std::cout << desc;
        return EXIT_FAILURE;
    }
    if( vm.count( "stagger-size" ) != 1 )
    {
        BOOST_LOG_TRIVIAL( fatal ) << "stagger-size parameter is missing or duplicated";
        std::cout << desc;
        return EXIT_FAILURE;
    }

    uint64_t accountNumericId = vm["id"].as<uint64_t>();
    uint64_t staggerSizeInBytes = vm["stagger-size"].as<uint64_t>();
    bool forceDirectMode = vm.count( "force-direct-mode" ) > 0;

    if ( !forceDirectMode && ( vm.count( "temp-dir" ) != 1 ) )
    {
        BOOST_LOG_TRIVIAL( fatal ) << "temp-dir parameter is missing or duplicated";
        std::cout << desc;
        return EXIT_FAILURE;
    }

    std::shared_ptr<PlotterInterface> plotter = PrepareCryoStubPlotter();

    try
    {
        PlotPlanner::FillSpace( plotter, accountNumericId, directories, CalcMaxStaggerSizeInNonces( staggerSizeInBytes ),
            boost::filesystem::canonical( tempDirectory ),
            forceDirectMode, maxBytes );
    }
    catch(std::exception& e)
    {
        BOOST_LOG_TRIVIAL( fatal ) << "Caught fatal exception: " << e.what();
        throw; // Rethrow error to simplify debugging
    }
    catch(...)
    {
        BOOST_LOG_TRIVIAL( fatal ) << "Caught fatal error";
        throw; // Rethrow error to simplify debugging
    }

    return EXIT_SUCCESS;
}
