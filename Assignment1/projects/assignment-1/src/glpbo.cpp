/*!
@file       glpbo.cpp
@author     pghali@digipen.edu
@co-author  parminder.singh@digipen.edu
@co-author  c.chiow@digipen.edu
@date       10/11/2016
@modified   24/06/2026

This file implements the GLPbo structure for Assignment 1.

It builds on the Tutorial 6 PBO pipeline (map PBO -> fill pixels -> upload
to texture -> render full-window quad) and adds a CPU-side software
rasterizer capable of rendering OBJ meshes in four render modes:
  0 - wireframe, all edges black
  1 - wireframe, each edge a random colour
  2 - flat shaded triangles (random colour per triangle)
  3 - smooth shaded triangles (barycentric interpolation of per-vertex
      normal-derived colours)

New functions added for Assignment 1:
  - viewport_xform      : z-rotation + NDC-to-window mapping
  - set_pixel           : scissored single-pixel write to PBO
  - render_linebresenham: integer-only Bresenham line rasterizer
  - render_triangle     : flat-shaded variant (edge equations + top-left)
  - render_triangle     : smooth-shaded variant (barycentric interpolation)
  - painter_mode        : interactive line-drawing with mouse

Changes to Tutorial 6 functions:
  - init         : loads scene file, parses OBJ models via DPML
  - emulate      : dispatches per-frame rendering based on render_mode /
                   is_painter_mode; handles viewport transform each frame
  - draw_fullwindow_quad : extended title-bar output (model stats)
  - cleanup      : no structural change

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glpbo.h>
#include <glhelper.h>
#include <hook_csd2101.h>
#include <dpml.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>   // std::rand, std::srand
#include <cstring>   // std::memcpy (painter_canvas restore)
#include <ctime>     // std::time
#include <vector>
#include <string>

/*                                               static data member definitions
----------------------------------------------------------------------------- */
GLsizei            GLPbo::width = 0;
GLsizei            GLPbo::height = 0;
GLsizei            GLPbo::pixel_cnt = 0;
GLsizei            GLPbo::byte_cnt = 0;
GLPbo::Color* GLPbo::ptr_to_pbo = nullptr;
GLuint             GLPbo::vaoid = 0;
GLuint             GLPbo::elem_cnt = 0;
GLuint             GLPbo::pboid = 0;
GLuint             GLPbo::texid = 0;
GLSLShader         GLPbo::shdr_pgm;
GLPbo::Color       GLPbo::clear_clr;

std::vector<GLPbo::Model> GLPbo::models;
int                GLPbo::mdl_ref = 0;
int                GLPbo::render_mode = 0;
float              GLPbo::rotate_angle = 0.f;
int                GLPbo::culled_cnt = 0;
bool               GLPbo::is_painter_mode = false;
bool               GLPbo::painter_dragging = false;
glm::ivec2         GLPbo::painter_start = { 0, 0 };
glm::ivec2         GLPbo::painter_end = { 0, 0 };
glm::ivec2         GLPbo::painter_prev = { 0, 0 };
std::vector<GLPbo::Color> GLPbo::painter_canvas;

/*  _________________________________________________________________________ */
/* Helpers                                                                     */
/*  _________________________________________________________________________ */

/*!
@brief  Generates a random GLubyte in [0, 255].
*/
static GLubyte rand_byte()
{
    return static_cast<GLubyte>(std::rand() % 256);
}

/*!
@brief  Generates a random glm::vec3 with components in [0, 1].
*/
static glm::vec3 rand_color()
{
    return glm::vec3(
        static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX),
        static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX),
        static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
}

/*  _________________________________________________________________________ */
/* Clear-colour helpers                                                        */
/*  _________________________________________________________________________ */

/*!
@brief  Sets GLPbo::clear_clr from a Color value.
*/
void GLPbo::set_clear_color(GLPbo::Color c)
{
    clear_clr = c;
}

/*!
@brief  Sets GLPbo::clear_clr from individual RGBA byte components.
*/
void GLPbo::set_clear_color(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    clear_clr = Color(r, g, b, a);
}

/*!
@brief  Fills the entire PBO data store with GLPbo::clear_clr.
*/
void GLPbo::clear_color_buffer()
{
    std::fill(ptr_to_pbo, ptr_to_pbo + pixel_cnt, clear_clr);
}

/*  _________________________________________________________________________ */
/* set_pixel                                                                   */
/*  _________________________________________________________________________ */

