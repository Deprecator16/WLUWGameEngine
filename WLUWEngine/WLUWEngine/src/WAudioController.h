#pragma once

#include <string>
#include <unordered_map>
#include "SDL_mixer.h"

using namespace std;

/**
* Audio controller class
*/
class WAudioController
{
public:
	unordered_map<string, Mix_Chunk*> sfx; //Dictionary of sound effects
	unordered_map<string, Mix_Music*> music; //Dictionary of music

	float sfxVolume = 1.0f; //SFX volume
	float musicVolume = 1.0f; //Music volume

	//Constructors and destructor
	WAudioController(float sVolume = 1.0f, float mVolume = 1.0f);
	~WAudioController();

	//Sound file loading functions
	int loadChunk(char* file, string key);
	int loadMusic(char* file, string key);

	//Check container functions
	bool containsChunk(string key);
	bool containsMusic(string key);

	//Getters
	Mix_Chunk* getChunk(string key);
	Mix_Music* getMusic(string key);

	//Volume functions
	float setSfxVolume(float volume);
	float setMusicVolume(float volume);
	float getSfxVolume();
	float getMusicVolume();

	//Play SFX functions
	int playSfxLooped(string key, int loops);
	int playSfx(string key);

	//Music functions
	int playMusicLooped(string key, int loops, int fadeInTime = 0);
	int playMusic(string key, int fadeInTime = 0);
	int pauseMusic();
	int resumeMusic();
	int haltMusic(int fadeOutTime = 0);
	bool getPausedMusic();
	bool getPlayingMusic();
};

