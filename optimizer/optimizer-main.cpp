#include <cstdlib>

#include <vector>
#include <string>
#include <iostream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>

#include <boost/filesystem.hpp>

#include "optimizer.h"

const char* const description = R"(
Burstcoin optimizer,
a tool to optimize Burstcoin plot files.

Files to optimize are given using -i, --input-file command line parameters or without any.

Optimized files by default are put into the same folder as the current input file,
can be overriden by a -o or --output-dir directories.

Example:
./burstcoin-optimizer folder1/0_0_1000_10 folder2/2000_0_1000_10
or
./burstcoin-optimizer folder1/0_0_1000_10 folder2/2000_0_1000_10 -o optimized-files/
)";

void CreateOutputDirIfNeeded( std::string& outputDir )
{
    if ( boost::filesystem::exists( outputDir ) )
    {
        if ( !boost::filesystem::is_directory( outputDir ) )
        {
            throw std::runtime_error( "Something that is not a directory occupies the same path as output directory." );
        }
    }
    else
    {
        EXCEPTION_ASSERT( boost::filesystem::create_directory( boost::filesystem::path( outputDir ) ) );
    }
}

int main( int argc, char** argv )
{
    namespace log = boost::log::trivial;
    namespace po = boost::program_options;

    boost::program_options::options_description desc( description );
    desc.add_options()
        ( "help,h", "produce help message" )
        ( "input-file,i", po::value<std::vector<std::string>>(), "optimize the specified input files, can be given multiple times" )
        ( "output-dir,o", po::value<std::string>()->default_value( "" ), "write results to specified folder, if doesn't exist will be createc" )
        ( "verbose,v", "write debug output. If given, much more output is written" )
        ;

    po::positional_options_description p;
    p.add("input-file", -1);

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

    std::vector<std::string> inputFiles = vm["input-file"].as<std::vector<std::string>>();
    if (inputFiles.empty())
    {
        BOOST_LOG_TRIVIAL( fatal ) << "No input files are given, exiting";
        return EXIT_FAILURE;
    }
    std::string outputDir = vm["output-dir"].as<std::string>();
    if (outputDir.empty())
    {
        outputDir = boost::filesystem::current_path().string();
    }

    try
    {
        CreateOutputDirIfNeeded( outputDir );
        
        for (const std::string& inputFilePath: inputFiles)
        {
            Optimizer::Optimize(
                boost::filesystem::canonical( inputFilePath ),
                boost::filesystem::canonical( outputDir )
            );
        }
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
