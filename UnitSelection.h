#ifndef UNIT_SELECTION_H
#define UNIT_SELECTION_H

#include <GL/gl3w.h>
#include <glm/gtc/matrix_transform.hpp>

#include "CompactArray.h"
#include "UnitPanel.h"

#include <vector>

class Entity;
class Program;

struct TransformComponent;

class UnitSelection {
public:
	UnitSelection(int player, CompactArray<TransformComponent>* transform_components, const Program* program, UnitPanel* unit_panel);

	void start(glm::vec2 view, glm::vec3 world);

	void end(glm::vec2 view, glm::vec3 world);

	void finalize();

	void draw();

	const std::vector<Entity*>& get() const;

	const std::vector<uint16_t> get_indices() const;
private:
	glm::vec2 _view_start;
	glm::vec2 _view_end;
	glm::vec3 _world_start;
	glm::vec3 _world_end;

	std::vector<Entity*> _selection;

	CompactArray<TransformComponent>*  _transform_components;

	int _player;

	const Program* _program;

	UnitPanel* _unit_panel;
};

#endif