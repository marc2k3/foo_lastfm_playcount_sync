#include "PCH.hpp"

namespace
{
	class DialogPreferences : public CDialogImpl<DialogPreferences>, public preferences_page_instance
	{
	public:
		DialogPreferences(preferences_page_callback::ptr callback) : m_callback(callback) {}

		BEGIN_MSG_MAP_EX(DialogPreferences)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_RANGE_HANDLER_EX(IDC_CHECK_ENABLED, IDC_EDIT_USER_NAME, OnChanged)
			COMMAND_ID_HANDLER_EX(IDC_BTN_AUTH, OnAuthBnClicked)
			COMMAND_ID_HANDLER_EX(IDC_BTN_CLEAR, OnClearBnClicked)
		END_MSG_MAP()

		enum { IDD = IDD_DIALOG_PREFERENCES };

		uint32_t get_state() noexcept final
		{
			const auto has_changed = [this]
			{
				if (m_check_enabled.IsChecked() != Config::enabled)
					return true;

				if (pfc::getWindowText(m_edit_user_name) != Config::get_user_name().c_str())
					return true;

				return false;
			}();

			uint32_t state = preferences_state::dark_mode_supported | preferences_state::resettable;

			if (has_changed)
			{
				state |= preferences_state::changed;
			}

			return state;
		}

		void apply() noexcept final
		{
			Config::enabled = m_check_enabled.IsChecked();
			const auto temp = pfc::getWindowText(m_edit_user_name);

			if (temp != Config::get_user_name())
			{
				Config::set_user_name(temp);
				Config::set_session_key("");
				m_btn_clear.EnableWindow(FALSE);
				m_btn_auth.EnableWindow(Config::user_check());
			}
		}

		void reset() noexcept final
		{
			m_check_enabled.SetCheck(false);
			pfc::setWindowText(m_edit_user_name, "");
			on_change();
		}

	private:
		BOOL OnInitDialog(CWindow, LPARAM) noexcept
		{
			CreatePreferencesHeaderFont(m_font, m_hWnd);
			GetDlgItem(IDC_HEADER).SetFont(m_font.m_hFont);

			const bool enabled = Config::enabled;
			m_check_enabled = GetDlgItem(IDC_CHECK_ENABLED);
			m_check_enabled.SetCheck(enabled);

			m_edit_user_name = GetDlgItem(IDC_EDIT_USER_NAME);
			pfc::setWindowText(m_edit_user_name, Config::get_user_name().c_str());
			m_edit_user_name.EnableWindow(enabled);

			m_btn_clear = GetDlgItem(IDC_BTN_CLEAR);
			m_btn_auth = GetDlgItem(IDC_BTN_AUTH);

			if (Config::session_check())
			{
				m_btn_clear.EnableWindow(TRUE);
				m_btn_auth.EnableWindow(FALSE);
			}
			else
			{
				m_btn_clear.EnableWindow(FALSE);
				m_btn_auth.EnableWindow(Config::user_check());
			}

			m_hooks.AddDialogWithControls(m_hWnd);
			return FALSE;
		}

		void OnAuthBnClicked(uint32_t, int32_t, CWindow) noexcept
		{
			auto task = fb2k::service_new<AuthTask>(m_hWnd, "auth.getToken");
			fb2k::cpuThreadPool::get()->runSingle(task);
		}

		void OnChanged(uint32_t, int32_t, CWindow) noexcept
		{
			on_change();
		}

		void OnClearBnClicked(uint32_t, int32_t, CWindow) noexcept
		{
			Config::set_session_key("");
			on_change();
		}

		void on_change() noexcept
		{
			const bool enabled = m_check_enabled.IsChecked();
			m_edit_user_name.EnableWindow(enabled);

			if (enabled && m_edit_user_name.GetWindowTextLengthW() > 0)
			{
				const auto sk = Config::get_session_key();
				m_btn_clear.EnableWindow(!sk.empty());
				m_btn_auth.EnableWindow(sk.empty());
			}
			else
			{
				m_btn_clear.EnableWindow(FALSE);
				m_btn_auth.EnableWindow(FALSE);
			}

			m_callback->on_state_changed();
		}

		CButton m_btn_clear, m_btn_auth;
		CCheckBox m_check_enabled;
		CEdit m_edit_user_name;
		CFont m_font;
		fb2k::CCoreDarkModeHooks m_hooks;
		preferences_page_callback::ptr m_callback;
	};

	class PreferencesPageImpl : public preferences_page_impl<DialogPreferences>
	{
	public:
		GUID get_guid() noexcept final
		{
			return guids::preferences_page;
		}

		GUID get_parent_guid() noexcept final
		{
			return preferences_page::guid_tools;
		}

		bool get_help_url(pfc::string_base& out) noexcept final
		{
			out = Component::home_page;
			return true;
		}

		const char* get_name() noexcept final
		{
			return Component::name.data();
		}
	};

	FB2K_SERVICE_FACTORY(PreferencesPageImpl);
	FB2K_ON_INIT_STAGE(CSeparator::Register, init_stages::before_ui_init)
}
