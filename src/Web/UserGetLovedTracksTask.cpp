#include "PCH.hpp"
#include "UserGetLovedTracksTask.hpp"

UserGetLovedTracksTask::UserGetLovedTracksTask(const lfm::Set& hashes, size_t page)
	: m_hashes(hashes)
	, m_page(page) {}

void UserGetLovedTracksTask::build_params() noexcept
{
	m_params.Add({ "api_key" , Component::api_key.data() });
	m_params.Add({ "format", "json" });
	m_params.Add({ "method", "user.getLovedTracks" });
	m_params.Add({ "username", Config::get_user_name() });
	m_params.Add({ "page", fmt::to_string(m_page) });
	m_params.Add({ "limit", "200" });
}

void UserGetLovedTracksTask::run() noexcept
{
	build_params();

	Request request;
	lfm::Totals t{};
	auto result = request.GET(m_params);

	if (!result)
	{
		Message::log(result.error());
		return;
	}

	auto& lovedtracks = result.value()["lovedtracks"];

	if (!lovedtracks.is_object())
		return;

	if (!lfm::get_totals(lovedtracks, t))
		return;

	auto& tracks = lovedtracks["track"];

	if (!tracks.is_array())
		return;

	lfm::List to_refresh;
	auto& hm = HashManager::get();
	auto transaction_ptr = HashManager::api()->begin_transaction();

	for (auto&& track : tracks)
	{
		std::string artist, title;
		auto& artist_obj = track["artist"];

		if (artist_obj.is_object() && artist_obj["name"].is_string())
		{
			artist = artist_obj["name"].get<std::string>();
		}

		if (track["name"].is_string())
		{
			title = track["name"].get<std::string>();
		}

		const auto hash = hm.get_hash(artist, title);

		if (m_hashes.count(hash) == 1)
		{
			auto f = hm.get_fields(hash);
			f.loved = 1;

			hm.set_fields(transaction_ptr, hash, f);
			to_refresh.add_item(hash);
		}
		else
		{
			Message::log("Did not find a match for '{}' by '{}'.", title, artist);
		}
	}

	Message::log("On page {} of {}, {} matches were found from a possible {}.", t.page, t.totalPages, to_refresh.get_count(), tracks.size());

	transaction_ptr->commit();
	hm.refresh(to_refresh);

	if (t.page == 1 && t.totalPages > 1)
	{
		for (size_t i = 2; i <= t.totalPages; ++i)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(250));

			auto task = fb2k::service_new<UserGetLovedTracksTask>(m_hashes, i);
			fb2k::cpuThreadPool::get()->runSingle(task);
		}
	}
}
