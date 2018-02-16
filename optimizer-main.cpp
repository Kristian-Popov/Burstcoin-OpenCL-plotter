#include <cstdlib>

#include <vector>
#include <string>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>

#include <boost/filesystem.hpp>

#include "optimizer.h"

int main( int argc, char** argv )
{
    namespace log = boost::log::trivial;
    namespace po = boost::program_options;

    boost::program_options::options_description desc( "Allowed options" );
    desc.add_options()
#if 0
        ( "help,h", "produce help message" )
        ( "input-file,i", po::value<std::vector<std::string>>, "optimize the specified input files, can be given multiple times" )
        ( "output-dir,o", po::value<std::string>()->default_value( "" ), "write results to specified folder" )
#endif
        ("help", "produce help message")
        ("compression", po::value<int>(), "set compression level")
        ;

    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    try
    {
        po::store(po::command_line_parser(argc, argv).
              options(desc).positional(p).run(), vm);
    }
    catch( boost::program_options::invalid_command_line_syntax& e )
    {
        BOOST_LOG_TRIVIAL( fatal ) << "Wrong command line arguments: " << e.what();
        BOOST_LOG_TRIVIAL( fatal ) << desc;
        return EXIT_FAILURE;
    }
    catch (std::exception& e)
    {
        BOOST_LOG_TRIVIAL( fatal ) << "Caught exception when parsing command line arguments: " << e.what();
        BOOST_LOG_TRIVIAL( fatal ) << desc;
        return EXIT_FAILURE;
    }
    boost::program_options::notify( vm );

    if( vm.count( "help" ) )
    {
        BOOST_LOG_TRIVIAL( fatal ) << desc;
        return EXIT_FAILURE;
    }

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
        for (const std::string& inputFilePath: inputFiles)
        {
            Optimizer::Optimize( inputFilePath, outputDir );
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
