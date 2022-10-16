#pragma once
#include "assimpscene.h"

class AnimationDataAssimp {
private:
	AssimpScene		m_assimpscene;
public:
	bool Init(std::string filename);
	const aiScene* GetScene() {
		return m_assimpscene.GetScene();
	}
	void Exit();
}; 