#pragma once

class TrackGetInfoTask : public fb2k::threadEntry
{
public:
	TrackGetInfoTask(const MetaV2& meta);

	void run() noexcept final;

private:
	void build_params() noexcept;

	JSON m_json;
	MetaV2 m_meta;
	cpr::Parameters m_params;
};
