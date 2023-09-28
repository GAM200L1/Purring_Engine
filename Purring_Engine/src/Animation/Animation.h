#pragma once

#include <vector>
#include <string>


namespace PE
{
	struct AnimationFrame
	{
		std::string textureKey;
		float duration;
	};

	class Animation
	{
	public:
		Animation();

		void AddFrame(std::string textureKey, float duration);

		std::string UpdateAnimation(float deltaTime);


	private:
		std::vector<AnimationFrame> m_animationFrames;
		unsigned m_currentFrameIndex;
		float m_elapsedTime;
	};

	class AnimationManager
	{
	public:
		int CreateAnimation();

		void AddFrameToAnimation(int animationID, std::string textureKey, float duration);

		std::string UpdateAnimation(int animationID, float deltaTime);

	private:
		std::vector<Animation> m_animations;
	};
}