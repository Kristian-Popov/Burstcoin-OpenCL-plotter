#include <cstdlib>

#include <vector>
#include <string>
#include <iostream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>

#include <boost/filesystem.hpp>

#include "overlap_checker.h"

const char* const description = R"(
Burstcoin overlap checker,
a simple tool to check if you have overlapping nonces.

Scans a given set of folders for a plot file so you don't have to select them manually.

Takes a list of directories (either -d, --directory command line parameters or without any key).

Example:
./burstcoin-overlap-checker /burstcoin-folder-1 /burstcoin-folder-2 /burstcoin-folder-3
or
./burstcoin-overlap-checker -d /home/burstcoin-plots
or
./burstcoin-overlap-checker --directory /home/burstcoin-plots
)";

int main( int argc, char** argv )
{
    namespace log = boost::log::trivial;
    namespace po = boost::program_options;

    boost::program_options::options_description desc( description );
    desc.add_options()
        ( "help,h", "produce help message" )
        ( "directory,d", po::value<std::vector<std::string>>()->composing(), "directory where plot files can be found" )
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
        BOOST_LOG_TRIVIAL( fatal ) << "No directories to search are given, exiting";
        return EXIT_FAILURE;
    }
    std::vector<std::string> directories = vm["directory"].as<std::vector<std::string>>();

    try
    {
        OverlapChecker::CheckForNonceOverlaps( directories );
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
