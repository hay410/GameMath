#include"PlayerParticleManager.h"

PlayerParticleManager::PlayerParticleManager() {
}

void PlayerParticleManager::Init()
{
	//èâä˙âªèàóù
	for (int i = 0; i < MAX_PLAYER_PARTICLE; ++i) {
		playerParticle[i].isAlive = false;
	}
}

void PlayerParticleManager::Generate(XMFLOAT3 playerPos, bool isPlayer)
{
	//ê∂ê¨ÇµÇƒÇ¢Ç»Ç¢ãOê’ÇíTÇµÇƒê∂ê¨Ç∑ÇÈ
	if (isPlayer == false) {
		for (int j = 0; j < 1; ++j) {
			for (int i = 0; i < MAX_PLAYER_PARTICLE; ++i) {
				if (playerParticle[i].isAlive == true) continue;
				Sprite Particle = SpriteManager::Instance()->Init2DTexture(XMFLOAT3(playerPos.x + ((rand() % PARTICLE_DEVIATION) - PARTICLE_DEVIATION / 2.0f),
					playerPos.y + ((rand() % PARTICLE_DEVIATION) - PARTICLE_DEVIATION / 2.0f), playerPos.z + ((rand() % PARTICLE_DEVIATION) - PARTICLE_DEVIATION / 2.0f)), XMFLOAT2(30, 30), billBoard, L"Resources/smoke.png");
				playerParticle[i].Generate(Particle);
				break;
			}
		}
		return;
	}

	for (int j = 0; j < 4; ++j) {
		for (int i = 0; i < MAX_PLAYER_PARTICLE; ++i) {
			if (playerParticle[i].isAlive == true) continue;
			Sprite Particle = SpriteManager::Instance()->Init2DTexture(XMFLOAT3(playerPos.x + ((rand() % PARTICLE_DEVIATION) - PARTICLE_DEVIATION / 2.0f),
				playerPos.y + ((rand() % PARTICLE_DEVIATION) - PARTICLE_DEVIATION / 2.0f), playerPos.z + ((rand() % PARTICLE_DEVIATION) - PARTICLE_DEVIATION / 2.0f)), XMFLOAT2(30, 30), billBoard, L"Resources/smoke.png");
			playerParticle[i].Generate(Particle);
			break;
		}
	}
}

void PlayerParticleManager::Update()
{
	for (int i = 0; i < MAX_PLAYER_PARTICLE; ++i) {
		if (playerParticle[i].isAlive == false) continue;
		playerParticle[i].Update();
	}
}

void PlayerParticleManager::Draw()
{
	for (int i = 0; i < MAX_PLAYER_PARTICLE; ++i) {
		if (playerParticle[i].isAlive == false) continue;
		playerParticle[i].Draw();
	}
}
