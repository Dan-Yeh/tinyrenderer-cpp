#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const int width  = 200;
const int height = 200;

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

/*
Draw a triangle using line sweeping approach
steps:
1. sort t0, t1 and t2 based on x
2. get three slopes
3. draw lines
*/
void triangle(Vec2i t0, Vec2i t1,  Vec2i t2, TGAImage &image, TGAColor color) {
    if (t1.x < t0.x) {
        std::swap(t0, t1);
    }

    if (t2.x < t0.x) {
        std::swap(t0, t2);
    }

    if (t2.x < t1.x) {
        std::swap(t1, t2);
    }
    // Currently, t0.x <= t1.x <= t2.x
    
    // get slopes
    float slope1 = (t1.y - t0.y) / (float) (t1.x - t0.x);
    float slope2 = (t2.y - t0.y) / (float) (t2.x - t0.x);
    float slope3 = (t2.y - t1.y) / (float) (t2.x - t1.x);

    // draw lines between l1, l2
    for (int x=t0.x; x<=t1.x; x++) {
        int y1 = t0.y + (x - t0.x) * slope1;
        int y2 = t0.y + (x - t0.x) * slope2;
        line(x, y1, x, y2, image, color);
    }

    // draw lines between l2, l3
    for (int x=t1.x; x<=t2.x; x++) {
        int y2 = t0.y + (x - t0.x) * slope2;
        int y3 = t1.y + (x - t1.x) * slope3;
        line(x, y2, x, y3, image, color);
    }

}

int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);

    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);


    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

