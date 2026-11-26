### 2.8-dev
- Remove track limits on context menu actions.

### 2.7
- Bug fixes.
- Compiled with the latest `foobar2000` `SDK`.

### 2.6
- Maybe fix potential crash during playback.

### 2.5
- `foobar2000` `2.25` is now required.
- Update third party dependencies and compile with Visual Studio 2026.

### 2.4
- Minor bug fixes.

### 2.3
- Internal improvements

### 2.2
- Bump minimum requirements to `foobar2000` `2.24` and `Windows 10`.
- Compiled with the latest `foobar2000` `SDK`.

### 2.1.6
- This  fixes an issue where bulk operations spammed `foobar2000` with
notifications to update playlists/library viewers after every web request
which is not good practice. It now happens once when all web requests have completed.

### 2.1.5
- Loved track import failures are now reported in the `foobar2000` `Console` by artist and
title. Previously, only a count of the successful imports was logged.

### 2.1.4
- Add main menu option to import `Last.fm` loved tracks without resetting existing loved tracks first.

### 2.1.3
- Add some new [title format fields](#available-fields).

### 2.1.2
- The minimum requirement is now `foobar2000` `2.1`.
- Compiled with the latest `foobar2000` `SDK`.

### 2.1.1
- Minor fixes.

### 2.1.0
- Large rewrite.

### 2.0.12
- Database tweaks.

### 2.0.11
- Attempts have been made to possibly fix a multi-threading issue on startup.

### 2.0.10
- `2.0.9` was updated to use code from the latest `foobar2000` `SDK` but the loved track import would crash on `foobar2000` `2.0 Beta 17` and earlier. The change has been reverted so it works on any version.

### 2.0.9
- Compiled with the latest `foobar2000` `SDK`.

### 2.0.8
- Compiled with the latest `foobar2000` `SDK`.
- Import / export improvements.

### 2.0.7
- Fix `Library` menu bug which prevented import/export of data when the `Preferences` were not enabled.

### 2.0.6
- Add extra sanity check when reading metadata to prevent crashes.

### 2.0.5
- Allow users to fetch playcounts with their recent listening history hidden. Requires authorisation.

### 2.0.4
- Adds data import/export as `JSON` from the `Library` menu.

### 2.0.1 - 2.0.3
- Various bug fixes.

### 2.0.0
- Initial release
