#ifndef VST2HOSTCALLBACK_H
#define VST2HOSTCALLBACK_H

class PianoRoll;
class PluginTrackView;

#include "SDK/aeffectx.h"
#include <SDK/aeffect.h>
#include <SDK/vstfxstore.h>
#include <src/midimanager.h>
#include <qvector.h>
#include <QObject>
#include <queue>
typedef VstIntPtr (*dispatcherFuncPtr)(AEffect *effect, VstInt32 opCode,
                                       VstInt32 index, VstInt32 value, void *ptr, float opt);
struct Rect {
    short top;
    short left;
    short bottom;
    short right;
};

struct EventToAdd
{
    uchar status = 0x90;
    uchar note = 0;
    bool eventOn = false;
    bool hasEventToAdd = false;
    qreal timeInTicks = 0;
    uchar velocity;
};

class Vst2HostCallback : public QObject
{
    Q_OBJECT
public:
    Vst2HostCallback();
    Vst2HostCallback(mTrack *track);
    AEffect* loadPlugin(char* fileName, char *pluginName);
    AEffect* LoadBridgedPlugin(char * szPath);
    bool canRecord();
    int configurePluginCallbacks(AEffect *plugin);
    void startPlugin(AEffect *plugin);
    void initializeIO();
    void processAudio(AEffect *plugin, float **inputs, float **outputs,long numFrames);
    void silenceChannel(float **channelData, int numChannels, long numFrames);
    void processMidi(AEffect *plugin);
    void initializeMidiEvents();
    void restartPlayback();
    void pauseOrResumePlayback(bool isResume);
    void addMidiEvent(uchar status, uchar note, uchar velocity);
    void setPianoRollRef(PianoRoll *piano);
    void setCanRecord(bool canRec);
    void turnOffAllNotes(AEffect *plugin);
    void showPlugin();
    void exportAudioInit();
    int exportAudioBegin(AEffect *plugin, float **outputs,
                          long numFrames);
    void exportAudioEnd();
    EventToAdd eventToAdd;
    std::queue<EventToAdd> midiEventQueue;
    std::deque<EventToAdd> recordedMidiEventDeque;
    mTrack *track;
    int ccFramesTillBlock[128];
    int ccVecPos[128];
    PianoRoll *pianoroll;

    float sampleRate = 44100.0f;
    bool canPlay = false;
    bool isMuted = false;
    bool isPaused = true;
    bool isMasterPlugin = false;
    int noteVecPos = 0;
    char *pluginName;
    PluginTrackView * masterPluginTrackView;

    dispatcherFuncPtr dispatcher;

private:
    VstEvents *events;

    QVector<int> *noteList;
    LPCSTR APPLICATION_CLASS_NAME = (LPCSTR)"MIDIHOST";
    HMODULE hinst;
    bool canRecording = false;
    bool hasReachedEnd = false;
    int TPQN = MidiManager::TPQN;
    const uint maxNotes = 256;
    VstMidiEvent *eventsHolder[256];
    uint numChannels = 2;
    int framesTillBlock = -1;
    float **outputs;
    float **inputs;
    float samplesPerTick = 0;

    HWND editor;
public slots:
    void setCustomPlackbackPos(int playbackPos);

};

struct pluginHolder
{
    Vst2HostCallback *host = NULL;
    AEffect *effect = NULL;
};

//from http://teragonaudio.com/article/How-to-make-your-own-VST-host.html

typedef AEffect * (*PFNBRIDGEMAIN)( audioMasterCallback audiomaster, char * pszPluginPath );

// Plugin's entry point
typedef AEffect* (*vstPluginFuncPtr)(audioMasterCallback host);
// Plugin's dispatcher function

// Plugin's getParameter() method
typedef float (*getParameterFuncPtr)(AEffect *effect, VstInt32 index);
// Plugin's setParameter() method
typedef void (*setParameterFuncPtr)(AEffect *effect, VstInt32 index, float value);
// Plugin's processEvents() method
typedef VstInt32 (*processEventsFuncPtr)(VstEvents *events);
// Plugin's process() method
typedef void (*processFuncPtr)(AEffect *effect, float **inputs,
                               float **outputs, VstInt32 sampleFrames);
#endif // VST2HOSTCALLBACK_H
