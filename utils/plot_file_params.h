#pragma once

#include <cstdint>

struct PlotFileParams
{
    PlotFileParams()
    {}

	PlotFileParams(uint64_t accountNumericId, 
		uint64_t startNonceNum, uint64_t sizeInNonce, uint64_t staggerSizeInNonces)
	{
		accountNumericId_ = accountNumericId;
		startNonceNum_ = startNonceNum;
        sizeInNonce_ = sizeInNonce;
        staggerSizeInNonces_ = staggerSizeInNonces;
	}

	uint64_t startNonceNum_ = 0, staggerSizeInNonces_ = 0, sizeInNonce_ = 0, accountNumericId_ = 0;
};