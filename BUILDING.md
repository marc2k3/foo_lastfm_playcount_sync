- Make sure you have Visual Studio 2026 installed with the C++ workload, ATL and the Windows 11 SDK.

- Install dependencies via vcpkg
```
vcpkg install cpr curl[http2] fmt nlohmann-json wtl --triplet x64-windows-static
vcpkg install cpr curl[http2] fmt nlohmann-json wtl --triplet x86-windows-static
```

- Perform a recursive clone of the repository
```
git clone --recurse https://github.com/marc2k3/foo_lastfm_playcount_sync
```

- Open `src/foo_lastfm_playcount_sync.slnx`.

- Edit `Component.hpp` and add your own Last.fm PRO API key and secret.

- Check the configuration is set to `Release` and choose your platform before building.

- On success, the compiled DLL should be found at

`src\x64\Release\foo_lastfm_playcount_sync.dll`

or

`src\Win32\Release\foo_lastfm_playcount_sync.dll`.
