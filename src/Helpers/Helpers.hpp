#pragma once

namespace lfm
{
	struct Fields
	{
		uint32_t playcount{};
		uint32_t loved{};
		uint32_t first_played{};
		uint32_t last_played{};

		operator bool() const { return playcount || loved || first_played || last_played; }
	};

	struct Totals
	{
		uint32_t page{};
		uint32_t total{};
		uint32_t totalPages{};
	};

	using List = pfc::list_t<metadb_index_hash>;
	using Set = concurrency::concurrent_unordered_set<metadb_index_hash>;

	template <typename... Args>
	static std::string md5(fmt::string_view format, Args&&... args) noexcept
	{
		const auto str = fmt::format(fmt::runtime(format), std::forward<Args>(args)...);
		return hasher_md5::get()->process_single_string(str.c_str()).asString().get_ptr();
	}

	bool get_totals(JSON& json, Totals& t) noexcept;
	std::optional<uint32_t> get_num(JSON& json) noexcept;
}
