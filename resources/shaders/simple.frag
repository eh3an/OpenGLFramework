#version 150

in  vec3 pass_Normal;
in  vec3 pass_Color;
in  vec3 pass_LightVector;
in  vec3 pass_ViewerVector;

out vec4 out_Color;

void main() {


    vec3 h = normalize(pass_LightVector.xyz + pass_ViewerVector);
    vec3 normal = normalize(pass_Normal);


    //planet ambient color can be assumed as being the same as the diffuse color
    //planet specular color can be assumed as being white
    // Formula: ka,kd,ks,a - material parameters, ia,id,is - light parameters
    float ka = 0.7;
    float kd = 0.45;
    float ks = 1.0;
    vec3 ia = vec3(0.3f, 0.3f, 0.3f);
    vec3 id = vec3(0.3f, 0.3f, 0.3f);
    vec3 is = vec3(1.0f, 1.0f, 1.0f);

    //shininess
    float b = 1.988f;

    // Lights
    vec3 ambient = ka * ia* pass_Color;
    vec3 diffuse = kd * id* pass_Color;
    vec3 specular = ks * is;

    vec3 result = vec3(ambient+
                       diffuse * dot(pass_LightVector.rgb, normal)+
                       specular* pow(dot(normal,h),b));

    out_Color = vec4(result,1.0f);
}
