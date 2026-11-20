#pragma once

class AuthTask : public fb2k::threadEntry
{
public:
	AuthTask(CWindow parent, std::string_view method, std::string_view token = "");

	void run() noexcept final;

private:
	void build_payload() noexcept;

	CWindow m_parent;
	cpr::Payload m_payload{};
	std::string m_method, m_token;
};
