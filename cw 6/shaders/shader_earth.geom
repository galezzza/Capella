#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    for (int i = 0; i < gl_in.length(); ++i) {
        vec4 position = gl_in[i].gl_Position;
        // Displace the y-coordinate by a random amount
        position.y += (rand(vec2(gl_PrimitiveID + i, 0.0)) * 2.0 - 1.0) * 2;
        gl_Position = position;
        EmitVertex();
    }
    EndPrimitive();
}
