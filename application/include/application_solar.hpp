#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // defining struct with properties for the planets
  class Planet
{
   public:
      float rot;
      float trans;
      float scale;
      bool hasMoon;
      float color[3];
      glm::fmat4 model_matrix;
      glm::fmat4 normal_matrix ;
};



  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  void updateProjection(int i);
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  //void glfwGetCursorPos( double xpos, double ypos);
  // draw all objects
  void render() const;
  void upload_planet_transforms(Planet  &planet) const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry(model& mdl, model_object& object);
  void updateView(int i);
  void updateView();
  // cpu representation of model
  model_object planet_object;
  model_object star_object;
};

#endif