/*!
@brief  Writes a single RGBA pixel into the mapped PBO at window coordinate
        (x, y).  Coordinate (0,0) is bottom-left.  Scissored to window.

@param  x    Horizontal pixel coordinate
@param  y    Vertical   pixel coordinate  (0 = bottom)
@param  clr  RGBA colour to write
*/
void GLPbo::set_pixel(GLint x, GLint y, GLPbo::Color clr)
{
    if (!ptr_to_pbo)                            return;
    if (x < 0 || x >= width)                   return;
    if (y < 0 || y >= height)                  return;
    ptr_to_pbo[y * width + x] = clr;
}

/*  _________________________________________________________________________ */
/* Bresenham line rasterizer                                                   */
/*  _________________________________________________________________________ */

/*!
@brief  Rasterizes the line segment from (px0,py0) to (px1,py1) using
        Bresenham's integer-only algorithm.  All arithmetic is strictly
        integer; no floating-point values, variables, or expressions.

        The full 8-octant generalization is handled by reflecting the
        problem into the first octant (dx >= dy >= 0, positive slope <= 1)
        and inverting the reflection per-pixel.

@param  px0, py0  Start point in window coordinates
@param  px1, py1  End   point in window coordinates
@param  draw_clr  RGBA colour for all pixels on the segment
*/
void GLPbo::render_linebresenham(GLint px0, GLint py0,
    GLint px1, GLint py1,
    GLPbo::Color draw_clr)
{
    // Work with differences; reflect into first octant
    GLint dx = px1 - px0;
    GLint dy = py1 - py0;

    // Step directions
    GLint sx = (dx > 0) ? 1 : -1;
    GLint sy = (dy > 0) ? 1 : -1;

    // Make dx, dy positive for comparisons
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    GLint x = px0;
    GLint y = py0;

    if (dx >= dy)
    {
        // Shallow slope (|slope| <= 1): step along x
        // Decision variable: 2*dy - dx  (initial error)
        GLint D = 2 * dy - dx;
        for (GLint i = 0; i <= dx; ++i)
        {
            set_pixel(x, y, draw_clr);
            if (D > 0)
            {
                y += sy;
                D += 2 * (dy - dx);
            }
            else
            {
                D += 2 * dy;
            }
            x += sx;
        }
    }
    else
    {
        // Steep slope (|slope| > 1): step along y
        GLint D = 2 * dx - dy;
        for (GLint i = 0; i <= dy; ++i)
        {
            set_pixel(x, y, draw_clr);
            if (D > 0)
            {
                x += sx;
                D += 2 * (dx - dy);
            }
            else
            {
                D += 2 * dx;
            }
            y += sy;
        }
    }
}

/*  _________________________________________________________________________ */
/* Flat-shaded triangle rasterizer                                             */
/*  _________________________________________________________________________ */

