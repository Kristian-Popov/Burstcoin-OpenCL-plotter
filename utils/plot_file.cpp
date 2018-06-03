#include "plot_file.h"
#include "plot_file_math.h"

#include <regex>

#include <boost/tokenizer.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

PlotFile::PlotFile( const boost::filesystem::path & filePath )
{
    Initialize( filePath );
}

PlotFile::PlotFile( const PlotFileParams & params, const boost::filesystem::path & directory )
{
    EXCEPTION_ASSERT( boost::filesystem::exists( directory ) &&
        boost::filesystem::is_directory( directory ) );
    EXCEPTION_ASSERT( params.staggerSizeInNonces_ > 0 );
    params_ = params;
    boost::filesystem::path filePath = directory /
        BuildFileNameWithoutSuffix();

    Initialize( filePath );
}

void PlotFile::StartCreation( Operation op )
{
    EXCEPTION_ASSERT( op == Operation::Optimization ); // Remove this when more operations are available
    EXCEPTION_ASSERT( status_ == PossibleStatuses::NotPresent );
    std::string filePath = BuildFilePathWithSuffix( optimizationSuffix_ );
    status_ = PossibleStatuses::OptimizingInProgress;
    
    // Open file and close it (basically create it)
    stream_.open( filePath, std::ios_base::out | std::ios_base::binary );
    stream_.close();
    
    // Reserve space
    boost::filesystem::resize_file( boost::filesystem::path( filePath ), 
        PlotFileMath::CalcPlotFileSize( params_ ) );
        
    // Open it again
    stream_.open( filePath, std::ios_base::out | std::ios_base::binary );
}

void PlotFile::FinishCreation()
{
    stream_.close();
    
    boost::filesystem::path oldFilePath = BuildFilePathWithSuffix( optimizationSuffix_ );
    boost::filesystem::path newFilePath = BuildFilePathWithSuffix();
    boost::filesystem::rename( oldFilePath, newFilePath );

    bool longValidation = false;
    if ( IsValid( newFilePath, longValidation ) )
    {
        status_ = PossibleStatuses::Valid;
    }
    else
    {
        throw std::logic_error( "Plot file building failed" );
    }
}

uint64_t PlotFile::Read(uint64_t staggerNum, uint64_t scoopNum, char * data)
{
    EXCEPTION_ASSERT(nullptr != data);
    EXCEPTION_ASSERT(PossibleStatuses::Valid == status_);
    uint64_t expectedSize = PlotFileMath::CalcScoopRegionSizeInBytes(params_);
    stream_.seekg( PlotFileMath::CalcScoopStartOffsetInBytes( params_, staggerNum, scoopNum ) );
    stream_.read(data, expectedSize);
    uint64_t bytesRead = stream_.gcount();
    EXCEPTION_ASSERT(expectedSize == bytesRead);
    return bytesRead;
}

void PlotFile::Write(uint64_t staggerNum, uint64_t scoopNum, const char * data)
{
    EXCEPTION_ASSERT(nullptr != data);
    uint64_t expectedSize = PlotFileMath::CalcScoopRegionSizeInBytes(params_);
    stream_.seekg( PlotFileMath::CalcScoopStartOffsetInBytes( params_, staggerNum, scoopNum ) );
    stream_.write(data, expectedSize);
}

/*
Common part of object initialization
*/
inline void PlotFile::Initialize( const boost::filesystem::path & filePath )
{
    if( boost::filesystem::exists( filePath ) )
    {
        if( boost::filesystem::is_regular_file( filePath ) )
        {
            bool longValidation = false;
            status_ = IsValid( filePath, longValidation ) ? PossibleStatuses::Valid : PossibleStatuses::Corrupted;
        }
        else
        {
            throw std::logic_error( "PlotFile filePath constructor: filePath " +
                filePath.string() + " refers "
                "to something that exists but is not a file." );
        }
    }
    else
    {
        status_ = PossibleStatuses::NotPresent;
    }
    PlotFileParams params = ExtractParamsFromFilePath( filePath );
    EXCEPTION_ASSERT( params.nonceNumRange_.SizeInNonce() % params.staggerSizeInNonces_ == 0 );
    EXCEPTION_ASSERT( params.nonceNumRange_.SizeInNonce() >= params.staggerSizeInNonces_ );
    params_ = params;
    filePathWithoutSuffix_ = filePath;
    stream_.exceptions( std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit );
    if (status_ == PossibleStatuses::Valid)
    {
        stream_.open( BuildFilePathWithSuffix(), std::ios_base::in | std::ios_base::binary );
    }
}

