#pragma once
#include "Scene.h"

namespace gomoku
{
	class SceneMatching final : public Scene
	{
	public:
		SceneMatching() = default;
		~SceneMatching() = default;
		SceneMatching(const SceneMatching& other) = delete;
		SceneMatching& operator=(const SceneMatching& rhs) = delete;

	private:
		void render() override;

	private:
		enum
		{
			STONE_STROKE_WIDTH = 2,
			LOW_OPACITY = 2,
		};
	};
}
