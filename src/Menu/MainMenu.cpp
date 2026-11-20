#include "PCH.hpp"
#include "Helpers/JsonData.hpp"

namespace
{
	static constexpr std::array main_items =
	{
		std::make_pair(&guids::mainmenu_import_clear, "Import Last.fm loved tracks (clear existing)"),
		std::make_pair(&guids::mainmenu_import_keep,  "Import Last.fm loved tracks (keep existing)"),
		std::make_pair(&guids::mainmenu_show_loved,   "Show loved tracks"),
		std::make_pair(&guids::mainmenu_show_recent,  "Show recently played"),
		std::make_pair(&guids::mainmenu_import_json,  "Import data..."),
		std::make_pair(&guids::mainmenu_export_json,  "Export data..."),
	};

	static constexpr auto main_item_count = static_cast<uint32_t>(main_items.size());

	class MainMenu : public mainmenu_commands
	{
	public:
		GUID get_command(uint32_t index) final
		{
			if (index >= main_item_count)
				FB2K_BugCheck();

			return *main_items[index].first;
		}

		GUID get_parent() noexcept final
		{
			return guids::mainmenu_group;
		}

		bool get_description(uint32_t index, pfc::string_base& out) final
		{
			if (index >= main_item_count)
				FB2K_BugCheck();

			get_name(index, out);
			return true;
		}

		bool get_display(uint32_t index, pfc::string_base& out, uint32_t& flags) final
		{
			if (index >= main_item_count)
				FB2K_BugCheck();

			flags = 0u;

			if ((index <= 1 && !Config::user_check()) || !check_library_init())
			{
				flags |= flag_disabled;
			}

			get_name(index, out);
			return true;
		}

		uint32_t get_command_count() noexcept final
		{
			return main_item_count;
		}

		void execute(uint32_t index, service_ptr_t<service_base>) final
		{
			if (index >= main_item_count)
				FB2K_BugCheck();

			if (!check_library_init())
				return;

			switch (index)
			{
			case 0: // import/clear
			case 1: // import/keep
				if (Config::user_check())
				{
					HashManager::get().import_lastfm_loved(index == 0u);
				}
				break;
			case 2:
				library_search_ui::get()->show("%lfm_loved% IS 1");
				break;
			case 3:
				library_search_ui::get()->show("%lfm_last_played% DURING LAST 1 WEEK SORT DESCENDING BY %lfm_last_played%");
				break;
			case 4:
				init_file_dialog(false, JsonData::Import);
				break;
			case 5:
				init_file_dialog(true, JsonData::Export);
				break;
			}
		}

		void get_name(uint32_t index, pfc::string_base& out) final
		{
			if (index >= main_item_count)
				FB2K_BugCheck();

			out = main_items[index].second;
		}

	private:
		bool check_library_init() noexcept
		{
			auto api = library_manager_v4::get();

			if (api->is_library_enabled() && api->is_initialized())
			{
				metadb_handle_list items;
				api->get_all_items(items);
				return items.get_count() > 0uz;
			}

			return false;
		}

		void init_file_dialog(bool save, const fb2k::fileDialogGetPath_t& path_func) noexcept
		{
			fb2k::fileDialogSetup::ptr dialog;

			if (save)
			{
				dialog = fb2k::fileDialog::get()->setupSave();
				dialog->setTitle("Save as");
				dialog->setDefaultExtension("json");
			}
			else
			{
				dialog = fb2k::fileDialog::get()->setupOpen();
				dialog->setTitle("Import from");
			}

			dialog->setParent(fb2k::get_main_window());
			dialog->setFileTypes("JSON file|*.json|All files|*.*");
			dialog->runSimple(path_func);
		}
	};

	static const auto mainmenu_group = mainmenu_group_popup_factory(
		guids::mainmenu_group,
		mainmenu_groups::library,
		mainmenu_commands::sort_priority_base,
		Component::name.data()
	);

	FB2K_SERVICE_FACTORY(MainMenu);
}
