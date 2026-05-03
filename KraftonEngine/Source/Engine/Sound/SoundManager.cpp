#include "SoundManager.h"

void SoundManager::initialize()
{
	if (!m_bgm.openFromFile(FPaths::Combine(FPaths::AssetDir() , L"Sound/Background.wav")))
	{
		throw std::runtime_error("BGM Load Failed : Sound/BackGround.wav" );
	}

	m_bgm.setLooping(true);


	LoadEffect(SoundEffect::Jump, FPaths::Combine(FPaths::AssetDir(), L"Sound/Jump.wav"));
	LoadEffect(SoundEffect::Death, FPaths::Combine(FPaths::AssetDir(), L"Sound/Death.wav"));
	LoadEffect(SoundEffect::Parry, FPaths::Combine(FPaths::AssetDir(), L"Sound/Parry.wav"));

}

void SoundManager::PlayBGM()
{
	m_bgm.play();
}

void SoundManager::StopBGM()
{
	m_bgm.stop();
}

void SoundManager::LoadEffect(SoundEffect ID, const std::wstring& FilePath)
{
	// SoundBufferMap[ID]가 없으면 default 생성, 있으면 덮어씀
	sf::SoundBuffer& buffer = SoundBufferMap[ID];

	if (!buffer.loadFromFile(FilePath))
	{
		throw std::runtime_error("Effect Load Failed ");
	}

	Sounds[ID].setBuffer(buffer);

}

void SoundManager::PlayEffect(SoundEffect ID)
{
	auto it = Sounds.find(ID);
	if (it == Sounds.end())
	{
		throw std::runtime_error("Effect not loaded");
	}

	it->second.play();

}
