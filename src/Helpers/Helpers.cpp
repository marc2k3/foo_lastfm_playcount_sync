#include "PCH.hpp"
#include "Helpers.hpp"

namespace lfm
{
	bool get_totals(JSON& json, Totals& t) noexcept
	{
		auto& attr = json["@attr"];

		if (!attr.is_object())
			return false;

		t.page = get_num(attr["page"]).value_or(0u);
		t.total = get_num(attr["total"]).value_or(0u);
		t.totalPages = get_num(attr["totalPages"]).value_or(0u);
		return t.total > 0u;
	}

	std::optional<uint32_t> get_num(JSON& json) noexcept
	{
		if (json.is_number_unsigned())
		{
			return json.get<uint32_t>();
		}
		else if (json.is_string())
		{
			const auto str = json.get<std::string>();

			if (pfc::string_is_numeric(str.c_str()))
			{
				return std::stoul(str);
			}
		}

		return std::nullopt;
	}
}
