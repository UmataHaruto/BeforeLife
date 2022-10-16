#include	<windows.h>
#include	"AnimationDataAssimp.h"

bool AnimationDataAssimp::Init(std::string filename) {

	bool sts = m_assimpscene.Init(filename);
	if (!sts){
		MessageBox(nullptr, "SkinAnimationDataAssimp load error", "error", MB_OK);
	}

	return true;
}

void AnimationDataAssimp::Exit() {
	m_assimpscene.Exit();
}