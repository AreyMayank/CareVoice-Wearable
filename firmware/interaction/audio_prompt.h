#ifndef AUDIO_PROMPT_H
#define AUDIO_PROMPT_H

#include <cstdint>

enum class AudioPromptType {
    NONE,
    FALL_CONFIRMATION_ALERT, // "Fall detected. Press button if you are okay."
    SOS_ACTIVATED,          // "Emergency SOS sent."
    CANCEL_ACKNOWLEDGED     // "Alert cancelled."
};

class AudioPrompt {
public:
    AudioPrompt(uint8_t bclk = 16, uint8_t lrck = 17, uint8_t dout = 18);

    bool begin();
    void playPrompt(AudioPromptType prompt);
    void stop();
    bool isPlaying() const;

private:
    uint8_t pin_bclk;
    uint8_t pin_lrck;
    uint8_t pin_dout;
    bool playing;
    AudioPromptType current_prompt;
};

#endif // AUDIO_PROMPT_H
