#ifndef UI_H
#define UI_H

#include "UnitPanel.h"

struct GLFWwindow;

class UI {
public:
	UI(GLFWwindow* window, const std::vector<GLuint>& abilities);

	~UI();

	UI(const UI&&) = delete;

	UI& operator=(const UI&) = delete;

	void new_frame() const;

	void update();

	void draw() const;

	UnitPanel& get_unit_panel();

private:
	GLFWwindow* _window;

	UnitPanel _unit_panel;
};

#endif