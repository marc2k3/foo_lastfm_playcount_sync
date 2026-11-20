#include "PCH.hpp"

namespace
{
	class TrackPropertyProvider : public track_property_provider_v5
	{
	public:
		bool is_our_tech_info(const char*) noexcept final
		{
			return false;
		}

		void enumerate_properties_v5(
			metadb_handle_list_cref handles,
			track_property_provider_v5_info_source& source,
			track_property_callback_v2& callback,
			abort_callback&) noexcept final
		{
			if (!callback.is_group_wanted(Component::name.data()))
				return;

			auto& hm = HashManager::get();
			const auto count = handles.get_count();

			if (count == 1uz)
			{
				auto rec = source.get_info(0uz);

				if (rec.info.is_empty())
					return;

				const auto f = hm.get_fields(rec);

				if (f.playcount > 0u)
					callback.set_property(Component::name.data(), 0.0, "Playcount", pfc::format_uint(f.playcount));

				if (f.loved > 0u)
					callback.set_property(Component::name.data(), 1.0, "Loved", pfc::format_uint(f.loved));

				if (f.first_played > 0u)
					callback.set_property(Component::name.data(), 2.0, "First Played", Time::FormatUnixTimestamp(f.first_played));

				if (f.last_played > 0u)
					callback.set_property(Component::name.data(), 3.0, "Last Played", Time::FormatUnixTimestamp(f.last_played));
			}
			else
			{
				lfm::Set hashes;
				uint32_t total{};

				for (size_t i = 0uz; i < count; ++i)
				{
					auto rec = source.get_info(i);

					if (rec.info.is_empty())
						continue;

					const auto hash = hm.get_hash(rec.info->info());

					if (hashes.insert(hash).second)
					{
						total += hm.get_fields(hash).playcount;
					}
				}

				if (total > 0u)
					callback.set_property(Component::name.data(), 0.0, "Playcount", pfc::format_uint(total));
			}
		}
	};

	FB2K_SERVICE_FACTORY(TrackPropertyProvider);
}