inline std::string PlotFile::BuildFileNameWithoutSuffix()
{
    return BuildPlotFileNameForParams( params_ );
}

std::string PlotFile::BuildFilePathWithSuffix( const std::string& suffix /* = "" */ ) const
{
    // TODO use BuildCanonicalFilePath()?
    return filePathWithoutSuffix_.string() + suffix;
}

inline PlotFileParams PlotFile::ExtractParamsFromFilePath( const boost::filesystem::path & filePath )
{
    EXCEPTION_ASSERT( IsNameValid( filePath ) );
    boost::filesystem::path fileName = filePath.filename();
    typedef boost::tokenizer<boost::char_separator<char> >
        tokenizer;
    tokenizer tokens( fileName.string(), boost::char_separator<char>( "_" ) );
    std::vector<std::string> tokensVector;
    std::copy( tokens.begin(), tokens.end(), std::back_inserter( tokensVector ) );
    EXCEPTION_ASSERT( tokensVector.size() == 4 );

    uint64_t accountNumericId = std::stoull( tokensVector.at( 0 ) );
    uint64_t startNonceNum = std::stoull( tokensVector.at( 1 ) );
    uint64_t sizeInNonce = std::stoull( tokensVector.at( 2 ) );
    uint64_t staggerSizeInNonces = std::stoull( tokensVector.at( 3 ) );
    return PlotFileParams( accountNumericId, NonceNumRange( startNonceNum, sizeInNonce ), staggerSizeInNonces );
}

inline bool PlotFile::IsValid( const boost::filesystem::path & filePath, bool longValidation )
{
    EXCEPTION_ASSERT( !longValidation ); // Long validation is not implemented yet
    PlotFileParams params = ExtractParamsFromFilePath( filePath );
    bool valid = ( params.nonceNumRange_.SizeInNonce() % params.staggerSizeInNonces_ ) == 0;
    valid = valid || ( params.nonceNumRange_.SizeInNonce() >= params.staggerSizeInNonces_ );
    return valid;
}

bool PlotFile::IsNameValid( const boost::filesystem::path& filePath )
{
    /*
     * Name is considered valid if:
     * it has all four numbers separated by a single underscore,
     * file name may have suffix but it has the following limitation:
     *    if suffix is present, it consists of a dot plus at least one letter
     *    (other symbols are not allowed)
     *    cannot have just a dot
     */
    std::string fileName = filePath.filename().string();
    std::regex regex( R"(\d+_\d+_\d+_\d+(\.[a-zA-Z]+)?)" );
    return std::regex_match( fileName, regex );
}

bool PlotFile::operator==( const PlotFile& rhs )
{
    return this->BuildCanonicalFilePath() == rhs.BuildCanonicalFilePath();
}

std::string PlotFile::BuildCanonicalFilePath() const
{
    return boost::filesystem::canonical( filePathWithoutSuffix_ ).string();
}

bool PlotFile::operator!=( const PlotFile& rhs )
{
    return !( *this == rhs );
}

bool PlotFile::operator<( const PlotFile& rhs )
{
    return BuildCanonicalFilePath() < rhs.BuildCanonicalFilePath();
}

// TODO test it somehow?
bool PlotFile::DoesItBelongToDirectory( const boost::filesystem::path& directory ) const
{
    return !boost::filesystem::relative( FileNameWithPath(), boost::filesystem::canonical( directory ) ).empty();
}

bool PlotFile::IsFullyOptimized() const
{
    return params_.nonceNumRange_.SizeInNonce() == params_.staggerSizeInNonces_;
}

std::string PlotFile::BuildPlotFileNameForParams( const PlotFileParams& params )
{
    return ( boost::format( "%d_%d_%d_%d" ) %
        params.accountNumericId_ %
        params.nonceNumRange_.StartNonceNum() %
        params.nonceNumRange_.SizeInNonce() %
        params.staggerSizeInNonces_ ).str();
}
