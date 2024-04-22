#pragma once
#include "Scene.h"

namespace gomoku
{
	class SceneLobby final : public Scene
	{
	public:
		SceneLobby();
		~SceneLobby() = default;
		SceneLobby(const SceneLobby& other) = delete;
		SceneLobby& operator=(const SceneLobby& rhs) = delete;

		void Render() override;
	};
}
