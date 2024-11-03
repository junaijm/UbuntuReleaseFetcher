# UbuntuReleaseFetcher

**UbuntuReleaseFetcher** is a utility to fetch detailed information about Ubuntu releases.

## Key Features

- **Fetch Supported Ubuntu Releases**: Retrieves all Ubuntu releases for a specified architecture that are currently in support.
- **Get Current LTS Release**: Queries the current LTS (Long-Term Support) release for a specified architecture.
- **Retrieve File Checksum**: Obtains the SHA-256 checksum of a specific package file (`disk1.img`) for a given release.

---

## Design Overview

### Class Diagram

The diagram below outlines the main components of the utility:

![UbuntuReleaseFetcher Class Diagram](https://github.com/user-attachments/assets/f475d271-ad09-469a-8db3-e4c355658a82)

### Core Components

- **Main**: The entry point of the application, handling user requests via command-line input. `Main` uses `UbuntuReleaseFetcher` to provide the required information.
  
- **UbuntuReleaseFetcher**: Implements `IReleaseFetcher` as the primary fetcher of Ubuntu release data. It depends on `IHttpClient` for HTTP requests, `UbuntuReleaseInfo` for structuring data, and `ILogger` for diagnostic logging.

- **UbuntuReleaseInfo**: A data model to hold release information, leveraging Boost's JSON library for parsing the data.

- **BoostHttpClient**: Implements `IHttpClient`, uses Boost.Beast library to fetch release information from a remote server via HTTP GET.

- **FileLogger**: Implements `ILogger`, handling diagnostic logs written to a file.

---

## Build and Installation Guide

### Prerequisites

Ensure the following tools are installed on your system:

1. **CMake**
2. **Visual Studio** _(Windows only)_ or **g++, build-essential** _(Linux/Mac)_
3. **Git**
4. **OpenSSL**

### Building the Project

1. **Clone the Repository**

   ```
   git clone https://github.com/junaijm/UbuntuReleaseFetcher.git
   ```
2. **Create a build directory**
   ```
   cd UbuntuReleaseFetcher
   mkdir build
   cd build
   ```
3. **Set OpenSSL Root**
   
   **Windows**
   ```
   set OPENSSL_ROOT=<OpenSSL root folder>
   ```
   ***Linux/Mac***
   ```
   export OPENSSL_ROOT=<OpenSSL root folder>
   ```
5. **Generate build files and execute build**
   ```
   cmake ..
   cmake --build .
   ```
### Run the tests
   Change directory to ``<root>/bin`` and execute test suit executable (``UbuntuReleaseFetcherTest``)
   
   **Windows**
   ```
   cd ../bin
   UbuntuReleaseFetcherTest.exe
   ```
   **Linux/Mac**
   ```
   cd ../bin
   ./UbuntuReleaseFetcherTest
   ```
