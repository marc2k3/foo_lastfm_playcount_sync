#include "PCH.hpp"
#include "TrackGetInfoTask.hpp"

TrackGetInfoTask::TrackGetInfoTask(const MetaV2& meta)
	: m_meta(meta) {}

void TrackGetInfoTask::build_params() noexcept
{
	m_params.Add({ "api_key" , Component::api_key.data() });
	m_params.Add({ "autocorrect", "0" });
	m_params.Add({ "format", "json" });
	m_params.Add({ "method", "track.getInfo" });
	m_params.Add({ "artist", m_meta.artist });
	m_params.Add({ "track", m_meta.track });
	m_params.Add({ "username", Config::get_user_name() });
}

void TrackGetInfoTask::run() noexcept
{
	build_params();

	Request request;
	auto result = request.GET(m_params);

	if (!result)
	{
		Message::log(result.error());
		return;
	}

	auto& track = result.value()["track"];

	if (!track.is_object())
		return;

	const auto userloved = lfm::get_num(track["userloved"]);

	if (!userloved)
		return;

	auto& hm = HashManager::get();
	auto f = hm.get_fields(m_meta.hash);
	f.loved = userloved.value() == 1u ? 1u : 0u;
	hm.set_fields(m_meta.hash, f);
	hm.refresh(m_meta.hash);
}
