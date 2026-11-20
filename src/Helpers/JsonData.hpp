#pragma once

class JsonData
{
public:
	static void Export(const fb2k::stringRef& path) noexcept;
	static void Import(const fb2k::stringRef& path) noexcept;

private:
	static std::filesystem::path GetFSPath(const fb2k::stringRef& path) noexcept;
	static uint32_t GetUint32(JSON& json) noexcept;
};
