#include "vr_scene.h"
#include <cgv/base/register.h>
#include <cgv/math/ftransform.h>
#include <cgv/math/pose.h>
#include <cg_vr/vr_events.h>
#include <random>

namespace vr {

/// set the common border color of labels
void vr_scene::set_label_border_color(const rgba& border_color)
{
	rrs.default_border_color = border_color;
	update_member(&rrs.percentual_border_width);
}
/// set the common border width in percent of the minimal extent
void vr_scene::set_label_border_width(float border_width)
{
	rrs.percentual_border_width = border_width;
	update_member(&rrs.percentual_border_width);
}

void vr_scene::construct_rectangular_table(float tw, float td, float th, float tW, float tpO, rgb table_clr, rgb leg_clr) 
{
	float tO = tpO * tw;
	float x0 = -0.5f * tw;
	float x1 = -0.5f * tw + tO;
	float x2 = -0.5f * tw + tO + tW;
	float x3 = 0.5f * tw - tO - tW;
	float x4 = 0.5f * tw - tO;
	float x5 = 0.5f * tw;
	float y0 = 0;
	float y1 = th - tW;
	float y2 = th;
	float z0 = -0.5f * td;
	float z1 = -0.5f * td + tO;
	float z2 = -0.5f * td + tO + tW;
	float z3 =  0.5f * td - tO - tW;
	float z4 =  0.5f * td - tO;
	float z5 =  0.5f * td;
	boxes.push_back(box3(vec3(x0, y1, z0), vec3(x5, y2, z5))); box_colors.push_back(table_clr);

	boxes.push_back(box3(vec3(x1, y0, z1), vec3(x2, y1, z2))); box_colors.push_back(leg_clr);
	boxes.push_back(box3(vec3(x3, y0, z1), vec3(x4, y1, z2))); box_colors.push_back(leg_clr);
	boxes.push_back(box3(vec3(x3, y0, z3), vec3(x4, y1, z4))); box_colors.push_back(leg_clr);
	boxes.push_back(box3(vec3(x1, y0, z3), vec3(x2, y1, z4))); box_colors.push_back(leg_clr);
}

void vr_scene::construct_round_table(float ttr, float tbr, float th, float tW, float tpO, rgb table_clr, rgb leg_clr)
{
	float r0 = 0.5f*tbr;
	float r1 = 0.5f*(1.0f-tpO)*tbr;
	float r2 = 2*tW;
	float r3 = 2*tW;
	float r4 = 0.5f*(1.0f - tpO)*ttr;
	float r5 = 0.5f*ttr;
	float y0 = 0;
	float y1 = tW;
	float y2 = 5 * tW;
	float y3 = th - 5 * tW;
	float y4 = th - tW;
	float y5 = th;
	cone_vertices.push_back(vec4(0, y0, 0, r0));
	cone_vertices.push_back(vec4(0, y1, 0, r0));
	
	cone_vertices.push_back(vec4(0, y1, 0, r1));
	cone_vertices.push_back(vec4(0, y2, 0, r2));
	
	cone_vertices.push_back(vec4(0, y2, 0, r2));
	cone_vertices.push_back(vec4(0, y3, 0, r3));

	cone_vertices.push_back(vec4(0, y3, 0, r3));
	cone_vertices.push_back(vec4(0, y4, 0, r4));
	cone_colors.push_back(table_clr);
	cone_colors.push_back(table_clr);
	for (size_t i = 2; i < cone_vertices.size(); ++i)
		cone_colors.push_back(leg_clr);
	cone_vertices.push_back(vec4(0, y4, 0, r5));
	cone_vertices.push_back(vec4(0, y5, 0, r5));
	cone_colors.push_back(table_clr);
	cone_colors.push_back(table_clr);
}

void vr_scene::construct_room(float w, float d, float h, float W, bool walls, bool ceiling) {
	// construct floor
	boxes.push_back(box3(vec3(-0.5f * w, -W, -0.5f * d), vec3(0.5f * w, 0, 0.5f * d)));
	box_colors.push_back(rgb(0.2f, 0.2f, 0.2f));

	if (walls) {
		// construct walls
		boxes.push_back(box3(vec3(-0.5f * w, -W, -0.5f * d - W), vec3(0.5f * w, h, -0.5f * d)));
		box_colors.push_back(rgb(0.8f, 0.5f, 0.5f));
		boxes.push_back(box3(vec3(-0.5f * w, -W, 0.5f * d), vec3(0.5f * w, h, 0.5f * d + W)));
		box_colors.push_back(rgb(0.8f, 0.5f, 0.5f));

		boxes.push_back(box3(vec3(0.5f * w, -W, -0.5f * d - W), vec3(0.5f * w + W, h, 0.5f * d + W)));
		box_colors.push_back(rgb(0.5f, 0.8f, 0.5f));

		boxes.push_back(box3(vec3(-0.5f * w, -W, -0.5f * d - W), vec3(-0.5f * w - W, h, 0.5f * d + W)));
		box_colors.push_back(rgb(0.5f, 0.8f, 0.5f));
	}
	if (ceiling) {
		// construct ceiling
		boxes.push_back(box3(vec3(-0.5f * w - W, h, -0.5f * d - W), vec3(0.5f * w + W, h + W, 0.5f * d + W)));
		box_colors.push_back(rgb(0.5f, 0.5f, 0.8f));
	}
}

void vr_scene::construct_environment(float s, float ew, float ed, float w, float d, float h) {
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(0, 1);
	unsigned n = unsigned(ew / s);
	unsigned m = unsigned(ed / s);
	float ox = 0.5f * float(n) * s;
	float oz = 0.5f * float(m) * s;
	for (unsigned i = 0; i < n; ++i) {
		float x = i * s - ox;
		for (unsigned j = 0; j < m; ++j) {
			float z = j * s - oz;
			if (fabsf(x) < 0.5f * w && fabsf(x + s) < 0.5f * w && fabsf(z) < 0.5f * d && fabsf(z + s) < 0.5f * d)
				continue;
			float h = 0.2f * (std::max(abs(x) - 0.5f * w, 0.0f) + std::max(abs(z) - 0.5f * d, 0.0f)) * distribution(generator) + 0.1f;
			boxes.push_back(box3(vec3(x, 0.0f, z), vec3(x + s, h, z + s)));
			rgb color = cgv::media::color<float, cgv::media::HLS>(distribution(generator), 0.1f * distribution(generator) + 0.15f, 0.3f);
			box_colors.push_back(color);
		}
	}
}

void vr_scene::build_scene(float w, float d, float h, float W)
{
	if (draw_room) {
		construct_room(w, d, h, W, false, false);
	}
	if (draw_environment) {
		construct_environment(0.3f, 3 * w, 3 * d, w, d, h);
	}
	switch (table_mode) {
	case TM_RECTANGULAR:
		construct_rectangular_table(table_width, table_depth, table_height, leg_width, percentual_leg_offset, table_color, leg_color);
		break;
	case TM_ROUND:
		construct_round_table(table_top_radius, table_bottom_radius, table_height, leg_width, percentual_leg_offset, table_color, leg_color);
		break;
	default:
		break;
	}
}

void vr_scene::clear_scene()
{
	boxes.clear();
	box_colors.clear();
	cone_vertices.clear();
	cone_colors.clear();
}

vr_scene::vr_scene() : lm(false)
{
	set_name("vr_scene");
	vr_view_ptr = 0;

	table_mode = TM_ROUND;
	draw_room = true;
	draw_environment = true;
	draw_walls = false;
	draw_ceiling = false;
	table_color = rgb(0.3f, 0.2f, 0.0f);
	table_width = 1.6f;
	table_depth = 0.8f;
	table_height = 0.7f;
	leg_color = rgb(0.2f, 0.1f, 0.1f);
	leg_width = 0.03f;
	percentual_leg_offset = 0.03f;

	rrs.map_color_to_material = cgv::render::CM_COLOR_AND_OPACITY;
	rrs.border_mode = cgv::render::RBM_MIN;
	rrs.texture_mode = cgv::render::RTM_RED_MIX_COLOR_AND_SECONDARY_COLOR;
	rrs.illumination_mode = cgv::render::IM_OFF;

	room_width = 5;
	room_depth = 7;
	room_height = 3;
	wall_width = 0.2f;
	build_scene(room_width, room_depth, room_height, wall_width);

	pixel_scale = 0.001f;

	on_set(&table_width);
}

cgv::reflect::enum_reflection_traits<TableMode> get_reflection_traits(const TableMode& tm)
{
	return cgv::reflect::enum_reflection_traits<TableMode>("HIDE,RECTANGULAR,ROUND");
}

bool vr_scene::self_reflect(cgv::reflect::reflection_handler& rh)
{
	return 		
		rh.reflect_member("table_mode", table_mode) &&
		rh.reflect_member("table_color", table_color) &&
		rh.reflect_member("table_width", table_width) &&
		rh.reflect_member("table_depth", table_depth) &&
		rh.reflect_member("table_height", table_height) &&
		rh.reflect_member("table_leg color", leg_color) &&
		rh.reflect_member("table_legs", leg_width) &&
		rh.reflect_member("percentual_leg_offset", percentual_leg_offset) &&
		rh.reflect_member("draw_room", draw_room) &&
		rh.reflect_member("room_width", room_width) &&
		rh.reflect_member("room_depth", room_depth) &&
		rh.reflect_member("room_height", room_height) &&
		rh.reflect_member("wall_width", wall_width) &&
		rh.reflect_member("draw_walls", draw_walls) &&
		rh.reflect_member("draw_ceiling", draw_ceiling) &&
		rh.reflect_member("draw_environment", draw_environment);
}

void vr_scene::on_set(void* member_ptr)
{
	if (member_ptr >= &table_width && member_ptr < &leg_color + 1) {
		switch (table_mode) {
		case TM_RECTANGULAR :
			boxes.resize(boxes.size() - 5);
			box_colors.resize(box_colors.size() - 5);
			construct_rectangular_table(table_width, table_depth, table_height, leg_width, percentual_leg_offset, table_color, leg_color);
			break;
		case TM_ROUND:
			cone_vertices.clear();
			cone_colors.clear();
			construct_round_table(table_top_radius, table_bottom_radius, table_height, leg_width, percentual_leg_offset, table_color, leg_color);
			break;
		}
	}
	if (member_ptr == &table_mode || member_ptr == &draw_room || member_ptr == &draw_environment || member_ptr == &draw_walls || member_ptr == &draw_ceiling || (member_ptr >= &room_width && member_ptr < &wall_width + 1)) {
		clear_scene();
		build_scene(room_width, room_depth, room_height, wall_width);
		if (member_ptr == &table_mode)
			update_table_labels();
	}
	update_member(member_ptr);
	post_redraw();
}

bool vr_scene::init(cgv::render::context& ctx)
{
	cgv::render::ref_sphere_renderer(ctx, 1);
	cgv::render::ref_box_renderer(ctx, 1);
	cgv::render::ref_cone_renderer(ctx, 1);
	cgv::render::ref_rectangle_renderer(ctx, 1);
	aam.init(ctx);
	cgv::gui::connect_vr_server(true);
	lm.init(ctx);
	cgv::media::font::font_ptr f = cgv::media::font::default_font(true);
	ctx.enable_font_face(f->get_font_face(cgv::media::font::FFA_BOLD), 36.0f);
	lm.set_font_face(f->get_font_face(cgv::media::font::FFA_BOLD));
	lm.set_font_size(36.0f);
	lm.set_text_color(rgba(0, 0, 0, 1));

	auto view_ptr = find_view_as_node();
	if (view_ptr) {
		// if the view points to a vr_view_interactor
		vr_view_ptr = dynamic_cast<vr_view_interactor*>(view_ptr);
		if (vr_view_ptr) {
			// configure vr event processing
			vr_view_ptr->set_event_type_flags(
				cgv::gui::VREventTypeFlags(
					cgv::gui::VRE_KEY +
					cgv::gui::VRE_ONE_AXIS +
					cgv::gui::VRE_ONE_AXIS_GENERATES_KEY +
					cgv::gui::VRE_TWO_AXES +
					cgv::gui::VRE_TWO_AXES_GENERATES_DPAD +
					cgv::gui::VRE_POSE
				));
			// vr_view_ptr->enable_vr_event_debugging(false);
			// configure vr rendering
			// vr_view_ptr->draw_action_zone(false);
			vr_view_ptr->draw_vr_kits(true);
			// vr_view_ptr->enable_blit_vr_views(true);
			// vr_view_ptr->set_blit_vr_view_width(200);
		}
	}
	return true;
}

void vr_scene::init_frame(cgv::render::context& ctx)
{
}

void vr_scene::clear(cgv::render::context& ctx)
{
	cgv::render::ref_sphere_renderer(ctx, -1);
	cgv::render::ref_box_renderer(ctx,-1);
	cgv::render::ref_cone_renderer(ctx, -1);
	aam.destruct(ctx);
	lm.destruct(ctx);
	cgv::render::ref_rectangle_renderer(ctx, -1);
}

void vr_scene::draw(cgv::render::context& ctx)
{
	bool repack = lm.is_packing_outofdate();
	lm.ensure_texture_uptodate(ctx);
	if (repack) {
		for (uint32_t li = 0; li < label_texture_ranges.size(); ++li)
			label_texture_ranges[li] = lm.get_texcoord_range(li);
	}

	if (draw_environment || draw_room || table_mode != TM_HIDE) {
		// activate render styles
		auto& br = cgv::render::ref_box_renderer(ctx);
		br.set_render_style(box_style);

		// draw static part
		br.set_box_array(ctx, boxes);
		br.set_color_array(ctx, box_colors);
		br.render(ctx, 0, boxes.size());

		if (!cone_vertices.empty()) {
			auto& cr = cgv::render::ref_cone_renderer(ctx);
			cr.set_render_style(cone_style);
			cr.set_sphere_array(ctx, cone_vertices);
			cr.set_color_array(ctx, cone_colors);
			cr.render(ctx, 0, cone_vertices.size());
		}
	}
}

/// draw transparent part here
void vr_scene::finish_frame(cgv::render::context& ctx)
{
	// compute label poses in lab coordinate system
	std::vector<vec3> P;
	std::vector<quat> Q;
	std::vector<vec2> E;
	std::vector<vec4> T;
	std::vector<rgba> C;
	mat34 ID; ID.identity();
	mat34 pose[5] = { ID, ID };
	bool valid[5] = { true, true, false, false, false };
	// update table pose
	cgv::math::pose_position(pose[CS_TABLE]) = vec3(0.0f, table_height, 0.0f);
	// extract poses from tracked vr devices
	if (vr_view_ptr) {
		const auto* cs = vr_view_ptr->get_current_vr_state();
		if (cs) {
			valid[CS_HEAD] = cs->hmd.status == vr::VRS_TRACKED;
			valid[CS_LEFT_CONTROLLER] = cs->controller[0].status == vr::VRS_TRACKED;
			valid[CS_RIGHT_CONTROLLER] = cs->controller[1].status == vr::VRS_TRACKED;
			if (valid[CS_HEAD])
				pose[CS_HEAD] = reinterpret_cast<const mat34&>(vr_view_ptr->get_current_vr_state()->hmd.pose[0]);
			if (valid[CS_LEFT_CONTROLLER])
				pose[CS_LEFT_CONTROLLER] = reinterpret_cast<const mat34&>(vr_view_ptr->get_current_vr_state()->controller[0].pose[0]);
			if (valid[CS_RIGHT_CONTROLLER])
				pose[CS_RIGHT_CONTROLLER] = reinterpret_cast<const mat34&>(vr_view_ptr->get_current_vr_state()->controller[1].pose[0]);
		}
	}
	// set poses of visible labels in valid coordinate systems
	for (uint32_t li = 0; li < label_coord_systems.size(); ++li) {
		//if (label_visibilities[li] == 0 || !valid[label_coord_systems[li]])
		//	continue;
		mat34 label_pose = cgv::math::pose_construct(label_orientations[li], label_positions[li]);
		cgv::math::pose_transform(pose[label_coord_systems[li]], label_pose);
		P.push_back(cgv::math::pose_position(label_pose));
		Q.push_back(quat(cgv::math::pose_orientation(label_pose)));
		E.push_back(label_extents[li]);
		T.push_back(label_texture_ranges[li]);
		C.push_back(lm.get_label(li).background_color);
	}
	// draw labels
	if (!P.empty()) {
		GLboolean blend = glIsEnabled(GL_BLEND); glEnable(GL_BLEND);
		GLenum blend_src, blend_dst, depth;
		glGetIntegerv(GL_BLEND_DST, reinterpret_cast<GLint*>(&blend_dst));
		glGetIntegerv(GL_BLEND_SRC, reinterpret_cast<GLint*>(&blend_src));
		glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint*>(&depth));

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto& rr = cgv::render::ref_rectangle_renderer(ctx);
		rr.set_render_style(rrs);
		rr.enable_attribute_array_manager(ctx, aam);
		rr.set_position_array(ctx, P);
		rr.set_rotation_array(ctx, Q);
		rr.set_extent_array(ctx, E);
		rr.set_texcoord_array(ctx, T);
		rr.set_color_array(ctx, C);
		rr.set_secondary_color(ctx, lm.get_text_color());
		lm.get_texture()->enable(ctx);
		rr.render(ctx, 0, P.size());
		lm.get_texture()->disable(ctx);
		rr.disable_attribute_array_manager(ctx, aam);

		if (!blend)
			glDisable(GL_BLEND);
		glBlendFunc(blend_src, blend_dst);
	}
}

