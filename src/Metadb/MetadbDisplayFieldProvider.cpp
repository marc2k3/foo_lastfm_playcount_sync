#include "PCH.hpp"

namespace
{
	static constexpr std::array field_names =
	{
		"lfm_playcount",
		"lfm_loved",
		"lfm_first_played",
		"lfm_first_played_ts",
		"lfm_last_played",
		"lfm_last_played_ts",
		"lfm_now",
		"lfm_now_ts",
		"lfm_first_played_ago",
		"lfm_last_played_ago",
	};

	class MetadbDisplayFieldProvider : public metadb_display_field_provider_v2
	{
	public:
		bool process_field(uint32_t index, metadb_handle* handle, titleformat_text_out* out) noexcept final
		{
			return process_field_v2(index, handle, handle->query_v2_(), out);
		}

		bool process_field_v2(uint32_t index, metadb_handle*, const metadb_v2::rec_t& rec, titleformat_text_out* out) noexcept final
		{
			if (rec.info.is_empty())
				return false;

			const auto f = HashManager::get().get_fields(rec);

			switch (index)
			{
			case 0u:
				if (f.playcount == 0u)
					return false;

				out->write_int(titleformat_inputtypes::meta, f.playcount);
				return true;
			case 1u:
				if (f.loved == 0u)
					return false;

				out->write_int(titleformat_inputtypes::meta, f.loved);
				return true;
			case 2u:
			case 3u:
				return write_ts(out, f.first_played, index == 2u);
			case 4u:
			case 5u:
				return write_ts(out, f.last_played, index == 4u);
			case 6u:
			case 7u:
				return write_ts(out, Time::Now(), index == 6u);
			case 8u:
				return write_ago(out, f.first_played);
			case 9u:
				return write_ago(out, f.last_played);
			}

			return false;
		}

		uint32_t get_field_count() noexcept final
		{
			return static_cast<uint32_t>(field_names.size());
		}

		void get_field_name(uint32_t index, pfc::string_base& out) noexcept final
		{
			out = field_names[index];
		}

	private:
		bool write_ago(titleformat_text_out* out, uint32_t ts) noexcept
		{
			const auto now_ts = Time::Now();

			if (ts == 0u || ts > now_ts)
				return false;

			pfc::string8 str;
			uint32_t diff = now_ts - ts;

			if (diff < day_in_seconds * 2u)
			{
				const auto today_string = Time::FormatUnixTimestamp(now_ts).subString(0uz, 10uz);
				const auto yesterday_string = Time::FormatUnixTimestamp(now_ts - day_in_seconds).subString(0uz, 10uz);
				const auto ts_string = Time::FormatUnixTimestamp(ts).subString(0uz, 10uz);

				if (ts_string == today_string)
				{
					str = "Today";
				}
				else if (ts_string == yesterday_string)
				{
					str = "Yesterday";
				}
				else
				{
					str = "1d";
				}
			}
			else
			{
				bool include_weeks_days = true;

				const auto years = diff / year_in_seconds;

				if (years > 0u)
				{
					include_weeks_days = false;
					diff -= years * year_in_seconds;
					str << years << "y ";
				}

				const auto months = diff / month_in_seconds;

				if (months > 0u)
				{
					diff -= months * month_in_seconds;
					str << months << "m ";
				}

				if (include_weeks_days)
				{
					const auto weeks = diff / week_in_seconds;

					if (weeks > 0u)
					{
						diff -= weeks * week_in_seconds;
						str << weeks << "w ";
					}

					const auto days = diff / day_in_seconds;

					if (days > 0u)
					{
						str << days << "d";
					}
				}
			}

			out->write(titleformat_inputtypes::unknown, str.trim(' '));
			return true;
		}

		bool write_ts(titleformat_text_out* out, uint32_t ts, bool as_string) noexcept
		{
			if (ts == 0u)
				return false;

			if (as_string)
				out->write(titleformat_inputtypes::meta, Time::FormatUnixTimestamp(ts));
			else
				out->write_int(titleformat_inputtypes::meta, ts);

			return true;
		}

		static constexpr uint32_t day_in_seconds = 24u * 60u * 60u;
		static constexpr uint32_t week_in_seconds = 7u * day_in_seconds;
		static constexpr uint32_t month_in_seconds = 30u * day_in_seconds;
		static constexpr uint32_t year_in_seconds = 365u * day_in_seconds;
	};

	FB2K_SERVICE_FACTORY(MetadbDisplayFieldProvider);
}
