// File Generated by Assets/BuildShader.py - source: [BasicVfxVS.d.glsl : SCROLL_UV1]
in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;
uniform mat4 uMvpMatrix;
uniform vec4 uUVScale;
out vec2 varTexCoord0;
out vec4 varUVScale;
out vec4 varUVScale1;
out vec4 varColor;
void main(void)
{
	varTexCoord0 = inTexCoord0;
	varUVScale = uUVScale;
	varColor = inColor / 255.0;
	gl_Position = uMvpMatrix * inPosition;
}
