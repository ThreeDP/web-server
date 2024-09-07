#ifndef __EHTTP_STATUS_CODE_HPP__
# define __EHTTP_STATUS_CODE_HPP__

namespace HttpStatusCode {
    enum Code {
        _DO_NOTHING = 0,
        _CGI = 1,
        _OK = 200,
        _CREATED = 201,
        _ACCEPTED = 202,
        _NO_CONTENT = 204,
        _MUILTIPLE_CHOICE = 300,
        _MOVED_PERMANENTLY = 301,
        _FOUND = 302,
        _TEMPORARY_REDIRECT = 307,
        _PERMANENT_REDIRECT = 308,
        _BAD_REQUEST = 400,
        _UNAUTHORIZED = 401,
        _FORBIDDEN = 403,
        _NOT_FOUND = 404,
        _METHOD_NOT_ALLOWED = 405,
        _CONTENT_TOO_LARGE = 413,
        _UNSUPPORTED_MEDIA_TYPE = 415,
        _INTERNAL_SERVER_ERROR = 500,
        _BAD_GATEWAY = 502,
        _GATEWAY_TIMEOUT = 504,
        _HTTP_VERSION_NOT_SUPPORTED = 505
    };
}
    

#endif