void vr_scene::stream_help(std::ostream& os)
{
	os << "vr_scene: navigate scenes with direction pad left and right and save with down" << std::endl;
}

bool vr_scene::handle(cgv::gui::event& e)
{
	return false;
}

void vr_scene::update_table_labels()
{
	auto cp = find_control(table_width);
	if (cp)
		cp->set("label", table_mode == TM_ROUND ? "top_radius" : "table_width");
	cp = find_control(table_depth);
	if (cp)
		cp->set("label", table_mode == TM_ROUND ? "bottom_radius" : "table_depth");
}

void vr_scene::create_gui()
{
	add_decorator("vr_scene", "heading");
	if (begin_tree_node("room", boxes)) {
		align("\a");
		add_member_control(this, "draw room", draw_room, "check");
		add_member_control(this, "draw walls", draw_walls, "check");
		add_member_control(this, "draw ceiling", draw_ceiling, "check");
		add_member_control(this, "width", room_width, "value_slider", "min=0.1;max=20.0;ticks=true");
		add_member_control(this, "depth", room_depth, "value_slider", "min=0.1;max=20.0;ticks=true");
		add_member_control(this, "height", room_height, "value_slider", "min=0.1;max=10.0;ticks=true");
		add_member_control(this, "wall width", wall_width, "value_slider", "min=0.1;max=2.0;ticks=true");
		add_member_control(this, "draw boxes", draw_environment, "check");
		align("\b");
		end_tree_node(boxes);
	}
	if (begin_tree_node("table", table_width)) {
		align("\a");
		add_member_control(this, "table", table_mode, "dropdown", "enums='HIDE,RECTANGULAR,ROUND'");
		add_member_control(this, "color", table_color);
		add_member_control(this, "width", table_width, "value_slider", "min=0.1;max=3.0;ticks=true");
		add_member_control(this, "depth", table_depth, "value_slider", "min=0.1;max=3.0;ticks=true");
		update_table_labels();
		add_member_control(this, "height", table_height, "value_slider", "min=0.1;max=3.0;ticks=true");
		add_member_control(this, "leg color", leg_color);
		add_member_control(this, "legs", leg_width, "value_slider", "min=0.0;max=0.3;ticks=true");
		add_member_control(this, "percentual_offset", percentual_leg_offset, "value_slider", "min=0.0;max=0.5;ticks=true");
		align("\b");
		end_tree_node(table_width);
	}
	if (begin_tree_node("labels", lm)) {
		align("\a");
		for (size_t i = 0; i < label_positions.size(); ++i) {
			if (begin_tree_node(std::string("label ") + cgv::utils::to_string(i), label_positions[i])) {
				align("\a");
				add_member_control(this, "visible", (bool&)label_visibilities[i], "toggle");
				add_member_control(this, "coordinate_system", (cgv::type::DummyEnum&)label_coord_systems[i], "dropdown", "enums='lab,table,head,left controller,right controller'");
				add_gui("position", label_positions[i], "vector", "gui_type='value_slider';options='min=-2;max=2;ticks=true'");
				add_gui("orientation", (vec4&)label_orientations[i], "direction", "gui_type='value_slider';options='min=-1;max=1;ticks=true'");
				add_gui("extent", label_extents[i], "vector", "gui_type='value_slider';options='min=0;max=1;ticks=true'");
				align("\b");
				end_tree_node(label_positions[i]);
			}
		}
		if (begin_tree_node("rectangles", rrs)) {
			align("\a");
			add_gui("rrs", rrs);
			align("\b");
			end_tree_node(rrs);
		}
		align("\b");
		end_tree_node(lm);
	}
}

}
#include <cgv/base/register.h>

cgv::base::object_registration<vr::vr_scene> vr_scene_reg("vr_scene");
