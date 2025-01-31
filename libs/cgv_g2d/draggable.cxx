#include "draggable.h"

namespace cgv {
namespace g2d {

draggable::draggable() : constraint_reference(CR_FULL_SIZE) {}

void draggable::set_constraint(const irect* area) {
	constraint = area;
}

const irect* draggable::get_constraint() const {
	return constraint;
}

void draggable::apply_constraint() {
	if(constraint)
		apply_constraint(*constraint);
}

void draggable::apply_constraint(const irect& area) {
	vec2 min_pnt = vec2(area.box.get_min_pnt());
	vec2 max_pnt = vec2(area.box.get_max_pnt());

	vec2 s = size;

	if(position_is_center)
		s *= 0.5f;

	switch(constraint_reference) {
	case CR_MIN_POINT:
		if(position_is_center) {
			min_pnt += s;
			max_pnt += s;
		}
		break;
	case CR_MAX_POINT:
		min_pnt -= s;
		max_pnt -= s;
		break;
	case CR_FULL_SIZE:
		if(position_is_center)
			min_pnt += s;
		max_pnt -= s;
		break;
	case CR_CENTER:
		if(!position_is_center) {
			min_pnt -= 0.5f*s;
			max_pnt -= 0.5f*s;
		}
	default:
		break;
	}

	pos = cgv::math::clamp(pos, min_pnt, max_pnt);
}

}
}
