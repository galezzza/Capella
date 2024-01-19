#include "Render_Utils.h"

#include <algorithm>

#include "glew.h"
#include "freeglut.h"


void Core::DrawContext(Core::RenderContext& context)
{

	glBindVertexArray(context.vertexArray);
	glDrawElements(
		GL_TRIANGLES,      // mode
		context.size,    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);
	glBindVertexArray(0);
}


void Core::deleteVAO(GLuint VAO) {
    GLint nAttr = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nAttr);
    glBindVertexArray(VAO);
    for (int iAttr = 0; iAttr < nAttr; ++iAttr) {
        GLint vboId = 0;
        glGetVertexAttribiv(iAttr, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vboId);
        if (vboId > 0) {
            GLuint _v = GLuint(vboId);
            glDeleteBuffers(1, &_v);
        }
    }
    glDeleteVertexArrays(1, &VAO);
}