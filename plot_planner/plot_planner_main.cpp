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

std::shared_ptr<PlotterInterface> PrepareCryoStubPlotter( const std::string& strategy, uint64_t staggerSizeInBytes )
{
    std::shared_ptr<CryoGPUStubPlotter> plotter = std::make_shared<CryoGPUStubPlotter>();
    plotter->SetParameters( strategy, PlotFileMath::CalcNonceCountInBytes( staggerSizeInBytes ) );
    return plotter;
}

int main( int argc, char** argv )
{
    namespace log = boost::log::trivial;
    namespace po = boost::program_options;
    const static uint64_t defaultMaxBytesVal = ULLONG_MAX;

    boost::program_options::options_description desc( "Allowed options" );
    desc.add_options()
        ( "help,h", "produce help message" )
        ( "directory,d", po::value<std::vector<std::string>>(), "directory where plot files can be found" )
        ( "id,k", po::value<uint64_t>(), "account numeric ID" )
        ( "max-bytes,b", po::value<uint64_t>()->default_value( defaultMaxBytesVal ),
            "how much bytes to fill in total in all folders. If not given, all free space will be filled" )
        ( "stagger-size,s", po::value<uint64_t>(), "stagger size in bytes (determines used RAM to a large extent)" )
        ( "strategy", po::value<std::string>(), "strategy used by Cryo's GPU miner. Valid values: buffer and direct" )
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

    std::vector<std::string> directories = vm["directory"].as<std::vector<std::string>>();
    if (directories.empty())
    {
        BOOST_LOG_TRIVIAL( fatal ) << "No directories to fill are given, exiting";
        return EXIT_FAILURE;
    }

    uint64_t accountNumericId = vm["id"].as<uint64_t>();
    uint64_t maxBytes = vm["max-bytes"].as<uint64_t>();
    uint64_t staggerSizeInBytes = vm["stagger-size"].as<uint64_t>();
    std::string strategy = vm["strategy"].as<std::string>();

    if ( maxBytes == defaultMaxBytesVal )
    {
        BOOST_LOG_TRIVIAL(info) << "Filling all available space";
    }
    else
    {
        // TODO improve to print megabytes or something
        BOOST_LOG_TRIVIAL(info) << "Filling up to " << maxBytes << " bytes of space";
    }

    std::shared_ptr<PlotterInterface> plotter = PrepareCryoStubPlotter( strategy, staggerSizeInBytes );

    try
    {
        PlotPlanner::FillSpace( plotter, accountNumericId, directories, maxBytes );
    }
    catch(std::exception& e)
    {
        BOOST_LOG_TRIVIAL( fatal ) << "Caught fatal exception: " << e.what();
        return EXIT_FAILURE;
    }
    catch(...)
    {
        BOOST_LOG_TRIVIAL( fatal ) << "Caught fatal error";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
