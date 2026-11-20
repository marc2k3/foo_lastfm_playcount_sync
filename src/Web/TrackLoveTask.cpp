#include "PCH.hpp"
#include "TrackLoveTask.hpp"

TrackLoveTask::TrackLoveTask(std::string_view method, const MetaV2& meta)
	: m_method(method)
	, m_meta(meta) {}

void TrackLoveTask::build_payload() noexcept
{
	const auto sk = Config::get_session_key();

	const auto api_sig = lfm::md5(
		"api_key{}artist{}method{}sk{}track{}{}",
		Component::api_key,
		m_meta.artist, m_method,
		sk,
		m_meta.track,
		Component::secret
	);

	m_payload.Add({ "api_key", Component::api_key.data() });
	m_payload.Add({ "api_sig", api_sig });
	m_payload.Add({ "method", m_method });
	m_payload.Add({ "sk", sk });
	m_payload.Add({ "artist", m_meta.artist });
	m_payload.Add({ "track", m_meta.track });
}

bool TrackLoveTask::run(lfm::Fields& f) noexcept
{
	build_payload();

	Request request;
	auto result = request.POST(m_payload);
	auto& str = result.error(); // response is never JSON so it's always an error string

	if (!str.contains("ok"))
	{
		Message::log(str);
		return false;
	}

	if (m_method == "track.love")
	{
		f.loved = 1;
		Message::log("'{}' by '{}' loved succesfully.", m_meta.track, m_meta.artist);
	}
	else if (m_method == "track.unlove")
	{
		f.loved = 0;
		Message::log("'{}' by '{}' unloved succesfully.", m_meta.track, m_meta.artist);
	}

	return true;
}
