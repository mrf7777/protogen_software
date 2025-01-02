#ifndef PROTOGEN_ENDPOINTS_HPP
#define PROTOGEN_ENDPOINTS_HPP

#include <functional>
#include <string>
#include <unordered_map>

#include <httplib.h>

namespace protogen {

enum class HttpMethod {
    Get, Post, Put, Delete, Options, Patch,
};
/**
 * A relative path and http method to access some handler.
 * 
 * This is used to abstract the dedicated URL prefixes for each
 * plugin such as apps, sensors, render surfaces, etc.
 */
struct Endpoint {
    HttpMethod method;
    std::string relativePath;
    bool operator==(const Endpoint& other) const { 
        return (method == other.method
        && relativePath == other.relativePath);
    }
};
/**
 * A function to handle an endpoint being called.
 */
using EndpointHandler = std::function<void(const httplib::Request&, httplib::Response&)>;
/**
 * A mapping from endpoints to handlers.
 */
using Endpoints = std::unordered_map<Endpoint, EndpointHandler>;

} // namespace

template<>
struct std::hash<protogen::Endpoint> {
    std::size_t operator()(const protogen::Endpoint& x) const noexcept {
        const std::size_t path_hash = std::hash<std::string>()(x.relativePath);
        const std::size_t method_hash = std::hash<int>()(static_cast<int>(x.method));
        return path_hash ^ method_hash;
    }
};

#endif // PROTOGEN_ENDPOINTS_HPP