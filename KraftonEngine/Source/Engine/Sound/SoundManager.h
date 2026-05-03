#pragma once
#include "Core/CoreTypes.h"
#include "ThirdParty/SFML/Audio.hpp"

class SoundManager
{
public:
	// sound file mapping 
	void init();

private:

	TMap<FString, sf::SoundBuffer> SoundResourceMap;



};