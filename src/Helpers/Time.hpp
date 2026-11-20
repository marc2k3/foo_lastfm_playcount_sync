#pragma once

struct Time
{
	static pfc::string8 FormatUnixTimestamp(uint32_t ts) noexcept
	{
		if (ts == 0u)
			return "";
		else
			return pfc::format_filetimestamp(pfc::fileTimeUtoW(ts));
	}

	static uint32_t Now() noexcept
	{
		return static_cast<uint32_t>(pfc::fileTimeWtoU(pfc::fileTimeNow()));
	}
};
