#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
	TGAImage diffusemap{};
	void load_texture(const std::string filename, const std::string suffix, TGAImage &img);
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
	const TGAImage& diffuse()  const { return diffusemap;  }
};

#endif //__MODEL_H__
