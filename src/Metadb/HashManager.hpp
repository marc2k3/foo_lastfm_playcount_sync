#pragma once

class HashManager
{
public:
	HashManager();

	static HashManager& get()
	{
		static HashManager instance;
		return instance;
	}

	static metadb_index_manager_v2::ptr api()
	{
		static metadb_index_manager_v2* cached = metadb_index_manager_v2::get().detach();
		return cached;
	}

	lfm::Fields get_fields(const metadb_v2::rec_t& rec) noexcept;
	lfm::Fields get_fields(metadb_index_hash hash) noexcept;
	lfm::Set get_hashes(metadb_handle_list_cref handles) noexcept;
	lfm::Set get_library_hashes() noexcept;
	metadb_index_hash get_hash(const file_info& info) noexcept;
	metadb_index_hash get_hash(std::string_view artist, std::string_view title) noexcept;
	void import_lastfm_loved(bool clear_existing) noexcept;
	void increment_playcount(metadb_index_hash hash, uint32_t ts) noexcept;
	void refresh(metadb_index_hash hash) noexcept;
	void refresh(const lfm::List& hashes) noexcept;
	void reset_loved_library_tracks() noexcept;
	void reset_tracks(metadb_handle_list_cref handles) noexcept;
	void set_fields(metadb_index_hash hash, const lfm::Fields& f) noexcept;
	void set_fields(const metadb_index_transaction::ptr& ptr, metadb_index_hash hash, const lfm::Fields& f) noexcept;

private:
	metadb_index_hash string_to_hash(std::string_view str) noexcept;

	hasher_md5::ptr m_hasher_api;
	metadb_v2::ptr m_metadb_api;
	titleformat_object::ptr m_obj;
};
