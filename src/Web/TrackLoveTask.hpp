#pragma once

class TrackLoveTask
{
public:
	TrackLoveTask(std::string_view method, const MetaV2& meta);

	bool run(lfm::Fields& f) noexcept;

private:
	void build_payload() noexcept;

	MetaV2 m_meta;
	cpr::Payload m_payload{};
	std::string m_method;
};
