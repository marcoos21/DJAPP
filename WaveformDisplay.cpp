/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 7 Mar 2023 9:41:46pm
    Author:  Marcus Mui

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(int _id,
                                 juce::AudioFormatManager& formatManager,
                                 juce::AudioThumbnailCache& thumbCache
                                ) : audioThumb(1000, formatManager, thumbCache),
                                    fileLoaded(false),
                                    position(0),
                                    id(_id)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId)); // clear the background
    //g.setColour (juce::Colours::grey);
    auto colour1 = juce::Colours::red;
    auto colour2 = juce::Colours::purple;
    g.fillAll (colour1.interpolatedWith (colour2, 0.5f));
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
    g.setColour(juce::Colours::white);

    g.setFont(18.0f);
    g.drawText("Deck: " + std::to_string(id), getLocalBounds(),
            juce::Justification::topLeft, true);

    if (fileLoaded)
    {
        g.setFont(15.0f);
        audioThumb.drawChannel(g,
                               getLocalBounds(),
                               0,
                               audioThumb.getTotalLength(),
                               0,
                               1.0f
                              );
        g.setColour(juce::Colours::white);
        g.drawRect(position * getWidth(), 0, getWidth() / 20, getHeight());
        g.setColour(juce::Colours::black);
        g.drawText(fileName, getLocalBounds(),
            juce::Justification::bottomLeft, true);
    }
    else
    {
        g.setFont(20.0f);
        auto colour1 = juce::Colours::red;
        auto colour2 = juce::Colours::purple;
        g.fillAll (colour1.interpolatedWith (colour2, 0.5f));
        g.drawText("File not loaded...", getLocalBounds(),
            juce::Justification::centred, true);   // draw some placeholder text
    }
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    repaint();
}

void WaveformDisplay::loadURL(juce::URL audioURL)
{
    DBG("WaveformDisplay::loadURL called");
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));
    if (fileLoaded)
    {
        DBG("WaveformDisplay::loadURL file loaded");
        fileName = audioURL.getFileName();
        repaint();
    }
    else
    {
        DBG("WaveformDisplay::loadURL file NOT loaded");
    }
}

void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }
}
