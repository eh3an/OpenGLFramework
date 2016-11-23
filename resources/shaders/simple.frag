#version 150

in  vec3 pass_Normal;
in  vec3 pass_Color;
in  vec3 pass_LightVector;
in  vec3 pass_ViewerVector;

//Assignment 4
uniform  sampler2D ColorTex;
in vec2 pass_TexCoord;


out vec4 out_Color;

void main() {


    vec4 TexColor = texture(ColorTex, pass_TexCoord);


    vec3 h = normalize(pass_LightVector.xyz + pass_ViewerVector);
    vec3 normal = normalize(pass_Normal);


    //planet ambient color can be assumed as being the same as the diffuse color
    //planet specular color can be assumed as being white
    // Formula: ka,kd,ks,a - material parameters, ia,id,is - light parameters
    float ka = 0.2;
    float kd = 0.3;
    float ks = 0.4;
    vec3 ia = vec3(0.3f, 0.3f, 0.3f);
    vec3 id = vec3(0.3f, 0.3f, 0.3f);
    vec3 is = vec3(0.4f, 0.4f, 0.4f);

    //shininess
    float b = 1.988f;

    // Lights
    vec3 ambient = ka * ia+ TexColor.rgb;
    vec3 diffuse = kd * id+ TexColor.rgb;
    vec3 specular = ks * is;

    vec3 result = vec3(ambient+
                       diffuse * dot(pass_LightVector.rgb, normal)+
                       specular* pow(dot(normal,h),b));

    out_Color = vec4(result,1.0f);
}
