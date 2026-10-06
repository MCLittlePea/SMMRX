#include "AudioManager.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include "../enums.hpp"

map<BGMInfo, BGMData> AudioManager::bgms;
map<string, SoundEffect> AudioManager::soundEffects;
map<string, pair<float, float>> AudioManager::bgmLoopSamples;

Music AudioManager::currentBGM = {};
bool AudioManager::hasCurrentBGM = false;
float AudioManager::musicLogicalPos = 0.0f;
float AudioManager::currentBGMLoopStart = 0.0f;
float AudioManager::currentBGMLoopEnd = 0.0f;
bool AudioManager::currentBGMHasLoop = false;
float AudioManager::bgmVolume = 1.0f;
float AudioManager::bgmFadeStartVolume = 0.0f;
float AudioManager::bgmFadeTargetVolume = 1.0f;
float AudioManager::bgmFadeTimer = 0.0f;
float AudioManager::bgmFadeDuration = 0.0f;
bool AudioManager::bgmFading = false;

Music AudioManager::hurryUpSe = {};
bool AudioManager::playingHurryUp = false;
float AudioManager::hurryUpTimer = 0.0f;
bool AudioManager::hurryUpTriggered = false;
string AudioManager::hurryUpPendingStyle;
string AudioManager::hurryUpPendingTheme;
string AudioManager::hurryUpPendingType;

static string bgmTypeToSuffix(const string& type) {
    if (type == "PlayNormal") return "";
    if (type == "PlayHurry") return "_Hurry";
    if (type == "PlayMoon") return "_Moon";
    if (type == "PlayMoonHurry") return "_Moon_Hurry";
    if (type == "Edit") return "_Edit";
    return "";
}

static string bgmInfoToLoopName(const string& style, const string& theme, const string& type) {
    return style + "_" + theme + bgmTypeToSuffix(type);
}

void AudioManager::init() {
    InitAudioDevice();
}

void AudioManager::close() {
    CloseAudioDevice();
}

void AudioManager::loadSoundEffect(const string& id, const string& path, float volume) {
    soundEffects[id] = {LoadSound(path.c_str()), volume};
}

void AudioManager::playSoundEffect(const string& id) {
    auto it = soundEffects.find(id);
    if (it != soundEffects.end()) {
        SetSoundVolume(it->second.sound, it->second.volume);
        PlaySound(it->second.sound);
    }
}

void AudioManager::loadSoundEffectsFromCSV(const string& path) {
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string cell;
        vector<string> c;
        while (getline(ss, cell, ',')) c.push_back(cell);
        if (c.size() < 2) continue;
        float vol = (c.size() >= 3) ? stof(c[2]) : 1.0f;
        loadSoundEffect(c[0], c[1], vol);
    }
}

void AudioManager::loadBGMLoopsFromCSV(const string& path) {
    ifstream file(path);
    if (!file.is_open()) return;
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t p1 = line.find(',');
        size_t p2 = line.find(',', p1 + 1);
        if (p1 == string::npos || p2 == string::npos) continue;
        string name = line.substr(0, p1);
        float ls = stof(line.substr(p1 + 1, p2 - p1 - 1));
        float le = stof(line.substr(p2 + 1));
        bgmLoopSamples[name] = {ls, le};
    }
}

void AudioManager::loadBGM(const string& style, const string& theme, const string& type, const string& path) {
    Music m = LoadMusicStream(path.c_str());
    BGMData data;
    data.music = m;
    data.hasLoop = false;
    data.loopStart = 0.0f;
    data.loopLength = 0.0f;
    string loopName = bgmInfoToLoopName(style, theme, type);
    auto lit = bgmLoopSamples.find(loopName);
    if (lit != bgmLoopSamples.end()) {
        float ls = lit->second.first;
        float le = lit->second.second;
        data.loopStart = ls;
        data.loopLength = le - ls;
        data.hasLoop = true;
    }
    bgms[{style, theme, type}] = data;
}

