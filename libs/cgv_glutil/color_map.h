#pragma once

#include <cgv/render/context.h>
#include <cgv/render/render_types.h>
#include <cgv/render/texture.h>

#include "piecewise_linear_interpolator.h"

//#include "lib_begin.h"

namespace cgv {
namespace glutil{

class color_map : public cgv::render::render_types {
protected:
	piecewise_linear_interpolator<rgb> color_interpolator;
	piecewise_linear_interpolator<float> opacity_interpolator;

public:
	void clear() {
		color_interpolator.clear();
		opacity_interpolator.clear();
	}

	void add_color_point(float t, rgb color) {
		// TODO: do we need to keep it in the interval [0,1]?
		// make sure t is in the interval [0,1]
		//t = cgv::math::clamp(t, 0.0f, 1.0f);
		color_interpolator.add_control_point(t, color);
	}

	void add_opacity_point(float t, float opacity) {
		opacity_interpolator.add_control_point(t, opacity);
	}

	const std::vector<decltype(color_interpolator)::control_point>& ref_color_points() { return color_interpolator.ref_control_points(); }
	const std::vector<decltype(opacity_interpolator)::control_point>& ref_opacity_points() { return opacity_interpolator.ref_control_points(); }

	rgb interpolate_color(float t) const {

		return color_interpolator.interpolate(t);
	}

	float interpolate_opacity(float t) const {

		return opacity_interpolator.interpolate(t);
	}

	rgba interpolate(float t) const {

		rgb color = color_interpolator.interpolate(t);
		float opacity = opacity_interpolator.interpolate(t);

		return rgba(color.R(), color.G(), color.B(), opacity);
	}
};

class gl_color_map : public color_map {
protected:
	cgv::render::texture tex;
	unsigned res = 256u;

public:
	gl_color_map() {}

	gl_color_map(unsigned resolution) : gl_color_map() {
		res = std::max(resolution, 2u);
	}
	
	~gl_color_map() {
		clear();
	}

	void clear() {
		color_map::clear();
		tex.clear();
	}

	bool destruct(cgv::render::context& ctx) {
		if(tex.is_created())
			return tex.destruct(ctx);
		return true;
	}

	bool init(cgv::render::context& ctx) {
		std::vector<uint8_t> data(3 * res);

		tex.destruct(ctx);
		cgv::data::data_view dv = cgv::data::data_view(new cgv::data::data_format(res, TI_UINT8, cgv::data::CF_RGB), data.data());
		tex = cgv::render::texture("uint8[R,G,B]", cgv::render::TF_LINEAR, cgv::render::TF_LINEAR);
		return tex.create(ctx, dv, 0);
	}

	void generate_texture(cgv::render::context& ctx) {
		std::vector<rgb> color_data(res);

		float step = 1.0f / static_cast<float>(res - 1);

		for(unsigned i = 0; i < res; ++i) {
			float t = i * step;
			color_data[i] = interpolate_color(t);
		}

		std::vector<uint8_t> color_data_8(3 * color_data.size());
		for(unsigned i = 0; i < color_data.size(); ++i) {
			rgba col = color_data[i];
			color_data_8[3 * i + 0] = static_cast<uint8_t>(255.0f * col.R());
			color_data_8[3 * i + 1] = static_cast<uint8_t>(255.0f * col.G());
			color_data_8[3 * i + 2] = static_cast<uint8_t>(255.0f * col.B());
		}

		cgv::data::data_view dv = cgv::data::data_view(new cgv::data::data_format(res, TI_UINT8, cgv::data::CF_RGB), color_data_8.data());

		unsigned width = tex.get_width();
		
		if(tex.is_created() && width == res) {
			tex.replace(ctx, 0, dv);
		} else {
			tex.destruct(ctx);
			tex = cgv::render::texture("uint8[R,G,B]", cgv::render::TF_LINEAR, cgv::render::TF_LINEAR);
			tex.create(ctx, dv, 0);
		}
	}
};

}
}

//#include <cgv/config/lib_end.h>
