#pragma once

#include <cgv/render/shader_program.h>
#include <cgv/render/context.h>

#include "generic_render_data.h"

#include "lib_begin.h"

namespace cgv {
namespace glutil{

class generic_renderer {
protected:
	std::string prog_name = "";
	cgv::render::shader_program prog;
	cgv::render::shader_define_map defines;

	bool has_indices = false;

	bool build_shader_program(const cgv::render::context& ctx, const cgv::render::shader_define_map& defines) {
		return prog.build_program(ctx, prog_name, true, defines);
	}

public:
	generic_renderer() {}
	generic_renderer(const std::string& prog_name) : prog_name(prog_name) {}

	void destruct(const cgv::render::context& ctx) {
		prog.destruct(ctx);
	}

	bool init(cgv::render::context& ctx) {
		return build_shader_program(ctx, defines);
	}

	bool set_shader_defines(const cgv::render::context& ctx, const cgv::render::shader_define_map& defines) {
		if(prog.is_created())
			prog.destruct(ctx);
		return build_shader_program(ctx, defines);
	}

	cgv::render::shader_program& ref_prog() {
		assert(("generic_renderer::ref_prog shader program is not created; call init before first use", prog.is_created()));
		return prog;
	}

	cgv::render::shader_program& enable_prog(cgv::render::context& ctx) {
		assert(("generic_renderer::enable_prog shader program is not created; call init before first use", prog.is_created()));
		prog.enable(ctx);
		return prog;
	}

	bool enable(cgv::render::context& ctx, generic_render_data& geometry) {
		bool res = prog.is_enabled() ? true : prog.enable(ctx);
		res &= geometry.enable(ctx, prog);
		has_indices = geometry.has_indices();
		return res;
	}

	bool disable(cgv::render::context& ctx, generic_render_data& geometry) {
		bool res = geometry.disable(ctx);
		res &= prog.disable(ctx);
		has_indices = false;
		return res;
	}

	void draw(cgv::render::context& ctx, cgv::render::PrimitiveType type, size_t start, size_t count) {
		GLenum pt = cgv::render::gl::map_to_gl(type);
		if(has_indices)
			glDrawElements(pt, (GLsizei)count, GL_UNSIGNED_INT, (void*)(start * sizeof(unsigned)));
		else
			glDrawArrays(pt, (GLint)start, (GLsizei)count);
	}

	bool render(cgv::render::context& ctx, cgv::render::PrimitiveType type, generic_render_data& geometry, size_t start = 0, size_t count = 0) {
		if(!enable(ctx, geometry))
			return false;
		draw(ctx, type, start, count ? count : geometry.get_render_count());
		return disable(ctx, geometry);
	}
};

}
}

#include <cgv/config/lib_end.h>
