#include "PCH.hpp"

namespace
{
	enum Item : size_t
	{
		Guid,
		Name,
		Desc
	};

	static constexpr std::array context_items =
	{
		std::make_tuple(
			&guids::context_love,
			"Love",
			"Send track love request for the selected items."
		),
		std::make_tuple(
			&guids::context_unlove,
			"Unlove",
			"Send track unlove request for the selected items."
		),
		std::make_tuple(
			&guids::context_fetch_plays,
			"Fetch stats",
			"Fetches stats for the selected items. Does not include loved status."
		),
		std::make_tuple(
			&guids::context_clear,
			"Clear stats",
			"Clear stats & loved status for the selected items."
		),
	};

	static constexpr auto context_item_count = static_cast<uint32_t>(context_items.size());

	class ContextMenu : public contextmenu_item_simple
	{
	public:
		GUID get_item_guid(uint32_t index) final
		{
			if (index >= context_item_count)
				FB2K_BugCheck();

			return *std::get<Item::Guid>(context_items[index]);
		}

		GUID get_parent() noexcept final
		{
			return guids::context_group;
		}

		bool context_get_display(uint32_t index, metadb_handle_list_cref, pfc::string_base& out, uint32_t& flags, const GUID&) final
		{
			if (index >= context_item_count)
				FB2K_BugCheck();

			flags = 0u;

			switch (index)
			{
			case 0u:
			case 1u:
				if (!Config::session_check())
				{
					flags |= FLAG_DISABLED_GRAYED;
				}
				break;
			case 2u:
				if (!Config::user_check())
				{
					flags |= FLAG_DISABLED_GRAYED;
				}
				break;
			}

			get_item_name(index, out);
			return true;
		}

		bool get_item_description(uint32_t index, pfc::string_base& out) final
		{
			if (index >= context_item_count)
				FB2K_BugCheck();

			out = std::get<Item::Desc>(context_items[index]);
			return true;
		}

		uint32_t get_num_items() noexcept final
		{
			return context_item_count;
		}

		void context_command(uint32_t index, metadb_handle_list_cref handles, const GUID&) final
		{
			if (index >= context_item_count)
				FB2K_BugCheck();

			switch (index)
			{
			case 0u:
			case 1u:
			{
				if (Config::session_check())
				{
					const auto method = index == 0u ? "track.love" : "track.unlove";
					auto callback = fb2k::service_new<ThreadedProcessCallback>(handles, method);
					const auto title = handles.get_count() == 1uz ? "" : method;
					ThreadedProcessCallback::init(callback, title);
				}
				break;
			}
			case 2u:
			{
				if (Config::user_check())
				{
					const auto method = "user.getTrackScrobbles";
					auto callback = fb2k::service_new<ThreadedProcessCallback>(handles, method);
					ThreadedProcessCallback::init(callback, method);
				}
				break;
			}
			case 3u:
				HashManager::get().reset_tracks(handles);
				break;
			}
		}

		void get_item_name(uint32_t index, pfc::string_base& out) final
		{
			if (index >= context_item_count)
				FB2K_BugCheck();

			out = std::get<Item::Name>(context_items[index]);
		}
	};

	static const auto contextmenu_group = contextmenu_group_popup_factory(
		guids::context_group,
		contextmenu_groups::root,
		Component::name.data(),
		0.0
	);

	FB2K_SERVICE_FACTORY(ContextMenu);
}
