/*
  ==============================================================================

    Track.h
    Created: 7 Mar 2023 9:42:18pm
    Author:  Marcus Mui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Track
{
    public:
        Track(juce::File _file);
        juce::File file;
        juce::URL URL;
        juce::String title;
        juce::String length;
        /**objects are compared by title*/
        bool operator==(const juce::String& other) const;
};
