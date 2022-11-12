#include "WAudioController.h"

WAudioController::WAudioController(double sVolume, double mVolume) :
	sfxVolume(sVolume),
	musicVolume(mVolume)
{
	//Allocate 16 mixing channels
	Mix_AllocateChannels(16);

}

WAudioController::~WAudioController()
{
	Mix_HaltChannel(-1); //Halt playback on all channels
	Mix_HaltMusic(); //Halt music playback

	for (auto& chunk : sfx)
	{
		Mix_FreeChunk(chunk.second);
	}

	for (auto& mus : music)
	{
		Mix_FreeMusic(mus.second);
	}

	sfx.clear();
	music.clear();
}

/**
* Load a sound chunk into the audio controller
* @param file File directory to load sound chunk from
* @param key Key to associate chunk with in controller sfx dictionary
* @return 1 if successfully loaded chunk, 0 if failure to load chunk from file, -1 if
*	dictionary key collision occurred
*/
int WAudioController::loadChunk(char* file, string key)
{
	//Load chunk, return 0 if failed to load
	Mix_Chunk* chunk = Mix_LoadWAV(file);
	if (!chunk)
		return 0;
	
	//Add chunk to dictionary, return -1 if failed
	bool added = sfx.insert(make_pair(key, chunk)).second;
	if (!added)
		return -1;

	return 1; //Successful
}

/**
* Load a music file into the audio controller
* @param file File directory to load music from
* @param key Key to associate music with in controller music dictionary
* @return 1 if succesfully loaded music, 0 if failure to load music from file, -1 if
*	dictionary key collision occurred
*/
int WAudioController::loadMusic(char* file, string key)
{
	//Load music, return 0 if failed to load
	Mix_Music* mus = Mix_LoadMUS(file);
	if (!mus)
		return 0;

	//Add music to dictionary, return -1 if failed
	bool added = music.insert(make_pair(key, mus)).second;
	if (!added)
		return -1;

	return 1; //Successful
}

/**
* Check if chunk has already been loaded
* @param key Key to check for existence
* @return True if chunk already exists, false otherwise
*/
bool WAudioController::containsChunk(string key)
{
	return getChunk(key);
}

/**
* Check if music has already been loaded.
* @param key Key to check for existence
* @return True if music already exists, false otherwise
*/
bool WAudioController::containsMusic(string key)
{
	return getMusic(key);
}

/**
* Get sound chunk from sfx dictionary by key
* @param key Key of chunk to get
* @return Sound chunk of key, nullptr if does not exist
*/
Mix_Chunk* WAudioController::getChunk(string key)
{
	Mix_Chunk* chunk = nullptr;
	auto found = sfx.find(key); //Get iterator

	//If iterator != end(), then key was found
	if (found != sfx.end())
		chunk = found->second;

	return chunk; //If key was not found, then returns nullptr
}

/**
* Get music from music dictionary by key
* @param key Key of music to get
* @return Music of key, nullptr if does not exist
*/
Mix_Music* WAudioController::getMusic(string key)
{
	Mix_Music* mus = nullptr;
	auto found = music.find(key); //Get iterator

	//If iterator != end(), then key was found
	if (found != music.end())
		mus = found->second;

	return mus; //If key was not found, then returns nullptr
}

/**
* Set volume of SFX
* @param volume New volume to set, as double from 0 to 1
* @return Previous volume
*/
double WAudioController::setSfxVolume(double volume)
{
	double prev = sfxVolume; //Old volume

	//Keep volume within bounds of 0 to 1
	if (volume < 0.0f)
		volume = 0.0f;
	else if (volume > 1.0f)
		volume = 1.0f;

	sfxVolume = volume; //Set volume

	//Convert from double to integral fraction of MIX_MAX_VOLUME, max volume defined by SDL_Mixer
	int vol = roundf(MIX_MAX_VOLUME * volume);
	Mix_Volume(-1, vol); //Set volume of all channels

	return prev; //Return old volume
}

/**
* Set volume of music
* @param volume New volume to set, as double from 0 to 1
* @return Previous volume
*/
double WAudioController::setMusicVolume(double volume)
{
	double prev = musicVolume; //Old volume

	//Keep volume within bounds of 0 to 1
	if (volume < 0.0f)
		volume = 0.0f;
	else if (volume > 1.0f)
		volume = 1.0f;

	musicVolume = volume; //Set volume

	//Convert from double to integral fraction of MIX_MAX_VOLUME(128), max volume defined by SDL_Mixer
	int vol = roundf(MIX_MAX_VOLUME * volume);
	Mix_VolumeMusic(vol); //Set volume of all channels

	return prev; //Return old volume	
}

