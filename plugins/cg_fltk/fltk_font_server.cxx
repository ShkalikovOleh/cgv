#include "fltk_font_server.h"
#include <fltk/run.h>
#include <fltk/draw.h>
#include <fltk/Font.h>
#include <fltk/gl.h>

/// construct from attributes
fltk_font_face::fltk_font_face(fltk::Font* _f, int _ffa) : f(_f), font_face(_ffa)
{
}

/// virtual destructor for ref_ptr support
fltk_font_face::~fltk_font_face()
{
//	delete f;
}
/// enumerate the supported sizes
void fltk_font_face::enumerate_sizes(std::vector<int>& supported_sizes) const
{
	int* ss;
	int n = f->sizes(ss);
	supported_sizes.resize(n);
	std::copy(ss, ss+n, supported_sizes.begin());
}
/// return the width of a text printed in the given size, which is measured in pixels
float fltk_font_face::measure_text_width(const std::string& text, float font_size) const
{
	fltk::setfont(f, font_size);
	return fltk::getwidth(text.c_str());
}

/// enables font face of given size and should be called once before calling draw_text functions
void fltk_font_face::enable(void* context_ptr, float _font_size)
{
	fltk::glsetfont(get_fltk_font(), _font_size);
}

/// draw text at given location with font_face implementation dependent rendering api and advance location
void fltk_font_face::draw_text(float& x, float& y, const std::string& text) const
{
	if (text.empty())
		return;
	int cursor_x = int(x);
	int cursor_y = int(y);
	glRasterPos2i(cursor_x, cursor_y);
	GLint r_prev[4];
	glGetIntegerv(GL_CURRENT_RASTER_POSITION, r_prev);
	fltk::gldrawtext(text.c_str(), (int)text.size());
	GLint r[4];
	glGetIntegerv(GL_CURRENT_RASTER_POSITION, r);
	cursor_x += r[0] - r_prev[0];
	cursor_y -= r[1] - r_prev[1];
	x = (float)cursor_x;
	y = (float)cursor_y;
}

///
fltk::Font* fltk_font_face::get_fltk_font() const
{
	return f;
}


/// 
fltk_font::fltk_font(fltk::Font* _f, int _ffa) : f(_f), ffa(_ffa) {}
/// virtual destructor for ref_ptr support
fltk_font::~fltk_font()
{
//	delete f;
}
/// return the name of the font
const char* fltk_font::get_name() const
{
	return f->name();
}
/// check whether the given font includes a face that include the possibly or-ed together selection of font face attributes
bool fltk_font::supports_font_face(int _ffa) const
{
	return (ffa & _ffa) == _ffa;
}
/// enumerate the supported font sizes
void fltk_font::enumerate_sizes(std::vector<int>& supported_sizes) const
{
	int* ss;
	int n = f->sizes(ss);
	supported_sizes.resize(n);
	std::copy(ss, ss+n, supported_sizes.begin());
}
/// return a pointer to a font face
font_face_ptr fltk_font::get_font_face(int _ffa) const
{
	if (!supports_font_face(_ffa))
		return font_face_ptr();
	int fltk_ffa = 0;
	if ( (_ffa & FFA_BOLD) != 0)
		fltk_ffa += fltk::BOLD;
	if ( (_ffa & FFA_ITALIC) != 0)
		fltk_ffa += fltk::ITALIC;

	return font_face_ptr(new fltk_font_face(f->plus(fltk_ffa), (FontFaceAttributes)_ffa));
}

void fltk_font_server::on_register()
{
	if (get_font_server().empty())
		register_font_server(this);
}

/// return "fltk_font_server"
std::string fltk_font_server::get_type_name() const
{
	return "fltk_font_server";
}

/// find an installed font by name
font_ptr fltk_font_server::find_font(const std::string& font_name)
{
	fltk::Font* f = fltk::font(font_name.c_str(),0);
	if (!f)
		return font_ptr();
	f->name();
	int ffa = 0;
	if (f->bold() != 0)
		ffa += FFA_BOLD;
	if (f->italic() != 0)
		ffa += FFA_ITALIC;
	return font_ptr(new fltk_font(f,ffa));
}

/// enumerate the names of all installed fonts
void fltk_font_server::enumerate_font_names(std::vector<const char*>& font_names)
{
	fltk::Font** fs;
	int n = fltk::list_fonts(fs);
	font_names.resize(n);
	for (int i=0; i<n; ++i)
		font_names[i] = fs[i]->name();
}

cgv::base::object_registration<fltk_font_server> font_serv("register font server");