/*!
@brief  Rasterizes a flat-shaded triangle using edge equations and the
        top-left tie-breaking rule.  Fragment centres are sampled at
        (x + 0.5, y + 0.5).  Back-facing triangles (CW winding) are culled.

@param  p0, p1, p2  Triangle vertices in window coordinates (x,y used; z ignored)
@param  clr         Flat colour for all fragments  [0,1] per component
@return true if front-facing (rendered), false if back-facing (culled)
*/
bool GLPbo::render_triangle(glm::vec3 const& p0,
    glm::vec3 const& p1,
    glm::vec3 const& p2,
    glm::vec3 const& clr)
{
    // Edge vectors (integer)
    float A01 = p0.y - p1.y, B01 = p1.x - p0.x;
    float A12 = p1.y - p2.y, B12 = p2.x - p1.x;
    float A20 = p2.y - p0.y, B20 = p0.x - p2.x;

    // Signed area x2 (cross product of two edges)
    // Positive = CCW (front-facing)
    float area2 = (p1.x - p0.x) * (p2.y - p0.y)
        - (p2.x - p0.x) * (p1.y - p0.y);
    if (area2 <= 0.f) return false; // back-facing or degenerate

    // Top-left bias: an edge is a "top" edge if dy == 0 and dx < 0,
    // or a "left" edge if dy < 0.
    // For CCW winding, "top-left" means we include a fragment on a shared
    // edge only if the edge is a top or left edge.
    // Bias = 0 for top/left edges, -1 otherwise (fill convention).
    auto is_topleft = [](glm::vec3 const& a, glm::vec3 const& b) -> bool
        {
            // top edge: horizontal edge going left (y equal, x decreasing)
            bool top = (a.y == b.y) && (a.x > b.x);
            // left edge: edge going down (y decreasing)
            bool left = (a.y > b.y);
            return top || left;
        };

    float bias01 = is_topleft(p0, p1) ? 0.f : -1.f;
    float bias12 = is_topleft(p1, p2) ? 0.f : -1.f;
    float bias20 = is_topleft(p2, p0) ? 0.f : -1.f;

    // Bounding box (clamped to window)
    int xmin = static_cast<int>(std::floor(std::min({ p0.x, p1.x, p2.x })));
    int ymin = static_cast<int>(std::floor(std::min({ p0.y, p1.y, p2.y })));
    int xmax = static_cast<int>(std::ceil(std::max({ p0.x, p1.x, p2.x })));
    int ymax = static_cast<int>(std::ceil(std::max({ p0.y, p1.y, p2.y })));

    xmin = std::max(xmin, 0);
    ymin = std::max(ymin, 0);
    xmax = std::min(xmax, width - 1);
    ymax = std::min(ymax, height - 1);

    // Convert colour to 8-bit
    GLubyte cr = static_cast<GLubyte>(clr.r * 255.f);
    GLubyte cg = static_cast<GLubyte>(clr.g * 255.f);
    GLubyte cb = static_cast<GLubyte>(clr.b * 255.f);
    Color   pixel_clr(cr, cg, cb, 255);

    // Rasterize
    for (int y = ymin; y <= ymax; ++y)
    {
        for (int x = xmin; x <= xmax; ++x)
        {
            // Sample at fragment centre
            float px = static_cast<float>(x) + 0.5f;
            float py = static_cast<float>(y) + 0.5f;

            float w0 = A12 * (px - p1.x) + B12 * (py - p1.y);
            float w1 = A20 * (px - p2.x) + B20 * (py - p2.y);
            float w2 = A01 * (px - p0.x) + B01 * (py - p0.y);

            // Top-left rule: inclusive (>=0) on top/left edges, exclusive (>0) on others
            bool inside = (bias12 == 0.f ? w0 >= 0.f : w0 > 0.f)
                && (bias20 == 0.f ? w1 >= 0.f : w1 > 0.f)
                && (bias01 == 0.f ? w2 >= 0.f : w2 > 0.f);
            if (inside)
            {
                set_pixel(x, y, pixel_clr);
            }
        }
    }
    return true;
}

/*  _________________________________________________________________________ */
/* Smooth-shaded triangle rasterizer                                           */
/*  _________________________________________________________________________ */

