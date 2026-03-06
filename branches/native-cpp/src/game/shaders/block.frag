#version 450

// 纹理图集
layout(binding = 1) uniform sampler2D blockTextureAtlas;

// 来自顶点着色器
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragWorldPos;

// 输出
layout(location = 0) out vec4 outColor;

void main() {
    // 简单的方向光照
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(normalize(fragNormal), lightDir), 0.0);

    // 环境光 + 漫反射
    vec3 lighting = vec3(0.4) + vec3(0.6) * diff;

    // 采样纹理
    vec4 texColor = texture(blockTextureAtlas, fragTexCoord);

    // 应用光照
    vec3 finalColor = texColor.rgb * lighting;

    outColor = vec4(finalColor, texColor.a);
}
