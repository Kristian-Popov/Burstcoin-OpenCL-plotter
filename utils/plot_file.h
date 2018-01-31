#pragma once

#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <string>

#include "plot_file_params.h"
#include "utils.h"

class PlotFile
{
public:
	enum class Status
	{
		NotPresent,
		Valid,
		Corrupted
	};

	PlotFile(const boost::filesystem::path& filePath)
	{
		if (boost::filesystem::exists( filePath ))
		{
			if (boost::filesystem::is_regular_file( filePath ))
			{
				bool longValidation = false;
				status = IsValid(filePath, longValidation) ? Status::Valid : Status::Corrupted;
			}
			else
			{
				throw std::logic_error( "PlotFile filePath constructor: filePath refers "
					"to something that exists but is not a file." );
			}
		}
		else
		{
			status = Status::NotPresent;
		}
		const PlotFileParams params = ExtractParamsFromFilePath( filePath );
		Initialize( params, filePath );
	}

    PlotFile(const PlotFileParams& params,
		const boost::filesystem::path& directory)
    {
		EXCEPTION_ASSERT( boost::filesystem::exists(directory) && 
			boost::filesystem::is_directory(directory) );
		boost::filesystem::path filePath = directory / 
			BuildFileName( params );
		Initialize( params, filePath );
    }
private:
    PlotFileParams params_;
	boost::filesystem::path& filePath_;
	Status status;
	
	/*
		Common part of object initialization. filePath MUST be a valid file path with parameters equal to supplied ones
	*/
	void Initialize(const PlotFileParams& params,
		const boost::filesystem::path& filePath)
	{
		EXCEPTION_ASSERT( sizeInNonce_ % staggerSizeInNonces_ == 0 );
        EXCEPTION_ASSERT( sizeInNonce_ >= staggerSizeInNonces_ );
		params_ = params;
		filePath_ = filePath;
	}
	
	std::string BuildFileName( const PlotFileParams& params )
	{
		return ( boost::format( "%d_%d_%d_%d" ) % params.accountNumericId_ % params.startNonceNum_ % 
			params.sizeInNonce_ % params.staggerSizeInNonces_ ).str();
	}
	
	PlotFileParams ExtractParamsFromFilePath( const boost::filesystem::path& filePath )
	{
		boost::filesystem::path fileName = filePath.filename();
		typedef boost::tokenizer<boost::char_separator<char> > 
			tokenizer;
		tokenizer tokens( fileName.string(), boost::char_separator<char>("_") );
		std::vector<std::string> tokensVector;
		std::copy( tokens.begin(), tokens.end(), std::back_inserter(tokensVector) );
		EXCEPTION_ASSERT( tokensVector.size() == 4 );
		
		PlotFileParams result;
		result.accountNumericId_ = std::stoull( tokensVector.at(0) );
		result.startNonceNum_ = std::stoull( tokensVector.at(1) );
		result.sizeInNonce_ = std::stoull( tokensVector.at(2) );
		result.staggerSizeInNonces_ = std::stoull( tokensVector.at(3) );
		return result;
	}
	
	bool IsValid(const boost::filesystem::path& filePath, bool longValidation)
	{
		EXCEPTION_ASSERT( !longValidation ); // Long validation is not implemented yet
		PlotFileParams params = ExtractParamsFromFilePath(filePath);
		bool valid = ( sizeInNonce_ % staggerSizeInNonces_ ) == 0;
        valid = valid || ( sizeInNonce_ >= staggerSizeInNonces_ );
		return valid;
	}
};