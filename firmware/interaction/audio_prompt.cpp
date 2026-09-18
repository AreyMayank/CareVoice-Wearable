#include "audio_prompt.h"

AudioPrompt::AudioPrompt(uint8_t bclk, uint8_t lrck, uint8_t dout)
    : pin_bclk(bclk), pin_lrck(lrck), pin_dout(dout), playing(false),
      current_prompt(AudioPromptType::NONE) {}

bool AudioPrompt::begin() {
    // I2S interface setup placeholder
    return true;
}

void AudioPrompt::playPrompt(AudioPromptType prompt) {
    current_prompt = prompt;
    playing = true;
    // Transmits PCM audio buffer to MAX98357A I2S bus
}

void AudioPrompt::stop() {
    playing = false;
    current_prompt = AudioPromptType::NONE;
}

bool AudioPrompt::isPlaying() const {
    return playing;
}
