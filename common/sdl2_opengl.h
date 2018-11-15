#ifndef SDL2_OPENGL_H_
#define SDL2_OPENGL_H_
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>

#define MAX_VBO_BYTES (1024l * 1024l * 8l) // 8MB VRAM


extern bool sogl_init(const char* winname,
                      int width, int height,
                      const GLchar* vs_src,
                      const GLchar* fs_src);

extern void sogl_term(void);

extern bool sogl_handle_events(void);


extern void sogl_begin_frame(void);
extern Uint32 sogl_end_frame(void);

extern void sogl_vattrp(const GLchar* attrib_name,
                        GLint size,
                        GLenum type,
                        GLboolean normalized,
                        GLsizei stride,
                        const GLvoid* pointer);

extern void sogl_set_uniform(const GLchar* name, const void* data);

#endif
