#pragma once

#include "../RegViewPage.h"
#include "Types.h"
#include "ee/GIF.h"
#include "gs/GSHandler.h"

class CGifPacketView : public CRegViewPage
{
public:
	CGifPacketView(HWND, const RECT&);
	virtual ~CGifPacketView() = default;

	void SetPacket(const uint8*, uint32, uint32);

private:
	struct WriteInfo
	{
		WriteInfo(uint32 srcAddress, uint8 reg, uint64 data)
		    : srcAddress(srcAddress)
		    , write(reg, data)
		{
		}

		uint32 srcAddress = 0;
		CGSHandler::RegisterWrite write;
	};
	typedef std::vector<WriteInfo> WriteInfoList;

	void DumpPacked(const uint8*, uint32&, const CGIF::TAG&, WriteInfoList&);
};