/*!
@brief  Rasterizes a smooth-shaded triangle using edge equations, top-left
        tie-breaking rule, and barycentric interpolation of vertex colours.

@param  p0, p1, p2  Triangle vertices in window coordinates
@param  c0, c1, c2  Per-vertex RGB colours in [0,1]
@return true if front-facing, false if back-facing (culled)
*/
bool GLPbo::render_triangle(glm::vec3 const& p0,
    glm::vec3 const& p1,
    glm::vec3 const& p2,
    glm::vec3 const& c0,
    glm::vec3 const& c1,
    glm::vec3 const& c2)
{
    float A01 = p0.y - p1.y, B01 = p1.x - p0.x;
    float A12 = p1.y - p2.y, B12 = p2.x - p1.x;
    float A20 = p2.y - p0.y, B20 = p0.x - p2.x;

    float area2 = (p1.x - p0.x) * (p2.y - p0.y)
        - (p2.x - p0.x) * (p1.y - p0.y);
    if (area2 <= 0.f) return false;

    float inv_area2 = 1.f / area2;

    auto is_topleft = [](glm::vec3 const& a, glm::vec3 const& b) -> bool
        {
            bool top = (a.y == b.y) && (a.x > b.x);
            bool left = (a.y > b.y);
            return top || left;
        };

    float bias01 = is_topleft(p0, p1) ? 0.f : -1.f;
    float bias12 = is_topleft(p1, p2) ? 0.f : -1.f;
    float bias20 = is_topleft(p2, p0) ? 0.f : -1.f;

    int xmin = static_cast<int>(std::floor(std::min({ p0.x, p1.x, p2.x })));
    int ymin = static_cast<int>(std::floor(std::min({ p0.y, p1.y, p2.y })));
    int xmax = static_cast<int>(std::ceil(std::max({ p0.x, p1.x, p2.x })));
    int ymax = static_cast<int>(std::ceil(std::max({ p0.y, p1.y, p2.y })));

    xmin = std::max(xmin, 0);
    ymin = std::max(ymin, 0);
    xmax = std::min(xmax, width - 1);
    ymax = std::min(ymax, height - 1);

    for (int y = ymin; y <= ymax; ++y)
    {
        for (int x = xmin; x <= xmax; ++x)
        {
            float px = static_cast<float>(x) + 0.5f;
            float py = static_cast<float>(y) + 0.5f;

            // Edge function values (unnormalised barycentric weights)
            float w0 = A12 * (px - p1.x) + B12 * (py - p1.y);
            float w1 = A20 * (px - p2.x) + B20 * (py - p2.y);
            float w2 = A01 * (px - p0.x) + B01 * (py - p0.y);

            // Top-left rule: inclusive (>=0) on top/left edges, exclusive (>0) on others
            bool inside = (bias12 == 0.f ? w0 >= 0.f : w0 > 0.f)
                && (bias20 == 0.f ? w1 >= 0.f : w1 > 0.f)
                && (bias01 == 0.f ? w2 >= 0.f : w2 > 0.f);
            if (inside)
            {
                // Normalise to get true barycentric coordinates
                float l0 = w0 * inv_area2;
                float l1 = w1 * inv_area2;
                float l2 = w2 * inv_area2;

                glm::vec3 col = l0 * c0 + l1 * c1 + l2 * c2;

                Color pixel_clr(
                    static_cast<GLubyte>(col.r * 255.f),
                    static_cast<GLubyte>(col.g * 255.f),
                    static_cast<GLubyte>(col.b * 255.f),
                    255);
                set_pixel(x, y, pixel_clr);
            }
        }
    }
    return true;
}

/*  _________________________________________________________________________ */
/* viewport_xform                                                              */
/*  _________________________________________________________________________ */

/*!
@brief  Applies a z-axis rotation (GLPbo::rotate_angle degrees) followed by
        the viewport transform to every vertex in model.pm, storing the
        result in model.pd.

        NDC -> window mapping:
          x_d = (x_n + 1) * 0.5 * (width  - 1)
          y_d = (y_n + 1) * 0.5 * (height - 1)
          z_d = 0  (depth ignored this submission)

@param  model   Model whose pd array will be filled
*/
void GLPbo::viewport_xform(Model& model)
{
    float rad = rotate_angle * (3.14159265358979323846f / 180.f);
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    float hw = static_cast<float>(width - 1) * 0.5f;
    float hh = static_cast<float>(height - 1) * 0.5f;

    model.pd.resize(model.pm.size());
    for (std::size_t i = 0; i < model.pm.size(); ++i)
    {
        float xn = model.pm[i].x;
        float yn = model.pm[i].y;

        // z-axis rotation
        float xr = cosA * xn - sinA * yn;
        float yr = sinA * xn + cosA * yn;

        // NDC -> window
        model.pd[i] = glm::vec3(
            (xr + 1.f) * hw,
            (yr + 1.f) * hh,
            0.f);
    }
}

/*  _________________________________________________________________________ */
/* painter_mode                                                                */
/*  _________________________________________________________________________ */

/*!
@brief  Implements the interactive painter application (freehand drawing).

        Called from emulate() once per frame while is_painter_mode is true.
        All actual drawing happens directly into painter_canvas inside
        GLHelper::mousepos_cb as the mouse moves during a drag (see that
        function for the segment-chaining logic). This function's only
        job is to copy the up-to-date canvas into the mapped PBO every
        frame so it's visible on screen.

@co-author  c.chiow@digipen.edu
@modified   30/06/2026
Replaced both the original two-click protocol and the intermediate
click-drag-release-with-preview version with true freehand drawing to
match the sample executable: short segments are committed continuously
as the cursor moves, producing curved/looped strokes rather than a single
straight chord from press-point to release-point.
*/
void GLPbo::painter_mode()
{
    if (!painter_canvas.empty())
    {
        std::memcpy(ptr_to_pbo, painter_canvas.data(),
            painter_canvas.size() * sizeof(Color));
    }
}

/*  _________________________________________________________________________ */
/* setup_quad_vao                                                              */
/*  _________________________________________________________________________ */

