#include "triangle.h"
#include "line.h"

void triangle(Vec2i t0, Vec2i t1,  Vec2i t2, TGAImage &image, TGAColor color) {
    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
}