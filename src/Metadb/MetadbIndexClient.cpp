#include "PCH.hpp"

namespace
{
	class MetadbIndexClient : public metadb_index_client
	{
	public:
		metadb_index_hash transform(const file_info& info, const playable_location&) noexcept final
		{
			return HashManager::get().get_hash(info);
		}
	};

	static auto g_client = new service_impl_single_t<MetadbIndexClient>();

	static void init_stage() noexcept
	{
		try
		{
			HashManager::api()->add(g_client, guids::metadb_index, system_time_periods::week * 4);
			HashManager::api()->dispatch_global_refresh();
		}
		catch (const std::exception& e)
		{
			HashManager::api()->remove(guids::metadb_index);

			fb2k::inMainThread([msg = fmt::format("Critical stats initialisation failure: {}", e.what())]
				{
					Message::popup(msg);
				});
		}
	}

	FB2K_ON_INIT_STAGE(init_stage, init_stages::before_config_read)
}
