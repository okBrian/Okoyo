#version 450 core
// Outputs a vec4 color
layout(location  = 0) out vec4 FragColor;
// Texture Coordinates come in from Vertex Shader
in vec2 texCoord;

// Texture Sample from user
uniform sampler2D screenTexture;

//Uniforms for enabling different features
uniform int enableColorConversions;
uniform int enableColorMaskRange;
uniform int enableKernel;

// The Texture Code ID from user
uniform int texCodeId;

// Uniforms for HSV For color masking
uniform vec2 h;
uniform vec2 s;
uniform vec2 v;

// Uniforms for Kernel for post-proccessing
uniform mat3 kernel;

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

// Apply Color Format Options to Texture
vec4 applyColorFormat(vec4 Texture)
{
    // Different Color Formats
    vec4 colorArr[] = vec4[]
    (
    Texture														    	            // RGB
    , vec4(Texture.b, Texture.g, Texture.r, Texture.a)				                // BGR
    , vec4(0.299 * Texture.r + 0.587 * Texture.g + 0.114 * Texture.b)               // Gray
    , vec4(rgb2hsv(Texture), 1)                                                     // HSV
    );

    return colorArr[texCodeId];
}

// Apply Kernal Effects
vec4 applyKernal()
{
    const float offset = 1.0 / 300.0;
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture , texCoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            col += sampleTex[i] * kernel[i][j];
        }
    }

    return vec4(col, 1.0);
}

void main()
{
    // Original Texture
    vec4 Texture = texture(screenTexture, texCoord);
    // Color Conversions
    if(bool(enableColorConversions)) 
    {
        Texture = applyColorFormat(Texture);
    }

    // Color Masking
    if(bool(enableColorMaskRange))
    {
        vec3 tex = rgb2hsv(Texture);
        Texture *= ((step(h[0], tex.r) - step(h[1], tex.r)) * (step(s[0], tex.g) - step(s[1], tex.g)) * (step(v[0], tex.b) - step(v[1], tex.b)));
    }

    if(bool(enableKernel))
    {
        Texture *= applyKernal();
    }

    // Out Color
    FragColor = Texture;
}