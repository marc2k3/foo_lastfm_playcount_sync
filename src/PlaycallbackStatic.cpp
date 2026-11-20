#include "PCH.hpp"

namespace
{
	class PlayCallbackStatic : public play_callback_static
	{
	public:
		uint32_t get_flags() noexcept final
		{
			return flag_on_playback_new_track | flag_on_playback_time;
		}

		void on_playback_new_track(metadb_handle_ptr handle) noexcept final
		{
			m_counter = 0uz;
			m_target = SIZE_MAX;

			if (!handle)
				return;

			const auto length = handle->get_length();

			if (length >= 30.0 && Config::user_check())
			{
				const auto half = static_cast<size_t>(std::round(length / 2.0));
				m_target = std::min(half, 240uz);
			}
		}

		void on_playback_time(double) noexcept final
		{
			if (m_target == SIZE_MAX)
				return;

			m_counter++;

			if (m_counter == 3)
			{
				if (Config::user_check())
				{
					auto handle = get_now_playing_handle();
					auto meta = MetaV2(handle);

					if (meta)
					{
						auto task = fb2k::service_new<TrackGetInfoTask>(meta); // get loved status
						fb2k::cpuThreadPool::get()->runSingle(task);
					}
				}
			}
			else if (m_counter == 5)
			{
				if (Config::user_check())
				{
					auto handle = get_now_playing_handle();

					if (!handle)
						return;

					const auto handles = pfc::list_single_ref_t<metadb_handle_ptr>(handle);
					auto callback = fb2k::service_new<ThreadedProcessCallback>(handles, "user.getTrackScrobbles");
					ThreadedProcessCallback::init(callback);
				}
			}
			else if (m_counter == m_target)
			{
				if (Config::user_check())
				{
					auto handle = get_now_playing_handle();
					auto meta = MetaV2(handle);

					if (meta)
						HashManager::get().increment_playcount(meta.hash, Time::Now());
				}
			}
		}

		void on_playback_dynamic_info(const file_info&) noexcept final {}
		void on_playback_dynamic_info_track(const file_info&) noexcept final {}
		void on_playback_edited(metadb_handle_ptr) noexcept final {}
		void on_playback_pause(bool) noexcept final {}
		void on_playback_seek(double) noexcept final {}
		void on_playback_starting(play_control::t_track_command, bool) noexcept final {}
		void on_playback_stop(play_control::t_stop_reason) noexcept final {}
		void on_volume_change(float) noexcept final {}

	private:
		static metadb_handle_ptr get_now_playing_handle() noexcept
		{
			metadb_handle_ptr handle;
			playback_control::get()->get_now_playing(handle);
			return handle;
		}

		size_t m_counter{};
		size_t m_target = SIZE_MAX;
	};

	FB2K_SERVICE_FACTORY(PlayCallbackStatic);
}
