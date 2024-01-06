#version 450 core
// Outputs a vec4 color
layout(location  = 0) out vec4 FragColor;
// Texture Coordinates come in from Vertex Shader
in vec2 texCoord;

// Texture Sample from user
uniform sampler2D screenTexture;
// The Texture Code ID from user
uniform int texCodeId;

// Uniforms for HSV For color masking
uniform float hmax;
uniform float hmin;
uniform float smax;
uniform float smin;
uniform float vmax;
uniform float vmin;

// All components are in the range [0…1], including hue.
vec3 rgb2hsv(vec4 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

void main()
{
    // Original Texture
    vec4 Texture = texture2D(screenTexture, texCoord);

    // Different Color Formats
    vec4 colorArr[] = vec4[]
    (
    Texture														    	            // RGB
    , vec4(Texture.b, Texture.g, Texture.r, Texture.a)				                // BGR
    , vec4(0.299 * Texture.r + 0.587 * Texture.g + 0.114 * Texture.b)               // Gray
    , vec4(rgb2hsv(Texture), 1)                                                     // HSV
    );

    // For Color Masking
    vec3 tex = rgb2hsv(Texture);
    float colorMask = ((step(hmin, tex.r) - step(hmax, tex.r)) * (step(smin, tex.g) - step(smax, tex.g)) * (step(vmin, tex.b) - step(vmax, tex.b)));

    // Out Color
    FragColor = colorMask * colorArr[texCodeId];
}