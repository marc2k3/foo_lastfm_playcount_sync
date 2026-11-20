#include "PCH.hpp"
#include "Request.hpp"

Request::Request()
{
	m_session.SetUrl(Component::base_url);
	m_session.SetUserAgent(Component::user_agent);
}

Request::Result Request::GET(const cpr::Parameters& params) noexcept
{
	m_session.SetParameters(params);

	auto r = m_session.Get();
	return parse_response(r);
}

Request::Result Request::POST(const cpr::Payload& payload) noexcept
{
	m_session.SetPayload(payload);

	auto r = m_session.Post();
	return parse_response(r);
}

Request::Result Request::parse_response(const cpr::Response& response) noexcept
{
	if (response.status_code == 0)
		return std::unexpected(response.error.message);

	auto json = JSON::parse(response.text, nullptr, false);

	if (!json.is_object())
		return std::unexpected(response.text);

	auto& message = json["message"];

	if (message.is_string()) // API error
		return std::unexpected(message.get<std::string>());

	return json;
}
