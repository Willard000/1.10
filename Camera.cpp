#include "Camera.h"

#include "Program.h"

#include <math.h>

const float M_PI_2 = float(acos(-1)) / 2.0f;

Camera::Camera(GLFWwindow* window, const CameraSettings& settings) :
	_window					( window ),
	_mode					( FREE ),
	_settings				( settings ),
	_projection				( glm::perspective(settings._field_of_view, settings._aspect, settings._z_near, settings._z_far) ),
	_view					( glm::mat4(0) ),
	_direction				( glm::vec3(0) ),
	_position				( glm::vec3(0) ),
	_right					( glm::vec3(0) ),
	_up						( glm::vec3(0) )
{
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	glViewport(0, 0, w, h);

	//GLfloat aspect = (GLfloat)w / (GLfloat)h;
	
	_ortho = glm::ortho(0, 1, 0, 1);
}

void Camera::update() {
	_direction = glm::vec3(
		cos(_settings._v_angle) * sin(_settings._h_angle),
		sin(_settings._v_angle),
		cos(_settings._v_angle) * cos(_settings._h_angle)
	);

	_right = glm::vec3(
		sin(_settings._h_angle - M_PI_2),
		0,
		cos(_settings._h_angle - M_PI_2)
	);

	_up = glm::cross(_right, _direction);

	_view = glm::lookAt(
		_position,
		_position + _direction,
		_up
	);

	for(const auto& program : _programs) {
		program->use();
		glUniformMatrix4fv(program->location("view"), 1, GL_FALSE, &_view[0][0]);
	}
}

void Camera::set_mode(int mode) {
	switch(mode) {
	case FREE:
		_mode = mode;
		break;
	case LOCKED:
		_settings._v_angle = LOCKED_VERTICAL_ANGLE;
		_settings._h_angle = 3.14f;
		_mode = mode;
		break;
	case TOGGLE:
		switch(_mode) {
		case FREE:
			set_mode(LOCKED);
			break;
		case LOCKED:
			set_mode(FREE);
			break;
		default:
			break;
		}
	default:
		break;
	}
}

void Camera::move(int direction, float time, float speed) {
	switch(_mode) {
	case FREE:
		move_free(direction, time, speed);
		break;
	case LOCKED:
		move_locked(direction, time, speed);
		break;
	default:
		break;
	}
}

void Camera::move_free(int direction, float time, float speed) {
	switch(direction) {
	case FORWARD:
		_position.x += _direction.x * ( DEFAULT_SPEED + speed ) * time;
		_position.z += _direction.z * ( DEFAULT_SPEED + speed ) * time;
		break;
	case BACKWARD:
		_position.x -= _direction.x * ( DEFAULT_SPEED + speed ) * time;
		_position.z -= _direction.z * ( DEFAULT_SPEED + speed ) * time;
		break;
	case LEFT:
		_position.x -= _right.x * ( DEFAULT_SPEED + speed ) * time;
		_position.z -= _right.z * ( DEFAULT_SPEED + speed ) * time;
		break;
	case RIGHT:
		_position.x += _right.x * ( DEFAULT_SPEED + speed ) * time;
		_position.z += _right.z * ( DEFAULT_SPEED + speed ) * time;
		break;
	case UP:
		_position.y += ( DEFAULT_SPEED + speed ) * time;
		break;
	case DOWN:
		_position.y -= ( DEFAULT_SPEED + speed ) * time;
		break;
	default:
		break;
	}
}

void Camera::move_locked(int direction, float time, float speed) {
	switch(direction) {
	case FORWARD:
		_position.z -= ( DEFAULT_SPEED + speed ) * time;
		break;
	case BACKWARD:
		_position.z += ( DEFAULT_SPEED + speed ) * time;
		break;
	case LEFT:
		_position.x -= ( DEFAULT_SPEED + speed ) * time;
		break;
	case RIGHT:
		_position.x += ( DEFAULT_SPEED + speed ) * time;
		break;
	case UP:
		_position.y += ( DEFAULT_SPEED + speed ) * time;
		break;
	case DOWN:
		_position.y -= ( DEFAULT_SPEED + speed ) * time;
		break;
	default:
		break;
	}
}

void Camera::rotate(float x, float y) {
	if (_mode == LOCKED) {
		return;
	}
	int width, height;
	glfwGetWindowSize(_window, &width, &height);
	_settings._h_angle += 0.001f * ( ( (float) width / 2.0f ) - x );
	_settings._v_angle += 0.001f * ( ( (float) height / 2.0f ) - y );
	glfwSetCursorPos(_window, (double) width / 2.0, (double) height / 2.0);
}

void Camera::attach_program(Program* program) {
	program->use();
	glUniformMatrix4fv(program->location("view"), 1, GL_FALSE, &_view[0][0]);
	glUniformMatrix4fv(program->location("projection"), 1, GL_FALSE, &_projection[0][0]);
	glUniformMatrix4fv(program->location("ortho"), 1, GL_FALSE, &_ortho[0][0]);

	_programs.push_back(program);
}

glm::mat4 Camera::get_view() const {
	return _view;
}

glm::mat4 Camera::get_projection() const {
	return _projection;
}

glm::vec3 Camera::get_position() const {
	return _position;
}

int Camera::get_mode() const {
	return _mode;
}

glm::vec3 Camera::mouse_position_world() const {
	double x, y;
	int width, height;
	glfwGetCursorPos(_window, &x, &y);
	glfwGetWindowSize(_window, &width, &height);
	const float x_norm = static_cast<float>( x ) / static_cast<float>( width / 2 ) - 1.0f;
	const float y_norm = -( static_cast<float>( y ) / static_cast<float>( height / 2 ) - 1.0f );

	//				       inverse projection							    clip space
	glm::vec4 view_space = glm::inverse(_projection) * glm::vec4(x_norm, y_norm, -1.0f, 1.0f);
	view_space.z = -1.0f;
	view_space.w = 0.0f;

	//	normalized world space	 inverse view						 view_space
	glm::vec3 v = glm::normalize(glm::inverse(_view) * view_space);

	glm::vec3 position;
	position.y = abs((_position.y - 0) / glm::clamp(v.y, -1.0f, 0.0f));
	position.x = ( position.y * v.x + _position.x );
	position.z = ( position.y * v.z + _position.z );

	position.y = 0; // ?? this needs to be changed depending on terrain height at (x, z)
	return position;
}