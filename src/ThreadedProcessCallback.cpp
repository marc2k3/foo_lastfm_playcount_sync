#include "PCH.hpp"
#include "ThreadedProcessCallback.hpp"

ThreadedProcessCallback::ThreadedProcessCallback(metadb_handle_list_cref handles, std::string_view method)
	: m_handles(handles)
	, m_method(method) {}

#pragma region static
void ThreadedProcessCallback::init(threaded_process_callback::ptr callback, std::string_view title) noexcept
{
	uint32_t flags{};

	if (title.empty())
	{
		flags = threaded_process::flag_silent;
	}
	else
	{
		flags = threaded_process::flag_show_progress | threaded_process::flag_show_delayed | threaded_process::flag_show_item;
	}

	threaded_process::get()->run_modeless(callback, flags, fb2k::get_main_window(), title.data());
}
#pragma endregion

void ThreadedProcessCallback::run(threaded_process_status& status, abort_callback& abort) noexcept
{
	lfm::List to_refresh;
	lfm::Set hashes;
	const auto count = m_handles.get_count();
	auto& hm = HashManager::get();
	auto transaction_ptr = HashManager::api()->begin_transaction();

	for (auto&& [index, handle] : std::views::enumerate(m_handles))
	{
		abort.check();
		status.set_progress(index + 1, count);
		status.set_item_path(handle->get_path());

		auto meta = MetaV2(handle);

		if (!meta)
			continue;

		if (!hashes.insert(meta.hash).second)
			continue;

		auto f = hm.get_fields(meta.hash);
		bool updated{};

		if (m_method == "user.getTrackScrobbles" && handle->get_length() >= 30.0)
		{
			auto task = TrackGetScrobblesTask(meta, 1);
			updated = task.run(f);
		}
		else if (m_method == "track.love" || m_method == "track.unlove")
		{
			auto task = TrackLoveTask(m_method, meta);
			updated = task.run(f);
		}

		if (updated)
		{
			hm.set_fields(transaction_ptr, meta.hash, f);
			to_refresh.add_item(meta.hash);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	if (to_refresh.get_count() > 0uz)
	{
		transaction_ptr->commit();
		hm.refresh(to_refresh);
	}
}
