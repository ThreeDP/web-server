# include "RouteResponse.hpp"

std::ostream &operator<<(std::ostream &os, RouteResponse const &route) {
    os << std::endl;
    os << "\tStatus Code: " << route.StatusCode << std::endl;
    os << "\tFile Extension: " << route.FileExtension << std::endl;
    if (route.RedirectPath != "")
        os << "\tredirect path: " << route.RedirectPath << std::endl;
	return (os);
}