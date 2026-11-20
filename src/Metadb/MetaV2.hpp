#pragma once

class MetaV2
{
public:
	MetaV2(const metadb_handle_ptr& handle)
	{
		if (handle)
		{
			m_info = handle->query_v2_().info;

			if (m_info)
			{
				artist = get_tag("ARTIST");
				track = get_tag("TITLE");
				hash = HashManager::get().get_hash(artist, track);
			}
		}
	}

	metadb_index_hash hash{};
	std::string album, albumArtist, artist, track;

	operator bool() const { return artist.length() && track.length(); }

private:
	std::string get_tag(std::string_view name) noexcept
	{
		const char* value = m_info->info().meta_get(name.data(), 0uz);

		if (value != nullptr)
			return value;

		return {};
	}

	metadb_info_container::ptr m_info;
};
