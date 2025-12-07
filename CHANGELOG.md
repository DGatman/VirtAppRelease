# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased] - 2023-10-27

### Added
- **Input Passthrough:** Added dedicated thread in `main.cpp` to forward physical mouse/keyboard input immediately.
  - Solves "frozen mouse" issue when bot is running but not active.
  - Removes dependency on Task Manager for input unblocking.
- **Build System:** Established CMake build workflow in `build_final`.

### Changed
- **Time Sync:** Optimized `scripts/set_local_time.py`.
  - Reduced `MAX_GLOBAL_RETRIES` from 10 to 2.
  - Reduced `RETRY_DELAY` from 3s to 1s.
  - Startup time significantly improved (from ~5-10 mins to <30s).

### Fixed
- **Compilation:** Fixed build environment issues by locating correct CMake path in Visual Studio 2022.
