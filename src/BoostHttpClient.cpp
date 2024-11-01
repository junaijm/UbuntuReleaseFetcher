#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ssl.hpp>

#include <sstream>

#include "BoostHttpClient.h"
#include "ILogger.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;

/// <summary>
/// Constructor
/// </summary>
/// <param name="logger">logger instance to be used for diagnostic logging</param>
BoostHttpClient::BoostHttpClient(std::shared_ptr<ILogger> logger) : Logger(logger)
{
}

/// <summary>
/// Destructor
/// </summary>
BoostHttpClient::~BoostHttpClient()
{
}

/// <summary>
/// Function to download remote file using HTTP::GET
/// This function returns downloaded file data in chunks to caller via callback method.
/// It is up to the caller how to use the file data (either store it disk or process it in memory)
/// 
/// Caller should expects multiple calls to callback function.
/// 
/// </summary>
/// <param name="hostName">remote host where file is stored</param>
/// <param name="remotePath">full path to the file to be downloaded</param>
/// <param name="dataCallback">function to be used for callback(filedata)</param>
/// <returns></returns>
bool BoostHttpClient::DownloadFile(const std::string& hostName,
                                   const std::string& remotePath,
                                   std::function<bool(const std::string&, const size_t)> dataCallback)
{
    bool downloadStatus = false;
    try
    {
        asio::io_context ioContext;
        asio::ip::tcp::resolver resolver(ioContext);
        auto const endPoints = resolver.resolve(hostName, "443"); // "443" is the service code for SSL.

        // Set up SSL context and secure connection
        asio::ssl::context sslContext(asio::ssl::context::sslv23);
        sslContext.set_default_verify_paths();
        asio::ssl::stream<beast::tcp_stream> stream(ioContext, sslContext);

        // Set SNI hostname
        if (!SSL_set_tlsext_host_name(stream.native_handle(), hostName.c_str()))
        {
            Logger->LogError("SSL_set_tlsext_host_name failed");
            return false;
        }

        // Connect to the server
        beast::get_lowest_layer(stream).connect(endPoints);
        stream.handshake(asio::ssl::stream_base::client);

        // Create and send the HTTP GET request
        http::request<http::string_body> httpRequest{ http::verb::get, remotePath, 11 }; // 11 stands for HTTP/1.1
        httpRequest.set(http::field::host, hostName);
        httpRequest.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        http::write(stream, httpRequest);

        // Prepare for response reading in chunks
        beast::flat_buffer buffer;
        http::response_parser<http::dynamic_body> responseParser;
        responseParser.body_limit(boost::none);

        // Read the response body in chunks and send it to caller as callback.
        const int PARSER_BUFFER_SIZE = 1024 * 1024; // 1 MB
        while (!responseParser.is_done())
        {
            responseParser.get().body().prepare(PARSER_BUFFER_SIZE);

            if (0 < http::read_some(stream, buffer, responseParser))
            {
                std::string responseData = beast::buffers_to_string(responseParser.get().body().data());

                // Invoke data callback.
                if (dataCallback)
                {
                    dataCallback(responseData, responseData.size());
                }
                else
                {
                    Logger->LogWarning("Callback not specified. Discarding read data");
                }
            }

            // Clear the parser buffer for next chunk of data.
            responseParser.get().body().clear();
        }

        // Gracefully close the SSL stream
        beast::error_code errorCode;
        stream.shutdown(errorCode);
        if (boost::system::errc::success != errorCode && asio::error::eof != errorCode)
        {
            std::stringstream logData;
            logData << "Something went wrong during Shutdown. See error code : " << errorCode;
            Logger->LogError(logData.str());
            return false;
        }

        return true; // Download successful
    }
    catch (const std::exception& e)
    {
        Logger->LogError("Exception caught in DownloadFile.\n" + std::string(e.what()));
    }
    catch (...)
    {
        Logger->LogError("Unknown exception caught in DownloadFile.");
    }

    return false;
}
