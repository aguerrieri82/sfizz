
#include "sfizz/Synth.h"
#include <st_audiofile_libs.h>

#ifdef _WINDOWS
    #define EXPORT __declspec(dllexport)
#endif

#ifdef __ANDROID__ 
    #define EXPORT __attribute__((visibility("default")))
#endif

struct sfz_config {
    int blockSize; // 1024
    int sampleRate; // 48000
    sfz::Synth::ProcessMode processMode;
    int sampleQuality; // 2
    int numVoices; // 64
    int oscillatorQuality;
};

struct sfz_buffers {

    sfz_buffers(size_t blockSize, int channels) :
        audioBuffer(channels, blockSize),
        interleavedBuffer(channels * blockSize),
        interleavedPcm(channels * blockSize) 
    {

    }

    sfz::AudioBuffer<float> audioBuffer;
    sfz::Buffer<float> interleavedBuffer;
    sfz::Buffer<int16_t> interleavedPcm;
};

extern "C"
{
    EXPORT sfz::Synth* createSynth()
    {
        return new sfz::Synth();
    }

    EXPORT void deleteSynth(sfz::Synth* synth)
    {
        delete synth;
    }

    EXPORT void setVolume(sfz::Synth& synth, float volume)
    {
        synth.setVolume(volume);
    }

    EXPORT float getVolume(sfz::Synth& synth)
    {
       return synth.getVolume();
    }

    EXPORT void controlCode(sfz::Synth& synth, int delay, int ccNumber, int ccValue)
    {
        synth.cc(delay, ccNumber, ccValue);
    }

    EXPORT void allSoundOff(sfz::Synth& synth)
    {
        synth.allSoundOff();
    }

    EXPORT void noteOn(sfz::Synth& synth, int delay, int noteNumber, int velocity)
    {
        synth.noteOn(delay, noteNumber, velocity);
    }

    EXPORT void noteOff(sfz::Synth& synth, int delay, int noteNumber, int velocity)
    {
        synth.noteOff(delay, noteNumber, velocity);
    }

    EXPORT void loadSfzFile(sfz::Synth& synth, const char* fileName)
    {
        synth.loadSfzFile(fileName);
    }

    EXPORT void configure(sfz::Synth& synth, sfz_config& config)
    {
        synth.setSamplesPerBlock(config.blockSize);
        synth.setSampleRate(config.sampleRate);
        synth.setSampleQuality(config.processMode, config.sampleQuality);
        synth.setOscillatorQuality(config.processMode, config.oscillatorQuality);
        synth.setNumVoices(config.numVoices);

        if (config.processMode == sfz::Synth::ProcessMode::ProcessFreewheeling)
            synth.enableFreeWheeling();
    }

    EXPORT sfz_buffers* createBuffer(size_t blockSize, int channels)
    {
        sfz_buffers* res = new sfz_buffers(blockSize, channels);
        return res;
    }

    EXPORT int16_t* getPcmPointer(sfz_buffers& buffers)
    {
        return buffers.interleavedPcm.data();
    }

    EXPORT float* getFloatPointer(sfz_buffers& buffers)
    {
        return buffers.interleavedBuffer.data();
    }

    EXPORT void deleteBuffer(sfz_buffers* buffer)
    {
        delete buffer;
    }

    EXPORT void render(sfz::Synth& synth, sfz_buffers& buffers, bool convertPcm)
    {
        synth.renderBlock(buffers.audioBuffer);

        if (buffers.audioBuffer.getNumChannels() == 2)
            sfz::writeInterleaved(buffers.audioBuffer.getConstSpan(0), buffers.audioBuffer.getConstSpan(1), absl::MakeSpan(buffers.interleavedBuffer));
        else
            std::memcpy(buffers.interleavedBuffer.data(), buffers.audioBuffer.getConstSpan(0).data(), buffers.interleavedBuffer.size());

        if (convertPcm)
            drwav_f32_to_s16(buffers.interleavedPcm.data(), buffers.interleavedBuffer.data(), buffers.interleavedPcm.size());
    }
}

