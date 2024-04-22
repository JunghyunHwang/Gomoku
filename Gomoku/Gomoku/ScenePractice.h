#pragma once
#include "Scene.h"

namespace gomoku
{
	class ScenePractice final : public Scene
	{
	public:
		ScenePractice() = default;
		~ScenePractice() = default;
		ScenePractice(const ScenePractice& other) = delete;
		ScenePractice& operator=(const ScenePractice& rhs) = delete;

		void Render() override;
	private:
		static GameManager* mGameManager;
	};
}
