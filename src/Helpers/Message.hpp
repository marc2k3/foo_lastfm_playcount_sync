#pragma once

struct Message
{
	template <typename... Args>
	static void log(fmt::string_view format, Args&&... args) noexcept
	{
		const auto msg = fmt::format(fmt::runtime(format), std::forward<Args>(args)...);
		FB2K_console_formatter() << Component::name.data() << ": " << msg.c_str();
	}

	template <typename... Args>
	static void popup(fmt::string_view format, Args&&... args) noexcept
	{
		const auto msg = fmt::format(fmt::runtime(format), std::forward<Args>(args)...);
		popup_message::g_show(msg.c_str(), Component::name.data());
	}
};
