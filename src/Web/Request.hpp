#pragma once

class Request
{
public:
	using Result = std::expected<JSON, std::string>;

	Request();

	Result GET(const cpr::Parameters& params) noexcept;
	Result POST(const cpr::Payload& payload) noexcept;

private:
	Result parse_response(const cpr::Response& response) noexcept;

	cpr::Session m_session;
};
