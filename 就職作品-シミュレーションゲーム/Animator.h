#pragma once
class Animator
{
public:
	enum AnimationType
	{
		CARRY_IDLE,
		CARRY_RUN,
		CUT,
		IDLE_00,
		IDLE_01,
		IDLE_02,
		MINE,
		SLEEP,
		WALK,
		NONE,
	};

	//再生中のアニメーション
	AnimationType animationtype = IDLE_00;

	void Update();
	
	void ChangeAnimation(AnimationType type);

private:
	
};

