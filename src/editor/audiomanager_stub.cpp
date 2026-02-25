/****************************************************************************
 *  Open RPG Maker is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation, either version 3 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  Open RPG Maker is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *  GNU General Public License for more details.                            *
 ****************************************************************************/

#include "audiomanager.h"

#include <QMap>
#include <QString>
#include <QTimer>

QMap<unsigned int, FMOD_SOUND*> AudioManager::sfx = QMap<unsigned int, FMOD_SOUND*>();
QMap<unsigned int, FMOD_SOUND*>::iterator AudioManager::sfxIter = AudioManager::sfx.end();
QMap<unsigned int, FMOD_CHANNEL*> AudioManager::sfxChannels = QMap<unsigned int, FMOD_CHANNEL*>();
QMap<unsigned int, FMOD_CHANNEL*>::iterator AudioManager::sfxChannelsIter = AudioManager::sfxChannels.end();
QMap<unsigned int, FMOD_DSP*> AudioManager::sfxDSPs = QMap<unsigned int, FMOD_DSP*>();
QMap<unsigned int, FMOD_DSP*>::iterator AudioManager::sfxDSPsIter = AudioManager::sfxDSPs.end();
QMap<unsigned int, bool> AudioManager::freeSFX = QMap<unsigned int, bool>();
QMap<unsigned int, bool>::iterator AudioManager::freeSFXIter = AudioManager::freeSFX.end();
QTimer *AudioManager::timer = 0;
FMOD_SOUND *AudioManager::musicFile = 0;
FMOD_CHANNEL *AudioManager::musicChannel = 0;
FMOD_DSP *AudioManager::musicDSP = 0;
FMOD_SYSTEM *AudioManager::fmodSystem = 0;
FMOD_RESULT AudioManager::fmodResult = FMOD_OK;
float AudioManager::musicMaxVolume = 0.0f;
float AudioManager::musicPlaySpeed = 1.0f;
int AudioManager::musicFadeInTime = 0;
bool AudioManager::freeMusic = false;
bool AudioManager::audioEnabled = false;

AudioManager::AudioManager() : QObject(0) {}
AudioManager::~AudioManager() {}

void AudioManager::initializeAudioManager() { audioEnabled = false; }
void AudioManager::shutdownAudioManager() {}

unsigned int AudioManager::loadSoundEffect(QString) { return 0; }
void AudioManager::freeSoundEffect(unsigned int) {}
void AudioManager::playSoundEffect(unsigned int, float, float, float, bool, int, bool) {}
void AudioManager::stopSoundEffect(unsigned int) {}
void AudioManager::loadMusicFile(QString) {}
void AudioManager::freeMusicFile() {}
void AudioManager::playMusic(int, float, float, float, bool, bool) {}
void AudioManager::stopMusic() {}
void AudioManager::stopAllSoundEffects() {}

bool AudioManager::isSoundEffectPlaying(unsigned int) { return false; }
bool AudioManager::isSoundEffectPlaying() { return false; }
bool AudioManager::isMusicPlaying() { return false; }

float AudioManager::getMusicPlaySpeed() { return musicPlaySpeed; }
unsigned int AudioManager::getMusicPosition() { return 0; }
unsigned int AudioManager::getSoundEffectLength(unsigned int) { return 0; }
unsigned int AudioManager::getMusicLength() { return 0; }
bool AudioManager::isAudioEnabled() { return audioEnabled; }

void AudioManager::updateSystem() {}

bool AudioManager::checkError(QString) { return false; }
