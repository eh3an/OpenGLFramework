#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"
#include "structs.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <iostream>


model planet_model{};
model star_model{};

//A variable to keep position of stars
std::vector<float> starVector;

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}, star_object{}
{

  std::string textureDir = resource_path + "textures/";


  // Load texture maps

  auto sunTexture      = utils::create_texture_object(texture_loader::file(textureDir + "sun.png"),0);
  auto mercuryTexture  = utils::create_texture_object(texture_loader::file(textureDir + "mercury.png"),1);
  auto venusTexture    = utils::create_texture_object(texture_loader::file(textureDir + "venus.png"),2);
  auto earthTexture    = utils::create_texture_object(texture_loader::file(textureDir + "earth.png"),3);
  auto marsTexture     = utils::create_texture_object(texture_loader::file(textureDir + "mars.png"),4);
  auto jupiterTexture  = utils::create_texture_object(texture_loader::file(textureDir + "jupiter.png"),5);
  auto saturnTexture   = utils::create_texture_object(texture_loader::file(textureDir + "saturn.png"),6);
  auto uranusTexture   = utils::create_texture_object(texture_loader::file(textureDir + "uranus.png"),7);
  auto skyTexture      = utils::create_texture_object(texture_loader::file(textureDir + "galaxy.png"),8);


  std::map<std::string, texture_object> textureMap;
  textureMap = {
          {"sun"      , sunTexture},
          {"mercury"  , mercuryTexture},
          {"venus"    , venusTexture},
          {"earth"    , earthTexture},
          {"mars"     , marsTexture},
          {"jupiter"  , jupiterTexture},
          {"saturn"   , saturnTexture},
          {"uranus"   , uranusTexture},
          {"sky"      , skyTexture}
   };


  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(-20, 20);

  for(int j = 0; j< 1000; j++){



        starVector.push_back( dis(gen) );
        starVector.push_back( dis(gen) );
        starVector.push_back( dis(gen));
        starVector.push_back( dis(gen) );
        starVector.push_back( dis(gen) );
        starVector.push_back( dis(gen) );
        //std::cout << dis(gen) << " ";
  }


  planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL| model::TEXCOORD);
  star_model  = {starVector, model::POSITION | model::NORMAL};
 initializeGeometry(star_model, star_object);
  initializeGeometry(planet_model, planet_object);

  initializeShaderPrograms();
}


void ApplicationSolar::render() const {







   //std::cout << " planet  Done";
   // draw stars
   //std::cout << " START";
   //glUseProgram(m_shaders.at("star").handle);
   //glBindVertexArray(star_object.vertex_AO);
   //glDrawArrays(GL_POINTS, 0, 1000);
   // deactivate vertex arrays after drawing
   //glDisableClientState(GL_VERTEX_ARRAY);

   //shaderId=glCreateProgram();
   //glAttachShader(shaderId,m_shaders.at("planet").handle);
   //glAttachShader(shaderId,m_shaders.at("star").handle);

   //glUseProgram(m_shaders.at("star").handle);
   //glLinkProgram(shaderId);
   //glUseProgram(shaderId);
   glUseProgram(m_shaders.at("star").handle);
   glBindVertexArray(star_object.vertex_AO);
   glDrawArrays(GL_POINTS, 0, 1000);

    int noOfPlanets=9;
      //Define rotation, translation, and scale factor for each planet
 Planet planet[noOfPlanets];
    float sourceRot= 365;
    float rot[noOfPlanets]={0.0f,sourceRot / 88.0f,sourceRot / 225.0f,sourceRot / 365.0f,sourceRot / 687.0f,sourceRot / 4329.0f,sourceRot/ 1751.0f,sourceRot / 30664.0f,0.0f};
    float trans[noOfPlanets]={0.0,15.0,7.0,18.0,21.0,26.0,31.0,36.0,0.0f};
    float scale[noOfPlanets]={0.7f,0.05f,0.2f,0.15f,0.1f,1.35f,1.49f,1.1f,100.0f};
    float color[noOfPlanets][3]={{1.0f, 0.8f , 0.0f} ,{0.5f, 0.5f , 0.5f} ,{1.0f, 0.35f, 0.0f} ,{0.1f, 0.4f , 0.7f},{0.8f, 0.6f , 0.3f} ,{0.8f, 0.8f , 0.5f} ,{0.9f, 0.7f , 0.5f} ,{0.5f, 1.0f , 1.0f},{0.0f, 0.0f , 0.3f}  };
    bool hasMoon[noOfPlanets]={false,false,false,true,false,false,false,false,false};

    glUseProgram(m_shaders.at("planet").handle);
    glBindVertexArray(planet_object.vertex_AO);
//Assign specifications to planet objects in array
    for (int i=0;i<noOfPlanets;i++){
        planet[i].rot=rot[i];
        planet[i].trans=trans[i];
        planet[i].scale=scale[i];
        planet[i].hasMoon=hasMoon[i];
        planet[i].color[0]=color[i][0];
        planet[i].color[1]=color[i][1];
        planet[i].color[2]=color[i][2];
       upload_planet_transforms(planet[i],i);
    }
   // deactivate vertex arrays after drawing
   //glDisableClientState(GL_VERTEX_ARRAY);

}


