#include "PCH.hpp"
#include "Config.hpp"

namespace Config
{
	cfg_bool enabled(guids::cfg_bool_enabled, false);
	cfg_string session_key(guids::cfg_string_session_key, "");
	cfg_string user_name(guids::cfg_string_user_name, "");

	bool session_check() noexcept
	{
		return user_check() && session_key.get().get_length();
	}

	bool user_check() noexcept
	{
		return enabled && user_name.get().get_length();
	}

	std::string get_session_key() noexcept
	{
		return session_key.get().get_ptr();
	}

	std::string get_user_name() noexcept
	{
		return user_name.get().get_ptr();
	}

	void set_session_key(std::string_view sk) noexcept
	{
		session_key = sk.data();
	}

	void set_user_name(std::string_view name) noexcept
	{
		user_name = name.data();
	}
}