void AudioManager::loadBGMFromTheme(const string& style, const string& theme) {
    string base = "assets/musics/SMB1/" + theme;
    loadBGM(style, theme, "PlayNormal",    base + ".mp3");
    loadBGM(style, theme, "Edit",      base + "_Edit.mp3");
    loadBGM(style, theme, "PlayMoon",      base + "_Moon.mp3");
    loadBGM(style, theme, "PlayHurry",     base + "_Hurry.mp3");
    loadBGM(style, theme, "PlayMoonHurry", base + "_Moon_Hurry.mp3");
}

void AudioManager::playBGM(const string& style, const string& theme, const string& type, bool alignPosition) {
    auto it = bgms.find({style, theme, type});
    if (it == bgms.end()) return;
    if (hasCurrentBGM) StopMusicStream(currentBGM);
    BGMData& data = it->second;
    currentBGM = data.music;
    PlayMusicStream(currentBGM);
    SetMusicVolume(currentBGM, bgmVolume);
    if (!alignPosition) {
        musicLogicalPos = 0.0f;
    }
    if (data.hasLoop) {
        currentBGMLoopStart = data.loopStart;
        currentBGMLoopEnd = data.loopStart + data.loopLength;
        currentBGMHasLoop = true;
        if (alignPosition) {
            float pos;
            if (musicLogicalPos < data.loopStart) {
                pos = musicLogicalPos;
            } else {
                pos = fmod(musicLogicalPos - data.loopStart, data.loopLength) + data.loopStart;
            }
            SeekMusicStream(currentBGM, pos);
        }
    } else {
        currentBGMHasLoop = false;
    }
    hasCurrentBGM = true;
}

void AudioManager::setMusicLogicalPos(float pos) {
    musicLogicalPos = pos;
    if (hasCurrentBGM && currentBGMHasLoop) {
        float actualPos;
        if (pos < currentBGMLoopStart) {
            actualPos = pos;
        } else {
            actualPos = fmod(pos - currentBGMLoopStart, currentBGMLoopEnd - currentBGMLoopStart) + currentBGMLoopStart;
        }
        SeekMusicStream(currentBGM, actualPos);
    }
}

void AudioManager::stopBGM() {
    if (hasCurrentBGM) StopMusicStream(currentBGM);
    hasCurrentBGM = false;
}

void AudioManager::setBGMVolume(float volume) {
    bgmVolume = volume;
    if (hasCurrentBGM) SetMusicVolume(currentBGM, bgmVolume);
}

void AudioManager::fadeBGM(float targetVolume, float duration) {
    bgmFadeStartVolume = bgmVolume;
    bgmFadeTargetVolume = targetVolume;
    bgmFadeTimer = 0.0f;
    bgmFadeDuration = duration;
    bgmFading = true;
}

void AudioManager::updateBGMFade(float dt) {
    if (!bgmFading) return;
    bgmFadeTimer += dt;
    float t = bgmFadeDuration > 0 ? min(bgmFadeTimer / bgmFadeDuration, 1.0f) : 1.0f;
    bgmVolume = bgmFadeStartVolume + (bgmFadeTargetVolume - bgmFadeStartVolume) * t;
    if (hasCurrentBGM) SetMusicVolume(currentBGM, bgmVolume);
    if (t >= 1.0f) bgmFading = false;
}

void AudioManager::updateStreams(float dt) {
    if (hasCurrentBGM) {
        UpdateMusicStream(currentBGM);
        musicLogicalPos += dt;
        if (currentBGMHasLoop) {
            float pos = GetMusicTimePlayed(currentBGM);
            if (pos >= currentBGMLoopEnd) {
                float overflow = pos - currentBGMLoopEnd;
                SeekMusicStream(currentBGM, currentBGMLoopStart + overflow);
            }
        }
    }
    if (playingHurryUp) {
        UpdateMusicStream(hurryUpSe);
        hurryUpTimer += dt;
        if (hurryUpTimer >= 3.1f) {
            playingHurryUp = false;
            StopMusicStream(hurryUpSe);
            playBGM(hurryUpPendingStyle, hurryUpPendingTheme, hurryUpPendingType);
        }
    }
}

void AudioManager::unloadAll() {
    for (auto& [info, data] : bgms)
        UnloadMusicStream(data.music);
}
