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
Model *model = NULL;

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
void triangle_sweeping(Vec2i t0, Vec2i t1,  Vec2i t2, TGAImage &image, TGAColor color) {
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

/*
Barycentric triangle drawing
*/

Vec3f getBaryVector(int x, int y, std::vector<Vec3f>& points) {
    Vec3f u = Vec3f(points[2].x-points[0].x, points[1].x-points[0].x, points[0].x-x)^Vec3f(points[2].y-points[0].y, points[1].y-points[0].y, points[0].y-y);
    /* `points` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u.z)<1) 
        return Vec3f(-1,1,1);
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

void triangle(std::vector<Vec3f> &points, std::vector<float> &z_buffer, TGAImage &image, TGAColor color) {
    float clampX = image.get_width() - 1;
    float clampY = image.get_height() - 1;
    Vec2f boundingboxMin(clampX, clampY);
    Vec2f boundingboxMax(0, 0);

    for (const auto& p: points) {
        boundingboxMin.x = std::max(0.f, std::min(boundingboxMin.x, p.x));
        boundingboxMin.y = std::max(0.f, std::min(boundingboxMin.y, p.y));

        boundingboxMax.x = std::min(clampX, std::max(boundingboxMax.x, p.x));
        boundingboxMax.y = std::min(clampY, std::max(boundingboxMax.y, p.y));
    }

    // go through bounding box
    for (int x=boundingboxMin.x; x <= boundingboxMax.x; x++) {
        for (int y=boundingboxMin.y; y <= boundingboxMax.y; y++) {
            Vec3f v = getBaryVector(x, y, points);
            if (v.x < 0 || v.y < 0 || v.z < 0)
                continue;
            float z = 0;
            for (int i=0; i <3; i++)
                z += points[i].raw[2] * v.raw[i];
            if (z_buffer[int(x + width*y)] < z) {
                z_buffer[int(x + width*y)] = z;
                image.set(x, y, color);
            }
        }
    }
}

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    std::vector<float> z_buffer(width*height, std::numeric_limits<float>::min());

    TGAImage image(width, height, TGAImage::RGB);

    Vec3f light_dir(0,0,-1);
    for (int i=0; i<model->nfaces(); i++) { 
        std::vector<int> face = model->face(i); 
        std::vector<Vec3f> screen_coords; 
        std::vector<Vec3f> world_coords; 
        for (int j=0; j<3; j++) { 
            Vec3f v = model->vert(face[j]); 
            screen_coords.push_back(Vec3f((v.x+1.)*width/2., (v.y+1.)*height/2., v.z)); 
            world_coords.push_back(v);
        } 

        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
        n.normalize(); 
        float intensity = n*light_dir; 
        if (intensity>0) {
            triangle(screen_coords, z_buffer, image, TGAColor(intensity*255, intensity*255, intensity*255, 255)); 
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