/**
* Get sfx volume
* @return SFX volume as a double from 0 to 1
*/
double WAudioController::getSfxVolume()
{
	return sfxVolume;
}

/**
* Get music volume
* @return Music volume as a double from 0 to 1
*/
double WAudioController::getMusicVolume()
{
	return musicVolume;
}

/**
* Plays and loops a sound effect a given number of times on the next 
	available channel
* @param key Key of sfx to play
* @param loops Number of times to loop sfx, -1 if infinite loop is wanted. Note that 
	loops = 1 will cause sfx to play twice
* @return Channel that the sfx is being played on, -1 if Mix error occurred, -2 if key does not exist
*/
int WAudioController::playSfxLooped(string key, int loops)
{
	Mix_Chunk* chunk = getChunk(key); //Get chunk to play

	//If chunk does not exist, return -2, otherwise attempt to play chunk
	int channel = chunk ? Mix_PlayChannel(-1, chunk, loops) : -2;

	//If Mix returned an error
	if (channel == -1)
		//Maybe critical error, or maybe just no channels were free
		printf("Mix_PlayChannel: %s\n", Mix_GetError());

	return channel;
}

/**
* Plays a sound effect on the next available channel.
* Simply calls playSfxLooped(key, 0)
* @param key Key of sfx to play
* @return Channel that the sfx is being played on, -1 if Mix error occurred, -2 if key does not exist
*/
int WAudioController::playSfx(string key)
{
	return playSfxLooped(key, 0);
}

/**
* Plays and loops music a given number of times. Will halt any previous music
* @param key Key of music to play
* @param loops Number of times to loop music, -1 if infinite loop is wanted. Note that
	loops = 1 will cause music to play twice
* @param fadeInTime Amount of time in milliseconds to fade in for. No fade (0), by default. Fade in only
	works for the first loop. Negative values are defaulted to 0.
* @return 0 if succesful, -1 if Mix error occurred, -2 if key does not exist
*/
int WAudioController::playMusicLooped(string key, int loops, int fadeInTime)
{
	Mix_Music* mus = getMusic(key); //Get music to play

	//If chunk does not exist, return -2, otherwise attempt to play chunk
	int errors = 0;
	
	//Apply fading in effect if fadeInTime is more than 0
	if (fadeInTime <= 0)
		errors = mus ? Mix_PlayMusic(mus, loops) : -2;
	else
		errors = mus ? Mix_FadeInMusic(mus, loops, fadeInTime) : -2;

	//If Mix returned an error
	if (errors == -1)
		//Maybe critical error, or maybe just no channels were free
		printf("Mix_PlayChannel: %s\n", Mix_GetError());

	return errors;
}

/**
* Plays music on an infinite loop. Will halt any previous music
* Simply calls playMusicLooped(key, -1)
* @param key Key of sfx to play
* @param fadeInTime Amount of time in milliseconds to fade in for. No fade (0), by default. Negative
* values are defaulted to 0.
* @return Channel that the sfx is being played on, -1 if Mix error occurred, -2 if key does not exist
*/
int WAudioController::playMusic(string key, int fadeInTime)
{
	return playMusicLooped(key, -1, fadeInTime);
}

/**
* Pause music if any is playing
* @return 0 to allow for compatibility with tertiary operators
*/
int WAudioController::pauseMusic()
{
	Mix_PauseMusic();
	return 0;
}

/**
* Resume music if it has been paused. NOTE: Not safe to use if no music has been played at all 
up to this point
* @return 0 to allow for compatibility with tertiary operators
*/
int WAudioController::resumeMusic()
{
	Mix_ResumeMusic();
	return 0;
}

/**
* Halt any music if it is playing. This will interrupt any fading effects
* @param fadeOutTime Amount of time in milliseconds to fade out for before halt. No fade (0) 
* by default. Negative values are defaulted to 0.
* @return 0 to allow for compatibility with tertiary operators
*/
int WAudioController::haltMusic(int fadeOutTime)
{
	if (fadeOutTime <= 0)
		Mix_HaltMusic();
	else
		Mix_FadeOutMusic(fadeOutTime);

	return 0;
}

/**
* Get if music is paused
* @return True if music is paused, false if not paused. NOTE: Does not check if music is playing/halted
*/
bool WAudioController::getPausedMusic()
{
	return Mix_PausedMusic();
}

/**
* Get if music is halted
* @return True if music is halted, false if not halted. NOTE: Does not check if music is paused
*/
bool WAudioController::getPlayingMusic()
{
	return Mix_PlayingMusic();
}
