#ifndef AUDIO_MANAGER_HPP
#define AUDIO_MANAGER_HPP

#include <raylib.h>
#include <map>
#include <string>
#include "../types.hpp"

using namespace std;

struct BGMData {
    Music music;
    float loopStart;
    float loopLength;
    bool hasLoop;
};

struct SoundEffect {
    Sound sound;
    float volume = 1.0f;
};

class AudioManager {
public:
    static map<BGMInfo, BGMData> bgms;
    static map<string, SoundEffect> soundEffects;
    static map<string, pair<float, float>> bgmLoopSamples;

    static Music currentBGM;
    static bool hasCurrentBGM;
    static float musicLogicalPos;
    static float currentBGMLoopStart;
    static float currentBGMLoopEnd;
    static bool currentBGMHasLoop;
    static float bgmVolume;
    static float bgmFadeStartVolume;
    static float bgmFadeTargetVolume;
    static float bgmFadeTimer;
    static float bgmFadeDuration;
    static bool bgmFading;

    static Music hurryUpSe;
    static bool playingHurryUp;
    static float hurryUpTimer;
    static bool hurryUpTriggered;
    static string hurryUpPendingStyle;
    static string hurryUpPendingTheme;
    static string hurryUpPendingType;

    static void init();
    static void close();
    static void loadSoundEffect(const string& id, const string& path, float volume = 1.0f);
    static void playSoundEffect(const string& id);
    static void loadSoundEffectsFromCSV(const string& path);
    static void loadBGMLoopsFromCSV(const string& path);
    static void loadBGM(const string& style, const string& theme, const string& type, const string& path);
    static void loadBGMFromTheme(const string& style, const string& theme);
    static void playBGM(const string& style, const string& theme, const string& type, bool alignPosition = false);
    static void setMusicLogicalPos(float pos);
    static void stopBGM();
    static void setBGMVolume(float volume);
    static void fadeBGM(float targetVolume, float duration);
    static void updateBGMFade(float dt);
    static void updateStreams(float dt);
    static void unloadAll();
};

#endif
