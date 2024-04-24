#pragma once
#include "Scene.h"
#include "SceneManager.h"

namespace gomoku
{
	class SceneLobby final : public Scene
	{
	public:
		SceneLobby() = default;
		~SceneLobby() = default;
		SceneLobby(const SceneLobby& other) = delete;
		SceneLobby& operator=(const SceneLobby& rhs) = delete;

		void Render() override;
	};
}
