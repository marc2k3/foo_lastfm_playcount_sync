#include "PCH.hpp"
#include "AuthTask.hpp"

AuthTask::AuthTask(CWindow parent, std::string_view method, std::string_view token)
	: m_parent(parent)
	, m_method(method)
	, m_token(token) {}

void AuthTask::build_payload() noexcept
{
	auto& hm = HashManager::get();
	m_payload.Add({ "api_key", Component::api_key.data() });
	m_payload.Add({ "format", "json" });
	m_payload.Add({ "method", m_method });

	if (m_method == "auth.getToken")
	{
		const auto api_sig = hm.md5(
			"api_key{}method{}{}",
			Component::api_key,
			m_method,
			Component::secret
		);

		m_payload.Add({ "api_sig", api_sig });
	}
	else if (m_method == "auth.getSession")
	{
		const auto api_sig = hm.md5(
			"api_key{}method{}token{}{}",
			Component::api_key,
			m_method,
			m_token,
			Component::secret
		);

		m_payload.Add({ "api_sig", api_sig });
		m_payload.Add({ "token", m_token });
	}
}

void AuthTask::run() noexcept
{
	build_payload();

	Request request;
	auto result = request.POST(m_payload);

	if (!result)
	{
		Message::popup(result.error());
		return;
	}

	auto& j = result.value();

	if (m_method == "auth.getToken")
	{
		auto& jtoken = j["token"];

		if (jtoken.is_string())
		{
			const auto token = jtoken.get<std::string>();
			const auto url = fmt::format("https://last.fm/api/auth/?api_key={}&token={}", Component::api_key, token);
			ShellExecuteW(nullptr, L"open", pfc::wideFromUTF8(url.c_str()), nullptr, nullptr, SW_SHOW);

			const auto status = MessageBoxW(
				m_parent,
				L"Leave this dialog open until you have granted permission for this component through your web browser. Only click OK when you've successfully done that.",
				pfc::wideFromUTF8(Component::name.data()),
				MB_OKCANCEL | MB_ICONINFORMATION | MB_APPLMODAL
			);

			if (status == IDOK)
			{
				auto task = fb2k::service_new<AuthTask>(m_parent, "auth.getSession", token);
				fb2k::cpuThreadPool::get()->runSingle(task);
			}
		}
	}
	else if (m_method == "auth.getSession")
	{
		auto& session = j["session"];

		if (session.is_object())
		{
			auto& jkey = session["key"];

			if (jkey.is_string())
			{
				const auto key = jkey.get<std::string>();
				Config::set_session_key(key);

				m_parent.GetDlgItem(IDC_BTN_CLEAR).EnableWindow(TRUE);
				m_parent.GetDlgItem(IDC_BTN_AUTH).EnableWindow(FALSE);

				Message::popup("Authorisation was successful.");
			}
		}
	}
}
