#include "line.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    float slope = dy / (float) dx;

    int x_increment = (x1 > x0) ? 1 : -1;

    for (int x=x0; x != x1; x+=x_increment) {
        int y = y0 + (x - x0) * slope;
        image.set(x, y, color);
    }
}