# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

```bash
# Configure (Release is default)
cmake .

# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug .

# Build
make

# Install
sudo make install

# Clean build artifacts
make clean && rm -rf CMakeFiles/ CMakeCache.txt
```

**macOS OpenSSL configuration:**
```bash
# Intel Mac
cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DOPENSSL_LIBRARIES=/usr/local/opt/openssl/lib .

# Apple Silicon
cmake -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl -DOPENSSL_LIBRARIES=/opt/homebrew/opt/openssl/lib .
```

**Build options:**
| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | (empty → Release) | Release (optimized) or Debug (with symbols) |
| `HAVE_SSL` | ON | Enable HTTPS support (requires OpenSSL) |
| `MYTGET_DEBUG_LOG` | OFF | Enable verbose debug logging |
| `MYTGET_STATIC` | OFF | Link statically for packaging |

**Versioning:** The project version is defined in the top-level `CMakeLists.txt` via `project(mytget VERSION X.XX LANGUAGES C CXX)`. This is substituted into `src/version.h` at build time from `src/version.h.in`. Do not edit `src/version.h` directly - it's generated and gitignored.

## Architecture

Mytget is a multi-threaded download accelerator using a plugin-based protocol architecture.

### Core Components

**Entry point:** `src/mytget.cpp` - parses CLI arguments, initializes SSL, and spawns `Downloader` instances for each URL.

**Task model:** `Task` class holds all download configuration (URL, proxy settings, retry/timeout values, output paths, thread count, file size, resume capability).

**Plugin system:** Abstract `Plugin` base class with protocol-specific implementations:
- `HttpPlugin` - handles HTTP/HTTPS downloads with range request support
- `FtpPlugin` - handles FTP downloads with directory listing and recursive retrieval

Plugin interface:
- `get_info(Task*)` - retrieves remote file size and determines if directory/resume is supported
- `download(Task&, Block*)` - downloads a byte range into a block
- `get_dir_list(Task&, const char*)` - fetches directory contents (FTP only)

**Download orchestration:** `Downloader` class manages the entire download workflow:
1. Initializes appropriate plugin based on URL protocol
2. Calls `plugin->get_info()` to determine file size and capabilities
3. Divides file into `Block` objects based on thread count
4. Spawns pthreads via `thread_create()` - each runs `download_thread()` static method
5. Coordinates thread state (STOP, RETR, WAIT, WAKEUP, EXIT, JOINED) via `schedule()`
6. Writes downloaded data to output file via `Block::bufferFile` (which wraps file I/O)
7. On completion, removes the `.mg!` temp file used for resume state

**Thread model:** Each download thread has a `Block` object containing:
- `pthread_t pid` - thread handle
- `ThreadState state` - current state in scheduling loop
- `BufferFile bufferFile` - handles writing to output file at correct offset
- `startPoint`, `downloaded`, `size` - byte range for this thread

**File resume:** If a download is interrupted, a `.mg!` file in the output directory stores progress. On retry, `init_threads_from_mg()` restores thread positions from this file.

**Proxy support:** HTTP GET proxy is supported. Proxy config comes from `-x` option or environment variables (`http_proxy`, `HTTP_PROXY`, `proxy`). Only HTTP proxies are supported.

### Source Organization

- `src/mytget.cpp` - main entry point, CLI parsing
- `src/downloader.{h,cpp}` - download orchestration and thread management
- `src/plugin.{h,cpp}` - abstract plugin interface
- `src/httpplugin.{h,cpp}` - HTTP/HTTPS protocol implementation
- `src/ftpplugin.{h,cpp}` - FTP protocol implementation
- `src/task.{h,cpp}` - download task configuration
- `src/block.{h,cpp}` - per-thread download state
- `src/http.{h,cpp}`, `src/ftp.{h,cpp}` - protocol helpers
- `src/url.{h,cpp}` - URL parsing
- `src/proxy.{h,cpp}` - proxy configuration
- `src/advio.{h,cpp}` - buffered file I/O
- `src/tcp.{h,cpp}` - TCP connection management
- `src/header.{h,cpp}` - HTTP header parsing
- `src/progressbar.{h,cpp}` - terminal progress display
- `src/ftpparser.{h,cpp}` - FTP directory listing parser
- `src/utils.{h,cpp}` - utility functions
- `src/debug.{h,cpp}` - debug logging (use `-b` flag to enable runtime)
