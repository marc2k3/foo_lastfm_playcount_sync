#include "PCH.hpp"
#include "HashManager.hpp"

HashManager::HashManager() : m_hasher_api(hasher_md5::get()), m_metadb_api(metadb_v2::get())
{
	titleformat_compiler::get()->compile_force(m_obj, "$lower($meta(artist,0) - %title%)");
}

lfm::Fields HashManager::get_fields(const metadb_v2::rec_t& rec) noexcept
{
	const auto hash = get_hash(rec.info->info());
	return get_fields(hash);
}

lfm::Fields HashManager::get_fields(metadb_index_hash hash) noexcept
{
	mem_block_container_impl data;
	api()->get_user_data(guids::metadb_index, hash, data);

	if (data.get_size() > 0uz)
	{
		try
		{
			lfm::Fields f;
			auto reader = stream_reader_formatter_simple_ref(data.get_ptr(), data.get_size());
			reader >> f.playcount;
			reader >> f.loved;
			reader >> f.first_played;
			reader >> f.last_played;
			return f;
		}
		catch (exception_io_data) {}
	}

	return {};
}

lfm::Set HashManager::get_hashes(metadb_handle_list_cref handles) noexcept
{
	lfm::Set hashes;

	m_metadb_api->queryMultiParallel_(handles, [&](size_t idx, const metadb_v2::rec_t& rec)
		{
			pfc::string8 str;
			m_metadb_api->formatTitle_v2(handles[idx], rec, nullptr, str, m_obj, nullptr);
			const auto hash = string_to_hash(str);
			hashes.insert(hash);
		});

	return hashes;
}

lfm::Set HashManager::get_library_hashes() noexcept
{
	metadb_handle_list items;
	library_manager::get()->get_all_items(items);
	return get_hashes(items);
}

metadb_index_hash HashManager::get_hash(const file_info& info) noexcept
{
	pfc::string8 str;
	m_obj->run_simple(playable_location_impl(), &info, str);
	return string_to_hash(str);
}

metadb_index_hash HashManager::get_hash(std::string_view artist, std::string_view title) noexcept
{
	const pfc::string8 str = fmt::format("{} - {}", artist, title);
	return string_to_hash(str.toLower());
}

metadb_index_hash HashManager::string_to_hash(std::string_view str) noexcept
{
	return m_hasher_api->process_single_string(str.data()).xorHalve();
}

void HashManager::import_lastfm_loved(bool clear_existing) noexcept
{
	if (clear_existing)
	{
		reset_loved_library_tracks();
	}

	const auto hashes = get_library_hashes();
	auto task = fb2k::service_new<UserGetLovedTracksTask>(hashes, 1);
	fb2k::cpuThreadPool::get()->runSingle(task);
}

void HashManager::increment_playcount(metadb_index_hash hash, uint32_t ts) noexcept
{
	auto f = get_fields(hash);
	f.playcount++;
	f.last_played = ts;

	if (f.first_played == 0u)
	{
		f.first_played = ts;
	}

	set_fields(hash, f);
	refresh(hash);
}

void HashManager::refresh(metadb_index_hash hash) noexcept
{
	fb2k::inMainThread([this, hash]
		{
			api()->dispatch_refresh(guids::metadb_index, hash);
		});
}

void HashManager::refresh(const lfm::List& hashes) noexcept
{
	fb2k::inMainThread([this, hashes]
		{
			api()->dispatch_refresh(guids::metadb_index, hashes);
		});
}

void HashManager::reset_loved_library_tracks() noexcept
{
	auto filter = search_filter_manager_v2::get()->create_ex(
		"%lfm_loved% IS 1",
		fb2k::service_new<completion_notify_dummy>(),
		search_filter_manager_v2::KFlagSuppressNotify
	);

	auto arr = search_index_manager::get()->get_library_index()->search(filter, nullptr, 0u, fb2k::noAbort);
	const auto& items = arr->as_list_of<metadb_handle>();

	if (items.get_count() == 0uz)
		return;

	lfm::List to_refresh;
	const auto hashes = get_hashes(items);
	auto transaction_ptr = api()->begin_transaction();

	for (const metadb_index_hash hash : hashes)
	{
		auto f = get_fields(hash);
		f.loved = 0u;
		set_fields(transaction_ptr, hash, f);
		to_refresh.add_item(hash);
	}

	transaction_ptr->commit();
	refresh(to_refresh);
}

void HashManager::reset_tracks(metadb_handle_list_cref handles) noexcept
{
	lfm::List to_refresh;
	const auto hashes = get_hashes(handles);
	auto transaction_ptr = api()->begin_transaction();

	for (const metadb_index_hash hash : hashes)
	{
		set_fields(transaction_ptr, hash, lfm::Fields());
		to_refresh.add_item(hash);
	}

	transaction_ptr->commit();
	refresh(to_refresh);
}

void HashManager::set_fields(metadb_index_hash hash, const lfm::Fields& f) noexcept
{
	stream_writer_formatter_simple writer;
	writer << f.playcount;
	writer << f.loved;
	writer << f.first_played;
	writer << f.last_played;

	api()->set_user_data(
		guids::metadb_index,
		hash,
		writer.m_buffer.get_ptr(),
		writer.m_buffer.get_size()
	);
}

void HashManager::set_fields(const metadb_index_transaction::ptr& ptr, metadb_index_hash hash, const lfm::Fields& f) noexcept
{
	stream_writer_formatter_simple writer;
	writer << f.playcount;
	writer << f.loved;
	writer << f.first_played;
	writer << f.last_played;

	ptr->set_user_data(
		guids::metadb_index,
		hash,
		writer.m_buffer.get_ptr(),
		writer.m_buffer.get_size()
	);
}
