/*
  ==============================================================================

    Author: Matt Hill

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public AudioAppComponent,
                               public Slider::Listener
{
public:
    //==============================================================================
    MainContentComponent()
    :
        currentSampleRate (0.0),
        currentAngle (0.0),
        angleDelta (0.0),
        currentFrequency (500.0),
        targetFrequency (currentFrequency),
        currentLevel (0.1),
        targetLevel (currentLevel)
    {
        addAndMakeVisible(frequencySlider);
        frequencySlider.setRange(50, 5000);
        frequencySlider.setSkewFactor(500);
        frequencySlider.setValue(currentFrequency, dontSendNotification);
        frequencySlider.addListener(this);
        
        addAndMakeVisible(levelSlider);
        levelSlider.setRange(0.0, 0.125);
        levelSlider.setValue((double)currentLevel, dontSendNotification);
        levelSlider.addListener(this);
        
        setSize (600, 100);
        setAudioChannels (2, 2);
    }

    ~MainContentComponent()
    {
        shutdownAudio();
    }
    
    void resized() override
    {
        frequencySlider.setBounds (10, 10, getWidth() - 20, 20);
        levelSlider.setBounds (10, 40, getWidth() - 20, 20);
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        if (slider == &frequencySlider) {
            targetFrequency = frequencySlider.getValue();
//            if (currentSampleRate > 0.0)
//                updateAngleDelta();
        }
        else if (slider == &levelSlider)
            targetLevel = (float) levelSlider.getValue();
    }
    
    inline void updateAngleDelta()
    {
        const double cyclesPerSample = currentFrequency / currentSampleRate;
        angleDelta = cyclesPerSample * 2.0 * double_Pi;
    }
    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        currentSampleRate = sampleRate;
        updateAngleDelta();
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        float* const buffer = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
        
        const double localTargetFrequency = targetFrequency;
        
        if (targetFrequency != currentFrequency)
        {
            const double frequencyIncrement = (targetFrequency - currentFrequency) / bufferToFill.numSamples;
            
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                const float currentSample = (float) std::sin (currentAngle);
                currentFrequency += frequencyIncrement;
                updateAngleDelta();
                currentAngle += angleDelta;
                buffer[sample] = currentSample;
            }
            
            currentFrequency = localTargetFrequency;
        }
        else
        {
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                const float currentSample = (float) std::sin (currentAngle);
                currentAngle += angleDelta;
                buffer[sample] = currentSample;
            }
        }
        
        const float localTargetLevel = targetLevel;
        bufferToFill.buffer->applyGainRamp (bufferToFill.startSample, bufferToFill.numSamples, currentLevel, localTargetLevel);
        currentLevel = localTargetLevel;
    }
    
    void releaseResources() override
    {
        // This will be called when the audio device stops, or when it is being
        // restarted due to a setting change.

        // For more details, see the help for AudioProcessor::releaseResources()
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::black);


        // You can add your drawing code here!
    }


private:
    //==============================================================================

    // Your private member variables go here...
    Slider frequencySlider;
    Slider levelSlider;
    double currentSampleRate, currentAngle, angleDelta;
    double currentFrequency, targetFrequency;
    float currentLevel, targetLevel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
