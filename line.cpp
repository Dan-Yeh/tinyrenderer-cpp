#include "line.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    int dx = x1 - x0;
    int dy = y1 - y0;

    // draw a denser line when line is steep (dx < dy)
    if (std::abs(dx) < std::abs(dy)) {
        steep = true;
        std::swap(x0, y0);
        std::swap(x1, y1);
        std::swap(dx, dy);
    }

    float slope = dy / (float) dx;

    int x_increment = (x1 > x0) ? 1 : -1;

    int x1_stop = x1 + x_increment;
    for (int x=x0; x != x1_stop ;x+=x_increment) {
        int y = y0 + (x - x0) * slope;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}