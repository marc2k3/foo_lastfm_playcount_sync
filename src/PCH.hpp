#pragma once
#define _ATL_MODULES
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#define WINVER _WIN32_WINNT_WIN10
#define NOMINMAX

#include <algorithm>
#include <array>
#include <chrono>
#include <expected>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <string>

#include <concurrent_unordered_set.h>

#include <cpr/cpr.h>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>
using JSON = nlohmann::json;

#include <helpers/foobar2000+atl.h>
#include <helpers/atl-misc.h>
#include <libPPUI/GDIUtils.h>
#include <libPPUI/Controls.h>
#include <pfc/filetimetools.h>
#include <pfc/string-conv-lite.h>

#include "Component.hpp"
#include "Resource.hpp"
#include "GUIDS.hpp"

#include "Dialog/Config.hpp"
#include "Helpers/Helpers.hpp"
#include "Helpers/Message.hpp"
#include "Helpers/Time.hpp"
#include "ThreadedProcessCallback.hpp"

#include "Metadb/HashManager.hpp"
#include "Metadb/MetaV2.hpp"

#include "Web/AuthTask.hpp"
#include "Web/Request.hpp"
#include "Web/TrackGetInfoTask.hpp"
#include "Web/TrackGetScrobblesTask.hpp"
#include "Web/TrackLoveTask.hpp"
#include "Web/UserGetLovedTracksTask.hpp"
