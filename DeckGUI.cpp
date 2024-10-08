/*
  ==============================================================================

    DeckGUI.cpp
    Created: 7 Mar 2023 9:41:16pm
    Author:  Marcus Mui

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(int _id,
                 DJAudioPlayer* _player,
                 juce::AudioFormatManager& formatManager,
                 juce::AudioThumbnailCache& thumbCache
                ) : player(_player),
                    id(_id),
                    waveformDisplay(id, formatManager, thumbCache)
{
    // add all components and make visible
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(volLabel);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(posLabel);
    addAndMakeVisible(reverbPlot1);
    addAndMakeVisible(reverbPlot2);
    addAndMakeVisible(waveformDisplay);

    // add listeners
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
    reverbSlider.addListener(this);
    reverbPlot1.addListener(this);
    reverbPlot2.addListener(this);

    //configure buttons
    auto colour1 = juce::Colours::red;
    auto colour2 = juce::Colours::purple;
    playButton.setColour(juce::TextButton::ColourIds::buttonColourId, colour1.interpolatedWith(colour2, 0.5f));
    stopButton.setColour(juce::TextButton::ColourIds::buttonColourId, colour1.interpolatedWith(colour2, 0.5f));
    loadButton.setColour(juce::TextButton::ColourIds::buttonColourId, colour1.interpolatedWith(colour2, 0.5f));
    //configure volume slider and label
    double volDefaultValue = 0.5;
    volSlider.setRange(0.0, 1.0);
    volSlider.setNumDecimalPlacesToDisplay(2);
    volSlider.setSliderStyle(juce::Slider::Rotary);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxAbove,
                              false,
                              50,
                              volSlider.getTextBoxHeight()
                             );
    volSlider.setValue(volDefaultValue);
    volSlider.setSkewFactorFromMidPoint(volDefaultValue);
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.attachToComponent(&volSlider, false);

    //configure speed slider and label
    double speedDefaultValue = 1.0;
    speedSlider.setRange(0.25, 4.0); //reaches breakpoint if sliderValue == 0
    speedSlider.setNumDecimalPlacesToDisplay(2);
    speedSlider.setSliderStyle(juce::Slider::Rotary);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxAbove,
                              false,
                              50,
                              speedSlider.getTextBoxHeight()
                             );
    speedSlider.setValue(speedDefaultValue);
    speedSlider.setSkewFactorFromMidPoint(speedDefaultValue);
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.attachToComponent(&speedSlider, false);

    //configure position slider and label
    posSlider.setRange(0.0, 1.0);
    posSlider.setNumDecimalPlacesToDisplay(2);
    posSlider.setSliderStyle(juce::Slider::Rotary);
    posSlider.setTextBoxStyle(juce::Slider::TextBoxAbove,
                              false,
                              50,
                              posSlider.getTextBoxHeight()
                             );
    posLabel.setText("Position", juce::dontSendNotification);
    posLabel.attachToComponent(&posSlider, false);

    //configure reverb slider
    reverbSlider.setRange(0.0, 1.0);
    reverbSlider.setNumDecimalPlacesToDisplay(2);

    //configure reverb plots
    reverbPlot1.setTooltip("x: damping\ny: room size");
    reverbPlot2.setTooltip("x: dry level\ny: wet level");

    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    auto colour1 = juce::Colours::red;
    auto colour2 = juce::Colours::purple;
    g.fillAll (colour1.interpolatedWith (colour2, 0.5f));
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    
}

void DeckGUI::resized()
{
     /*This method is where you should set the bounds of any child
     components that your component contains..*/
    //auto sliderLeft = getWidth() / 9;
    auto mainRight = getWidth() - getHeight() / 2;
    auto plotRight = getWidth() - mainRight; // should == getHeight() / 2

    //                   x start, y start, width, height
    playButton.setBounds(0, 0, mainRight / 3, getHeight() / 8);
    stopButton.setBounds(mainRight / 3, 0, mainRight / 3, getHeight() / 8);
    loadButton.setBounds(2 * mainRight / 3, 0, mainRight / 3, getHeight() / 8);
 
    volSlider.setBounds(-80, getHeight()/7, getWidth()/2, getHeight()/7*3);
    volLabel.setCentreRelative(0.34f, 0.38f);
    
    speedSlider.setBounds(95, getHeight()/7, getWidth()/2, getHeight()/7*3);
    speedLabel.setCentreRelative(0.605f, 0.38f);

    posSlider.setBounds(270, getHeight()/7, getWidth()/2, getHeight()/7*3);
    posLabel.setCentreRelative(0.851f, 0.38f);
    
    reverbPlot1.setBounds(mainRight, 0, plotRight, getHeight() / 2);
    reverbPlot2.setBounds(mainRight, getHeight()/2, plotRight, getHeight() / 2);
    waveformDisplay.setBounds(0, 4 * getHeight() / 8, mainRight, 4 * getHeight() / 8);
}

void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        DBG("Play button was clicked ");
        player->play();
    }
    if (button == &stopButton)
    {
        DBG("Stop button was clicked ");
        player->stop();
    }
    if (button == &loadButton)
    {
        DBG("Load button was clicked ");
        //juce::FileChooser chooser{"Select a file"};
        auto fileChooserFlags =
        juce::FileBrowserComponent::canSelectFiles;
        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
        {
            player->loadURL(juce::URL{chooser.getResult()});
            // and now the waveformDisplay as well
            waveformDisplay.loadURL(juce::URL{chooser.getResult()});
            DBG(juce::URL{ chooser.getResult() }.getFileName());
            
        });
    }
}


void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volSlider)
    {
        DBG("Volume slider moved " << slider->getValue());
        player->setGain(slider->getValue());
    }
    if (slider == &speedSlider)
    {
        DBG("Speed slider moved " << slider->getValue());
        player->setSpeed(slider->getValue());
    }
    if (slider == &posSlider)
    {
        DBG("Position slider moved " << slider->getValue());
        player->setPositionRelative(slider->getValue());
    }
}

void DeckGUI::coordPlotValueChanged(CoordinatePlot* coordinatePlot)
{
    DBG("DeckGUI::coordPlotValueChanged called");
    if (coordinatePlot == &reverbPlot1)
    {
        DBG("Deck " << id << ": ReverbPlot1 was clicked");
        player->setRoomSize(coordinatePlot->getY());
        player->setDamping(coordinatePlot->getX());
    }
    if (coordinatePlot == &reverbPlot2)
    {
        DBG("Deck " << id << ": ReverbPlot2 was clicked");
        player->setWetLevel(coordinatePlot->getY());
        player->setDryLevel(coordinatePlot->getX());
    }
}

bool DeckGUI::isInterestedInFileDrag(const juce::StringArray& files)
{
    DBG("DeckGUI::isInterestedInFileDrag called. "
        + std::to_string(files.size()) + " file(s) being dragged.");
    return true;
}

void DeckGUI::filesDropped(const juce::StringArray& files, int x, int y)
{

    DBG("DeckGUI::filesDropped at " + std::to_string(x)
        + "x and " + std::to_string(y) + "y" );
    if (files.size() == 1)
    {
        loadFile(juce::URL{ juce::File{files[0]} });
    }
}

void DeckGUI::loadFile(juce::URL audioURL)
{
    DBG("DeckGUI::loadFile called");
    player->loadURL(audioURL);
    waveformDisplay.loadURL(audioURL);
}

void DeckGUI::timerCallback()
{
    //check if the relative position is greater than 0
    //otherwise loading file causes error
    if (player->getPositionRelative() > 0)
    {
        waveformDisplay.setPositionRelative(player->getPositionRelative());
    }
}
