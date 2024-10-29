#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/json.hpp>

#include <fstream>
#include <iostream>

#include "BoostHttpClient.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;

bool BoostHttpClient::DownloadFile(const std::string& hostName,
                                   const std::string& remotePath,
                                   const std::string& downloadPath) 
{
    bool downloadStatus = false;
    try 
    {
        asio::io_context ioContext;
        asio::ip::tcp::resolver resolver(ioContext);
        auto const endPoints = resolver.resolve(hostName, "443");

        // Set up SSL context and secure connection
        asio::ssl::context sslContext(asio::ssl::context::sslv23);
        sslContext.set_default_verify_paths();
        asio::ssl::stream<beast::tcp_stream> stream(ioContext, sslContext);

        // Set SNI hostname
        if (!SSL_set_tlsext_host_name(stream.native_handle(), hostName.c_str())) 
        {
            std::cerr << "SSL_set_tlsext_host_name failed" << std::endl;
            return downloadStatus;
        }

        // Connect to the server
        beast::get_lowest_layer(stream).connect(endPoints);
        stream.handshake(asio::ssl::stream_base::client);

        // Create and send the HTTP GET request
        http::request<http::string_body> httpRequest{ http::verb::get, remotePath, 11 };
        httpRequest.set(http::field::host, hostName);
        httpRequest.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        http::write(stream, httpRequest);

        // Open the output file for writing
        std::ofstream outFile(downloadPath, std::ios::binary);
        if (!outFile) 
        {
            std::cerr << "Failed to open target file for writing" << std::endl;
            return downloadStatus;
        }

        // Prepare for response reading in chunks
        beast::flat_buffer buffer;
        http::response_parser<http::dynamic_body> responseParser;
        responseParser.body_limit(boost::none);

        //// Read the response headers
        //http::read_header(stream, buffer, parser);

        // Read the response body in chunks and write to file
        const int PARSER_BUFFER_SIZE = 10 * 1024; // 10 KB
        while (!responseParser.is_done())
        {
            responseParser.get().body().prepare(PARSER_BUFFER_SIZE);

            if (0 < http::read_some(stream, buffer, responseParser))
            {
                outFile << beast::buffers_to_string(responseParser.get().body().data());
            }

            // Clear the parser buffer for next chunk of data.
            responseParser.get().body().clear();
        }

        outFile.close();

        // Gracefully close the SSL stream
        beast::error_code errorCode;
        stream.shutdown(errorCode);
        if (boost::system::errc::success != errorCode && asio::error::eof != errorCode)
        {
            std::cerr << "Something went wrong during Shutdown. See error code : " << errorCode << std::endl;
            return downloadStatus;
        }

        downloadStatus = true; // Download successful
    }
    catch (const beast::system_error& se) 
    {
        std::cerr << "Boost.Beast error: " << se.what() << std::endl;
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    }
    catch (...) 
    {
        std::cerr << "Unknown error occurred" << std::endl;
    }

    return downloadStatus;
}