/*!
@brief  Sets up the VAO/VBO/EBO for a full-window NDC quad (from Tutorial 6,
        unchanged).
*/
void GLPbo::setup_quad_vao()
{
    std::array<glm::vec2, 4> pos_vtx = {
        glm::vec2(-1.f, -1.f),
        glm::vec2(1.f, -1.f),
        glm::vec2(1.f,  1.f),
        glm::vec2(-1.f,  1.f)
    };
    std::array<glm::vec2, 4> tex_vtx = {
        glm::vec2(0.f, 0.f),
        glm::vec2(1.f, 0.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(0.f, 1.f)
    };
    std::array<GLushort, 6> idx_vtx = { 0, 1, 2, 2, 3, 0 };
    elem_cnt = static_cast<GLuint>(idx_vtx.size());

    GLuint vbo_hdl;
    glCreateBuffers(1, &vbo_hdl);

    GLsizeiptr pos_sz = sizeof(glm::vec2) * pos_vtx.size();
    GLsizeiptr tex_sz = sizeof(glm::vec2) * tex_vtx.size();
    GLsizeiptr elem_sz = sizeof(GLushort) * idx_vtx.size();

    glNamedBufferStorage(vbo_hdl, pos_sz + tex_sz, nullptr,
        GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(vbo_hdl, 0, pos_sz, pos_vtx.data());
    glNamedBufferSubData(vbo_hdl, pos_sz, tex_sz, tex_vtx.data());

    GLuint ebo_hdl;
    glCreateBuffers(1, &ebo_hdl);
    glNamedBufferStorage(ebo_hdl, elem_sz, idx_vtx.data(),
        GL_DYNAMIC_STORAGE_BIT);

    glCreateVertexArrays(1, &vaoid);
    glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(glm::vec2));

    glEnableVertexArrayAttrib(vaoid, 0);
    glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vaoid, 0, 0);

    glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl, pos_sz, sizeof(glm::vec2));
    glEnableVertexArrayAttrib(vaoid, 1);
    glVertexArrayAttribFormat(vaoid, 1, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vaoid, 1, 1);

    glVertexArrayElementBuffer(vaoid, ebo_hdl);
}

/*  _________________________________________________________________________ */
/* setup_shdrpgm                                                               */
/*  _________________________________________________________________________ */

/*!
@brief  Compiles, links, and validates the GLSL shader program (from
        Tutorial 6, unchanged except for automation hook).
*/
void GLPbo::setup_shdrpgm()
{
    std::vector<std::pair<GLenum, std::string>> shdr_files;

    AUTOMATION_HOOK_SHADER(shdr_pgm, shdr_files);

    if (!shdr_pgm.IsLinked())
    {
        std::string vs_src = R"(
#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 vTexCoord;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vTexCoord   = aTexCoord;
}
)";
        std::string fs_src = R"(
#version 450 core
in  vec2 vTexCoord;
uniform sampler2D uTex2d;
out vec4 fFragColor;
void main() {
    fFragColor = texture(uTex2d, vTexCoord);
}
)";
        shdr_files.push_back({ GL_VERTEX_SHADER,   vs_src });
        shdr_files.push_back({ GL_FRAGMENT_SHADER, fs_src });

        if (GL_FALSE == shdr_pgm.CompileLinkValidate(shdr_files, false))
        {
            std::cerr << "GLPbo::setup_shdrpgm - shader failed:\n"
                << shdr_pgm.GetLog() << "\n";
            std::exit(EXIT_FAILURE);
        }
    }
}

/*  _________________________________________________________________________ */
/* init                                                                        */
/*  _________________________________________________________________________ */