//This function takes a planet object as input and will draw that

 void ApplicationSolar::upload_planet_transforms (Planet  &p,int index) const{


  glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * p.rot), glm::fvec3{0.0f, 1.0f, 0.0f});
  // set the size for the planets
  model_matrix = glm::scale(model_matrix, glm::vec3(p.scale));

  //determine the distance of the planets to the origin
  model_matrix = glm::translate(model_matrix, glm::fvec3{p.trans,0.0f, -1.0f}); // here change for distance and size
  if(index==8){ // 8 is the sky, only need scale
        model_matrix={};
        model_matrix = glm::scale(model_matrix, glm::vec3(p.scale));
        model_matrix = glm::translate(model_matrix, glm::fvec3{p.trans,0.0f, -1.2f});
  }


  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));

  // extra matrix for normal transformation to keep them orthogonal to surface
  glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));

  // Setting the colors for the planets
  glUniform3f(m_shaders.at("planet").u_locs.at("ColorVector"), p.color[0], p.color[1], p.color[2]);


  glUniform1i(m_shaders.at("planet").u_locs.at("ColorTex"), index);
  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

  if(p.hasMoon)
  {
       // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);


    // Using the already used model_matrix from above instantly
    model_matrix = glm::rotate(model_matrix,  float(glfwGetTime() * 10.0f), glm::fvec3{0.0f, 1.0f, 0.0f});
    model_matrix = glm::translate(model_matrix,  glm::fvec3{1.5f,0.0f, -1.0f});
    model_matrix = glm::scale(model_matrix, glm::vec3(0.35f));

    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                       1, GL_FALSE, glm::value_ptr(model_matrix));

    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(normal_matrix));

    glUniform3f(m_shaders.at("planet").u_locs.at("ColorVector"), 1.0f, 1.0f, 0.0f);

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
  }



}






void ApplicationSolar::updateView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);

  glUseProgram(m_shaders.at("planet").handle);
  // add light to sun
  glm::vec4 sun = {0.0f, 0.0f, 0.0f, 1.0f};
  sun = view_matrix * sun;
  glUniform3f(m_shaders.at("planet").u_locs.at("LightSource"), sun.x, sun.y, sun.z);
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("planet").handle);




}





void ApplicationSolar::updateProjection() {
  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
  glUseProgram(m_shaders.at("planet").handle);
}
// update uniform locations
void ApplicationSolar::uploadUniforms() {
  updateUniformLocations();
  glUseProgram(m_shaders.at("planet").handle);
  updateView();
  updateProjection();

  //
   //glUseProgram(m_shaders.at("star").handle);
    //updateStarView();
  //updateStarProjection();
}











// handle key input
// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {

  if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    updateView();
  }
  else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    updateView();
  }
  else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{1.0f, 0.0f, 0.0f});
    updateView();
  }
  else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{-1.0f, 0.0f, 0.0f});
    updateView();
  }
  else if (key == GLFW_KEY_UP && action == GLFW_PRESS){
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -1.0f, 0.0f});
    updateView();
  }
  else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 1.0f, 0.0f});
    updateView();
  }
  else if(key == GLFW_KEY_X && action == GLFW_PRESS){
    m_view_transform = glm::rotate(m_view_transform, 0.05f, glm::fvec3{0.0f, -1.0f, 0.0f});
    updateView();
  }
  else if(key == GLFW_KEY_V && action == GLFW_PRESS){
    m_view_transform = glm::rotate(m_view_transform, 0.05f, glm::fvec3{0.0f, 1.0f, 0.0f});
    updateView();
  }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling

  //zooming in by moving the mouse up
  if ( pos_x>0 && pos_y <0) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    updateView();
  }
  // zooming out by moving the mouse down
  else if ( pos_x<0 && pos_y >0){

    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    updateView();
  }
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {

  m_shaders.emplace("star", shader_program{m_resource_path + "shaders/stars.vert",
                                           m_resource_path + "shaders/stars.frag"});
  // request uniform locations for shader program
  m_shaders.at("star").u_locs["ViewMatrix"] = 1;
  m_shaders.at("star").u_locs["ProjectionMatrix"] = 1;
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",
                                           m_resource_path + "shaders/simple.frag"});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("planet").u_locs["ColorVector"]=-1;
  m_shaders.at("planet").u_locs["LightSource"]=-1;
  m_shaders.at("planet").u_locs["ColorTex"] = -1;



}
// load models
void ApplicationSolar::initializeGeometry(model& mdl, model_object& object)
{

    // generate vertex array object
    glGenVertexArrays(1, &object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mdl.data.size(), mdl.data.data(), GL_STATIC_DRAW);

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, mdl.vertex_bytes, mdl.offsets[model::POSITION]);
    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, mdl.vertex_bytes, mdl.offsets[model::NORMAL]);
    // activate third attribute on gpu
    glEnableVertexAttribArray(2);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, mdl.vertex_bytes, mdl.offsets[model::TEXCOORD]);

     // generate generic buffer
    glGenBuffers(1, &object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * mdl.indices.size(), mdl.indices.data(), GL_STATIC_DRAW);

    // store type of primitive to draw
    object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object
    object.num_elements = GLsizei(mdl.indices.size());


}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteBuffers(1, &star_object.element_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);
}

// exe entry point
int main(int argc, char* argv[]) {



  Launcher::run<ApplicationSolar>(argc, argv);
}
