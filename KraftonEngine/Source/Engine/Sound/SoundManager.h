#pragma once
#include "Core/CoreTypes.h"
#include "Platform/Paths.h"
#include "ThirdParty/SFML/Audio.hpp"


enum class SoundEffect : uint32
{
	Jump, 
	Parry,
	Death
};


class FSoundManager
{
public:
	// sound file mapping 
	void initialize();

	void PlayBGM();
	void StopBGM();


	void LoadEffect(SoundEffect ID, const std::wstring& FilePath);
	void PlayEffect(SoundEffect ID);


private:
	TMap<SoundEffect, std::unique_ptr<sf::SoundBuffer>> SoundBufferMap;
	TMap<SoundEffect, std::unique_ptr<sf::Sound>>       Sounds;
	sf::Music m_bgm; 


};