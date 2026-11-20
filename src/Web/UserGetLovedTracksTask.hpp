#pragma once

class UserGetLovedTracksTask : public fb2k::threadEntry
{
public:
	UserGetLovedTracksTask(const lfm::Set& hashes, size_t page);

	void run() noexcept final;

private:
	void build_params() noexcept;

	cpr::Parameters m_params;
	lfm::Set m_hashes;
	size_t m_page{};
};
