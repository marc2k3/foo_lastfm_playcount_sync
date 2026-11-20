#pragma once

namespace Config
{
	bool session_check() noexcept;
	bool user_check() noexcept;
	std::string get_session_key() noexcept;
	std::string get_user_name() noexcept;
	void set_session_key(std::string_view sk) noexcept;
	void set_user_name(std::string_view name) noexcept;

	extern cfg_bool enabled;
}
