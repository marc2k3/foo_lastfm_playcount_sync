#pragma once

class TrackGetScrobblesTask
{
public:
	TrackGetScrobblesTask(const MetaV2& meta, size_t page);

	bool run(lfm::Fields& f) noexcept;

private:
	static uint32_t get_timestamp(JSON& json) noexcept;

	void build_params() noexcept;

	JSON m_json;
	MetaV2 m_meta;
	cpr::Parameters m_params;
	size_t m_page{};
};