/*!
@brief  Initialises all GLPbo resources.

        Steps:
          1. Store dimensions, compute pixel_cnt and byte_cnt.
          2. Set default clear colour (white).
          3. Create immutable texture storage (GL_RGBA8).
          4. Create immutable PBO storage (write + map flags).
          5. Build full-window quad VAO.
          6. Compile shader program.
          7. Load OBJ models listed in ./scenes/ass-1.scn via DPML.
             Per-vertex normals are remapped from [-1,1] to [0,1] for use
             as RGB colour data.

@param  w   Framebuffer width  in pixels
@param  h   Framebuffer height in pixels
*/
void GLPbo::init(GLsizei w, GLsizei h)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    width = w;
    height = h;
    pixel_cnt = width * height;
    byte_cnt = pixel_cnt * 4;

    set_clear_color(255, 255, 255, 255);

    // Texture
    glCreateTextures(GL_TEXTURE_2D, 1, &texid);
    glTextureStorage2D(texid, 1, GL_RGBA8, width, height);

    // PBO
    glCreateBuffers(1, &pboid);
    glNamedBufferStorage(pboid, byte_cnt, nullptr,
        GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

    setup_quad_vao();
    setup_shdrpgm();

    // ---- Load scene file ------------------------------------------------
    std::ifstream scene_file("../scenes/ass-1.scn");
    if (!scene_file.is_open())
    {
        std::cerr << "GLPbo::init - cannot open ../scenes/ass-1.scn\n";
        return;
    }

    std::string mesh_name;
    while (scene_file >> mesh_name)
    {
        std::string obj_path = "../meshes/" + mesh_name + ".obj";

        Model mdl;
        mdl.name = mesh_name;

        bool ok = DPML::parse_obj_mesh(
            obj_path,
            mdl.pm,       // positions
            mdl.nml,      // normals
            mdl.tex,      // texcoords
            mdl.tri,      // triangle indices
            true,         // load_nml_coord_flag  (want normals)
            false,        // load_tex_coord_flag  (don't need tex this submission)
            true);        // model_centered_flag

        if (!ok)
        {
            std::cerr << "GLPbo::init - failed to parse " << obj_path << "\n";
            continue;
        }

        // Remap normals from [-1,1] to [0,1] for use as RGB colour
        for (auto& n : mdl.nml)
            n = (n + glm::vec3(1.f)) * 0.5f;

        models.push_back(std::move(mdl));
    }

    if (models.empty())
    {
        std::cerr << "GLPbo::init - no models loaded\n";
    }

    // Initial viewport transform
    if (!models.empty())
        viewport_xform(models[static_cast<std::size_t>(mdl_ref)]);
}

/*  _________________________________________________________________________ */
/* emulate                                                                     */
/*  _________________________________________________________________________ */

