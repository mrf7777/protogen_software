#ifndef PROTOGEN_WINDOW_H
#define PROTOGEN_WINDOW_H

#include <protogen/Resolution.hpp>

namespace protogen {

struct Window {
    int top_left_x;
    int top_left_y;
    Resolution size;
};

}

#endif