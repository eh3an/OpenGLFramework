#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

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


  planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);
  star_model  = {starVector, model::POSITION | model::NORMAL};

  initializeGeometry(planet_model, planet_object);
  initializeGeometry(star_model, star_object);
  initializeShaderPrograms();
}


void ApplicationSolar::render() const {



    //Define rotation, translation, and scale factor for each planet
 Planet planet[8];
    float sourceRot= 365;
    float rot[8]={0.0f,sourceRot / 88.0f,sourceRot / 225.0f,sourceRot / 365.0f,sourceRot / 687.0f,sourceRot / 4329.0f,sourceRot/ 1751.0f,sourceRot / 30664.0f};
    float trans[8]={0.0,15.0,7.0,18.0,21.0,26.0,31.0,36.0};
    float scale[8]={0.7f,0.05f,0.2f,0.15f,0.1f,1.35f,1.49f,1.1f};
    bool hasMoon[8]={false,false,false,true,false,false,false,false};


//Assign specifications to planet objects in array
    for (int i=0;i<8;i++){
        planet[i].rot=rot[i];
        planet[i].trans=trans[i];
        planet[i].scale=scale[i];
        planet[i].hasMoon=hasMoon[i];
       upload_planet_transforms(planet[i]);
    }



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
   // deactivate vertex arrays after drawing
   glDisableClientState(GL_VERTEX_ARRAY);

}


//This function takes a planet object as input and will draw that

 void ApplicationSolar::upload_planet_transforms (Planet  &planet) const{
        glUseProgram(m_shaders.at("planet").handle);

        //rotate, scale and translate planet based on its specification
        glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * planet.rot), glm::fvec3{0.0f, 1.0f, 0.0f});
        model_matrix = glm::scale(model_matrix, glm::vec3(planet.scale));
        model_matrix = glm::translate(model_matrix, glm::fvec3{planet.trans,0.0f, -1.0f}); // here change for distance and size
        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));

        // extra matrix for normal transformation to keep them orthogonal to surface
        glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));

        // bind the VAO to draw
        glBindVertexArray(planet_object.vertex_AO);
        //Draw the planet
        glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);


        //For planets which has a moon, a moon will be made which is rotating around this planet
        if(planet.hasMoon)
        {

            glUseProgram(m_shaders.at("planet").handle);
            model_matrix = glm::rotate(model_matrix,  float(glfwGetTime() * 10.0f), glm::fvec3{0.0f, 1.0f, 0.0f});
            model_matrix = glm::translate(model_matrix,  glm::fvec3{1.5f,0.0f, -1.0f});
            model_matrix = glm::scale(model_matrix, glm::vec3(0.35f));
            glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                         1, GL_FALSE, glm::value_ptr(model_matrix));
            // extra matrix for normal transformation to keep them orthogonal to surface
            glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
            glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                         1, GL_FALSE, glm::value_ptr(normal_matrix));
            // bind the VAO to draw
            glBindVertexArray(planet_object.vertex_AO);
            glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
        }



        planet.model_matrix=model_matrix;
        planet.normal_matrix=normal_matrix;



}




void ApplicationSolar::updateView(int i) {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  if(i==1){
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
  }
   if(i==2){
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
  }
  //glUniformMatrix4fv(glGetUniformLocation(m_shaders.at("planet").handle,"ViewMatrix"),
  //                   1, GL_FALSE, glm::value_ptr(view_matrix));

}




void ApplicationSolar::updateView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
  glUseProgram(m_shaders.at("star").handle);
}



void ApplicationSolar::updateProjection(int i) {
  // upload matrix to gpu
  if (i==1){
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
  }

  if (i==2){
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
  }


}


void ApplicationSolar::updateProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
  glUseProgram(m_shaders.at("star").handle);
}
// update uniform locations
void ApplicationSolar::uploadUniforms() {
  updateUniformLocations();
  glUseProgram(m_shaders.at("star").handle);
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


  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",
                                           m_resource_path + "shaders/simple.frag"});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;

  m_shaders.emplace("star", shader_program{m_resource_path + "shaders/stars.vert",
                                           m_resource_path + "shaders/stars.frag"});
  // request uniform locations for shader program
  m_shaders.at("star").u_locs["ViewMatrix"] = 1;
  m_shaders.at("star").u_locs["ProjectionMatrix"] = 1;
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
