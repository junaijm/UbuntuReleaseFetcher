# UbuntuReleaseFetcher
Utility to fetch information about Ubuntu releases

# Key functions:
 - Query all Ubuntu releases for a given architecture which are currently in support
 - Query current LTS release for a given architecture
 - Query checksum (sha256) of a package file (disk1.img) for a given release

# Design approach:
Below class diagram dipicts important classes. 

![image](https://github.com/user-attachments/assets/f475d271-ad09-469a-8db3-e4c355658a82)

**Main:**
Acts as entry point and handles the user requests via command-line input. Main uses UbuntuReleaseFetcher to fetch the required information. 

**UbuntuReleaseFetcher:**
Implements IReleaseFetcher and serves as the primary fetcher of Ubuntu release data. It depends on both IHttpClient and UbuntuReleaseInfo for fetching and structuring data. It also depends on ILogger for logging diagnostic data.

**UbuntuReleaseInfo:**
Acts as a data structure or model to hold release information. It uses Boost-json for structuring the data.

**BoostHttpClient:**
Implements IHttpClient and utilizes the Boost-beast library for getting release infomation from remote host via HTTP GET.

**FileLogger:**
Implements ILogger and is responsible for logging diagnostic logs to a file.
