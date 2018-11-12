#ifndef SDL2_OPENGL_H_
#define SDL2_OPENGL_H_
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>

#define MAX_VBO_BYTES (1024l * 1024l * 8l) // 8MB VRAM


extern bool sdl2_opengl_init(const char* winname,
                             int width, int height,
		             const GLchar* vs_src,
		             const GLchar* fs_src);

extern void sdl2_opengl_term(void);

extern bool sdl2_opengl_handle_events(void);


extern void sdl2_opengl_begin_frame(void);
extern Uint32 sdl2_opengl_end_frame(void);

extern void sdl2_opengl_vattrp(const GLchar* attrib_name,
                               GLint size,
                               GLenum type,
                               GLboolean normalized,
                               GLsizei stride,
                               const GLvoid* pointer);

#endif
