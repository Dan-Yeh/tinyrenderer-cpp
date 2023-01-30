#include "line.h"

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false;
    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;

    // draw a denser line when line is steep (dx < dy)
    if (std::abs(dx) < std::abs(dy)) {
        steep = true;
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        std::swap(dx, dy);
    }

    float slope = dy / (float) dx;

    int x_increment = (p1.x > p0.x) ? 1 : -1;

    int x1_stop = p1.x + x_increment;
    for (int x=p0.x; x != x1_stop ;x+=x_increment) {
        int y = p0.y + (x - p0.x) * slope;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}