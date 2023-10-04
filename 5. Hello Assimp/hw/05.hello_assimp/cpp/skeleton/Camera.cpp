#include "Camera.h"

// TODO: fill up the following function properly
void Camera::set_rotation(const glm::quat &_q)
{
  glm::mat4 rotation = glm::mat4_cast(_q);

  up_dir_ = glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
  right_dir_ = glm::vec3(rotation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
  front_dir_ = glm::vec3(rotation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
  //get_view_matrix();
}

// TODO: re-write the following function properly
const glm::quat Camera::get_rotation() const
{
  return glm::quatLookAt(-front_dir_, up_dir_);
}

// TODO: fill up the following function properly
void Camera::set_pose(const glm::quat &_q, const glm::vec3 &_t)
{
  set_rotation(_q);
  position_ = _t;
  //get_view_matrix();
}

// TODO: fill up the following function properly
void Camera::get_pose(glm::quat &_q, glm::vec3 &_t) const
{
  _q = glm::quat_cast(glm::mat3(right_dir_, up_dir_, -front_dir_));
  _t = position_;
}

// TODO: rewrite the following function properly
const glm::mat4 Camera::get_pose() const
{
  glm::mat4 pose = glm::mat4_cast(get_rotation());
  pose[3] = glm::vec4(position_, 0.0f);
  return pose;
}

// TODO: fill up the following function properly
void Camera::set_pose(const glm::mat4 &_frame)
{
  // +x_cam: right direction of the camera    (it should be a unit vector whose length is 1)
  right_dir_ = glm::normalize(glm::vec3(_frame[0][0], _frame[1][0], _frame[2][0])); // +x
  // +y_cam: up direction of the camera       (it should be a unit vector whose length is 1)
  up_dir_ = glm::normalize(glm::vec3(_frame[0][1], _frame[1][1], _frame[2][1])); // +y
  // -z_cam: front direction of the camera    (it should be a unit vector whose length is 1)
  front_dir_ = glm::normalize(glm::vec3(_frame[0][2], _frame[1][2], _frame[2][2]) * -1.0f); // -z
  // pos_cam: position of the camera
  position_ = glm::vec3(_frame[3]); // pos
  //get_view_matrix();
}

// TODO: fill up the following function properly
void Camera::set_pose(const glm::vec3 &_pos, const glm::vec3 &_at, const glm::vec3 &_up_dir)
{
  // up_dir_    = ..... ;
  // front_dir_ = ..... ;    // -z_cam direction
  // right_dir_ = ..... ;    // +x_cam direction
  // up_dir_    = ..... ;    // +y_cam direction

  // position_  = ..... ;      // pos

  position_ = _pos;
  front_dir_ = glm::normalize(_at - _pos);
  right_dir_ = glm::normalize(glm::cross(front_dir_, _up_dir));
  up_dir_ = glm::normalize(glm::cross(right_dir_, front_dir_));
  //get_view_matrix();
}

// TODO: rewrite the following function properly
const glm::mat4 Camera::get_view_matrix() const
{
  glm::vec3 look = position_ + front_dir_;
  return glm::lookAt(position_, look, up_dir_);
}

// TODO: rewrite the following function properly
const glm::mat4 Camera::get_projection_matrix() const
{
  // TODO: Considering the followings,
  //       You must return a proper projection matrix
  //       i) camera mode: it can be either kOrtho or kPerspective
  //       ii) zoom-in/-out: if the camera mode is kOrtho,
  //                         utilize ortho_scale_ for zoom-in/-out
  //                         if the camera mode is kPerspective,
  //                         utilize fovy_ for zoom-in/-out
  //       iii) aspect ratio: utilize aspect_ in the both camera modes
  //       iv) near/far clipping planes: utilize near_, far_

  if (mode_ == kOrtho)
  {
    float left = -ortho_scale_ * aspect_;
    float right = ortho_scale_ * aspect_;
    float bottom = -ortho_scale_;
    float top = ortho_scale_;
    return glm::ortho(left, right, bottom, top, near_, far_);
  }
  else
  {
    float fov = glm::radians(fovy_);
    return glm::perspective(fov, aspect_, near_, far_);
  }
}

// TODO: fill up the following functions properly
void Camera::move_forward(float delta)
{
  position_ += delta * front_dir_;
  get_view_matrix();
}

// TODO: fill up the following functions properly
void Camera::move_backward(float delta)
{
  position_ -= delta * front_dir_;
  //get_view_matrix();
}

// TODO: fill up the following functions properly
void Camera::move_left(float delta)
{
  position_ -= right_dir_ * delta;
  //get_view_matrix();
}

// TODO: fill up the following functions properly
void Camera::move_right(float delta)
{
  position_ += right_dir_ * delta;
  //get_view_matrix();
}

// TODO: fill up the following functions properly
void Camera::move_up(float delta)
{
  position_ += up_dir_ * delta;
  //get_view_matrix();
}

// TODO: fill up the following functions properly
void Camera::move_down(float delta)
{
  position_ -= up_dir_ * delta;
  //get_view_matrix();
}
