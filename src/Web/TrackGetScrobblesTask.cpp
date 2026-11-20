#include "PCH.hpp"
#include "TrackGetScrobblesTask.hpp"

TrackGetScrobblesTask::TrackGetScrobblesTask(const MetaV2& meta, size_t page)
	: m_meta(meta)
	, m_page(page) {}

#pragma region static
uint32_t TrackGetScrobblesTask::get_timestamp(JSON& json) noexcept
{
	auto& date = json["date"];

	if (date.is_object())
	{
		auto& uts = date["uts"];
		return lfm::get_num(uts).value_or(0u);
	}

	return 0u;
}
#pragma endregion

void TrackGetScrobblesTask::build_params() noexcept
{
	const auto sk = Config::get_session_key();

	m_params.Add({ "api_key" , Component::api_key.data() });
	m_params.Add({ "autocorrect", "0" });
	m_params.Add({ "format", "json" });
	m_params.Add({ "method", "user.getTrackScrobbles" });
	m_params.Add({ "artist", m_meta.artist });
	m_params.Add({ "track", m_meta.track });
	m_params.Add({ "page", fmt::to_string(m_page) });
	m_params.Add({ "limit", "200" });

	if (sk.length())
	{
		m_params.Add({ "sk", sk });
	}
	else
	{
		m_params.Add({ "username", Config::get_user_name() });
	}
}

bool TrackGetScrobblesTask::run(lfm::Fields& f) noexcept
{
	build_params();

	Request request;
	auto result = request.GET(m_params);

	if (!result)
	{
		Message::log(result.error());
		return false;
	}

	auto& trackscrobbles = result.value()["trackscrobbles"];

	if (!trackscrobbles.is_object())
		return false;

	lfm::Totals t{};

	if (!lfm::get_totals(trackscrobbles, t))
		return false;

	auto& tracks = trackscrobbles["track"];

	if (!tracks.is_array())
		return false;

	const auto last_item_index = tracks.size() - 1;

	if (t.page == 1)
	{
		f.playcount = t.total;
		f.last_played = get_timestamp(tracks[0]);
		f.first_played = get_timestamp(tracks[last_item_index]);

		if (t.totalPages > 1) // more than 1 page? We need to fetch the first played date from there.
		{
			auto task = TrackGetScrobblesTask(m_meta, t.totalPages);
			return task.run(f);
		}
	}
	else // not the first page - only interested in first played date here
	{
		f.first_played = get_timestamp(tracks[last_item_index]);
	}

	return true;
}
