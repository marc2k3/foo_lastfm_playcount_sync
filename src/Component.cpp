#include "PCH.hpp"

namespace Component
{
	static std::string about_text() noexcept
	{
		const auto date = pfc::string(__DATE__).replace("  ", " ");
		const auto msvc = fmt::to_string(_MSC_FULL_VER);

		return fmt::format(
			"Copyright (c) 2022 - 2026 marc2003\n\n"
			"Build: {}, {}\n\n"
			"foobar2000 SDK: {}\n"
			"MSVC: {}.{}.{}",
			__TIME__,
			date.get_ptr(),
			FOOBAR2000_SDK_VERSION,
			msvc.substr(0, 2),
			msvc.substr(2, 2),
			msvc.substr(4)
		);
	}

	DECLARE_COMPONENT_VERSION(
		name,
		version,
		about_text()
	);

	class InstallationValidator : public component_installation_validator
	{
	public:
		bool is_installed_correctly() noexcept final
		{
			return test_my_name("foo_lastfm_playcount_sync.dll")
				&& core_version_info_v2::get()->test_version(2u, 25u, 0u, 0u)
				&& IsWindows10OrGreater();
		}
	};

	FB2K_SERVICE_FACTORY(InstallationValidator);
}