/*!
@brief  Per-frame CPU-side rasterizer.  Called once per game loop iteration.

        Workflow:
          1. Apply viewport transform to the current model each frame
             (needed because rotate_angle may change).
          2. Map the PBO data store.
          3. If painter mode: call painter_mode() (no clear).
             If static mode:
               a. Clear colour buffer (black background).
               b. Rasterize model triangles according to render_mode.
          4. Unmap PBO.
          5. DMA PBO -> texture via glTextureSubImage2D.
*/
void GLPbo::emulate()
{
    // ---- Key input --------------------------------------------------------
    GLFWwindow* win = GLHelper::ptr_window;

    // M: cycle to next model
    static bool m_was_pressed = false;
    bool m_pressed = (glfwGetKey(win, GLFW_KEY_M) == GLFW_PRESS);
    if (m_pressed && !m_was_pressed && !models.empty())
    {
        mdl_ref = (mdl_ref + 1) % static_cast<int>(models.size());
    }
    m_was_pressed = m_pressed;

    // W: cycle render mode (static mode only)
    static bool w_was_pressed = false;
    bool w_pressed = (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS);
    if (w_pressed && !w_was_pressed && !is_painter_mode)
    {
        render_mode = (render_mode + 1) % 4;
    }
    w_was_pressed = w_pressed;

    // R: toggle continuous rotation on/off
    static bool r_was_pressed = false;
    static bool is_rotating = false;
    bool r_pressed = (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS);
    if (r_pressed && !r_was_pressed)
    {
        is_rotating = !is_rotating;
    }
    r_was_pressed = r_pressed;

    if (is_rotating)
    {
        rotate_angle += 0.5f;
        if (rotate_angle >= 360.f) rotate_angle -= 360.f;
    }

    // P: toggle painter mode
    static bool p_was_pressed = false;
    bool p_pressed = (glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS);
    if (p_pressed && !p_was_pressed)
    {
        is_painter_mode = !is_painter_mode;
        if (is_painter_mode)
        {
            // Entering painter mode: (re)initialize the persistent canvas
            // to solid yellow. The per-frame painter_mode() call will
            // copy this into the PBO on the very next frame, so no direct
            // GL map/clear/unmap is needed here anymore.
            painter_canvas.assign(static_cast<std::size_t>(pixel_cnt),
                Color(255, 255, 0, 255));
            painter_dragging = false;
        }
    }
    p_was_pressed = p_pressed;

    // ---- Viewport transform for current model ----------------------------
    if (!models.empty())
    {
        viewport_xform(models[static_cast<std::size_t>(mdl_ref)]);
    }

    // ---- Painter mode: just draw committed segment -----------------------
    if (is_painter_mode)
    {
        ptr_to_pbo = reinterpret_cast<Color*>(
            glMapNamedBuffer(pboid, GL_WRITE_ONLY));
        if (!ptr_to_pbo)
        {
            std::cerr << "GLPbo::emulate - glMapNamedBuffer returned nullptr\n";
            return;
        }

        painter_mode();

        glUnmapNamedBuffer(pboid);
        ptr_to_pbo = nullptr;

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboid);
        glTextureSubImage2D(texid, 0, 0, 0, width, height,
            GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        return;
    }

    // ---- Static rendering mode ------------------------------------------
    ptr_to_pbo = reinterpret_cast<Color*>(
        glMapNamedBuffer(pboid, GL_WRITE_ONLY));
    if (!ptr_to_pbo)
    {
        std::cerr << "GLPbo::emulate - glMapNamedBuffer returned nullptr\n";
        return;
    }

    // Clear to white
    set_clear_color(255, 255, 255, 255);
    clear_color_buffer();

    if (!models.empty())
    {
        Model const& mdl = models[static_cast<std::size_t>(mdl_ref)];
        std::size_t tri_count = mdl.tri.size() / 3;
        culled_cnt = 0;

        // ---------------------------------------------------------------
        // Painter's algorithm (A1-friendly, no real depth buffer):
        // sort triangles farthest -> nearest using each triangle's average
        // NDC z (mdl.pm). Rotation is about the z-axis only, so z is
        // unaffected by viewport_xform() and this order stays valid even
        // as the model spins. Triangles drawn later overwrite triangles
        // drawn earlier, so nearer triangles must be drawn last.
        // NOTE: if your scene still looks back-to-front, flip the
        // comparison below (a.z < b.z  <->  a.z > b.z) - the "near" sign
        // of NDC z depends on how the OBJ/parser produced it.
        // ---------------------------------------------------------------
        static std::vector<glm::vec3> tri_colors;
        static std::vector<std::array<Color, 3>> edge_colors;
        static int colors_for_model = -1;
        if (colors_for_model != mdl_ref)
        {
            std::srand(static_cast<unsigned>(mdl_ref + 1));
            tri_colors.resize(tri_count);
            for (auto& c : tri_colors) c = rand_color();

            edge_colors.resize(tri_count);
            for (auto& e : edge_colors)
            {
                e[0] = Color(rand_byte(), rand_byte(), rand_byte(), 255);
                e[1] = Color(rand_byte(), rand_byte(), rand_byte(), 255);
                e[2] = Color(rand_byte(), rand_byte(), rand_byte(), 255);
            }

            colors_for_model = mdl_ref;
        }

        if (is_rotating)
        {
            for (auto& e : edge_colors)
            {
                e[0] = Color(rand_byte(), rand_byte(), rand_byte(), 255);
                e[1] = Color(rand_byte(), rand_byte(), rand_byte(), 255);
                e[2] = Color(rand_byte(), rand_byte(), rand_byte(), 255);
            }
        }

        for (std::size_t t = 0; t < tri_count; ++t)
        {
            unsigned short i0 = mdl.tri[t * 3 + 0];
            unsigned short i1 = mdl.tri[t * 3 + 1];
            unsigned short i2 = mdl.tri[t * 3 + 2];

            glm::vec3 const& v0 = mdl.pd[i0];
            glm::vec3 const& v1 = mdl.pd[i1];
            glm::vec3 const& v2 = mdl.pd[i2];

            switch (render_mode)
            {
            case 0: // Wireframe � black
            {
                // Quick back-face cull via cross product sign
                float area2 = (v1.x - v0.x) * (v2.y - v0.y)
                    - (v2.x - v0.x) * (v1.y - v0.y);
                if (area2 <= 0.f) { ++culled_cnt; break; }
                Color black(0, 0, 0, 255);
                render_linebresenham(
                    static_cast<GLint>(v0.x), static_cast<GLint>(v0.y),
                    static_cast<GLint>(v1.x), static_cast<GLint>(v1.y), black);
                render_linebresenham(
                    static_cast<GLint>(v1.x), static_cast<GLint>(v1.y),
                    static_cast<GLint>(v2.x), static_cast<GLint>(v2.y), black);
                render_linebresenham(
                    static_cast<GLint>(v2.x), static_cast<GLint>(v2.y),
                    static_cast<GLint>(v0.x), static_cast<GLint>(v0.y), black);
                break;
            }
            case 1: // Wireframe � random colour per edge
            {
                float area2 = (v1.x - v0.x) * (v2.y - v0.y)
                    - (v2.x - v0.x) * (v1.y - v0.y);
                if (area2 <= 0.f) { ++culled_cnt; break; }
                Color c0r = edge_colors[t][0];
                Color c1r = edge_colors[t][1];
                Color c2r = edge_colors[t][2];
                render_linebresenham(
                    static_cast<GLint>(v0.x), static_cast<GLint>(v0.y),
                    static_cast<GLint>(v1.x), static_cast<GLint>(v1.y), c0r);
                render_linebresenham(
                    static_cast<GLint>(v1.x), static_cast<GLint>(v1.y),
                    static_cast<GLint>(v2.x), static_cast<GLint>(v2.y), c1r);
                render_linebresenham(
                    static_cast<GLint>(v2.x), static_cast<GLint>(v2.y),
                    static_cast<GLint>(v0.x), static_cast<GLint>(v0.y), c2r);
                break;
            }
            case 2: // Flat shaded — random colour per triangle
            {
                glm::vec3 rc = tri_colors[t];
                if (!render_triangle(v0, v1, v2, rc))
                    ++culled_cnt;
                break;
            }            case 3: // Smooth shaded � interpolated normals as colours
            {
                glm::vec3 const& c0n = mdl.nml[i0];
                glm::vec3 const& c1n = mdl.nml[i1];
                glm::vec3 const& c2n = mdl.nml[i2];
                if (!render_triangle(v0, v1, v2, c0n, c1n, c2n))
                    ++culled_cnt;
                break;
            }
            default:
                break;
            }
        }
    }

    glUnmapNamedBuffer(pboid);
    ptr_to_pbo = nullptr;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboid);
    glTextureSubImage2D(texid, 0, 0, 0, width, height,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

/*  _________________________________________________________________________ */
/* draw_fullwindow_quad                                                        */
/*  _________________________________________________________________________ */

/*!
@brief  Renders the full-window textured quad and updates the window title
        bar with model name, vertex count, triangle count, culled count,
        render mode, rotation angle, and FPS.
*/
void GLPbo::draw_fullwindow_quad()
{
    shdr_pgm.Use();

    glBindTextureUnit(0, texid);
    shdr_pgm.SetUniform("uTex2d", 0);

    glBindVertexArray(vaoid);
    glDrawElements(GL_TRIANGLES,
        static_cast<GLsizei>(elem_cnt),
        GL_UNSIGNED_SHORT,
        nullptr);
    glBindVertexArray(0);

    shdr_pgm.UnUse();

    // ---- Title bar -------------------------------------------------------
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "A1 | c.chiow@digipen.edu";

    if (!models.empty())
    {
        Model const& mdl = models[static_cast<std::size_t>(mdl_ref)];
        std::size_t tri_count = mdl.tri.size() / 3;

        static const char* mode_names[] = {
            "Wireframe-Black",
            "Wireframe-Color",
            "Flat-Shaded",
            "Smooth-Shaded"
        };

        oss << " | Model: " << mdl.name
            << " | Vtx: " << mdl.pm.size()
            << " | Tri: " << tri_count
            << " | Culled: " << culled_cnt
            << " | Mode: " << mode_names[render_mode]
            << " | Rot: " << rotate_angle << (char)0xC2 << (char)0xB0
            << " | FPS: " << GLHelper::fps;
    }

    if (is_painter_mode)
        oss << " | PAINTER MODE";

    glfwSetWindowTitle(GLHelper::ptr_window, oss.str().c_str());
}

/*  _________________________________________________________________________ */
/* cleanup                                                                     */
/*  _________________________________________________________________________ */

/*!
@brief  Releases all GPU resources acquired by GLPbo::init.
*/
void GLPbo::cleanup()
{
    glDeleteVertexArrays(1, &vaoid);
    glDeleteBuffers(1, &pboid);
    glDeleteTextures(1, &texid);
    shdr_pgm.DeleteShaderProgram();
}