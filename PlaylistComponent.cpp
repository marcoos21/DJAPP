/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 7 Mar 2023 9:42:08pm
    Author:  Marcus Mui

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent(DeckGUI* _deckGUI1,
                                     DeckGUI* _deckGUI2,
                                     DJAudioPlayer* _playerForParsingMetaData
                                    ) : deckGUI1(_deckGUI1),
                                        deckGUI2(_deckGUI2),
                                        playerForParsingMetaData(_playerForParsingMetaData)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    // add components
    addAndMakeVisible(importButton);
    addAndMakeVisible(searchField);
    addAndMakeVisible(library);
    addAndMakeVisible(addToPlayer1Button);
    addAndMakeVisible(addToPlayer2Button);

    // attach listeners
    importButton.addListener(this);
    searchField.addListener(this);
    addToPlayer1Button.addListener(this);
    addToPlayer2Button.addListener(this);

    // searchField configuration
    searchField.setTextToShowWhenEmpty("Search Tracks (enter to submit)",
                                       juce::Colours::white);
    searchField.onReturnKey = [this] { searchLibrary (searchField.getText()); };
    
    // setup table and load library from file
    library.getHeader().addColumn("Tracks", 1, 1);
    library.getHeader().addColumn("Length", 2, 1);
    library.getHeader().addColumn("", 3, 1);
    library.setModel(this);
    loadLibrary();
}

PlaylistComponent::~PlaylistComponent()
{
    saveLibrary();
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    auto colour1 = juce::Colours::red;
    auto colour2 = juce::Colours::purple;
    g.fillAll (colour1.interpolatedWith (colour2, 0.5f));
    g.setColour(colour1.interpolatedWith (colour2, 0.5f));
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);

}

void PlaylistComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    //                   x start, y start, width, height
    importButton.setBounds(0, 0, getWidth(), getHeight() / 16);
    library.setBounds(0, 1 * getHeight() / 16, getWidth(), 13 * getHeight() / 16);
    searchField.setBounds(0, 14 * getHeight() / 16, getWidth(), getHeight() / 16);
    addToPlayer1Button.setBounds(0, 15 * getHeight() / 16, getWidth() / 2, getHeight() / 16);
    addToPlayer2Button.setBounds(getWidth() / 2, 15 * getHeight() / 16, getWidth() / 2, getHeight() / 16);

    //set columns
    library.getHeader().setColumnWidth(1, 12.8 * getWidth() / 20);
    library.getHeader().setColumnWidth(2, 5 * getWidth() / 20);
    library.getHeader().setColumnWidth(3, 2 * getWidth() / 20);
    
    auto colour1 = juce::Colours::red;
    auto colour2 = juce::Colours::purple;
    importButton.setColour(juce::TextButton::ColourIds::buttonColourId, colour1.interpolatedWith(colour2, 0.5f));
    addToPlayer1Button.setColour(juce::TextButton::ColourIds::buttonColourId, colour1.interpolatedWith(colour2, 0.5f));
    addToPlayer2Button.setColour(juce::TextButton::ColourIds::buttonColourId, colour1.interpolatedWith(colour2, 0.5f));
    library.setColour(juce::ListBox::backgroundColourId, colour1.interpolatedWith (colour2, 0.5f));
}

int PlaylistComponent::getNumRows()
{
    return tracks.size();
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g,
                                           int rowNumber,
                                           int width,
                                           int height,
                                           bool rowIsSelected
                                          )
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::white);
    }
    else
    {
        auto colour1 = juce::Colours::red;
        auto colour2 = juce::Colours::purple;
        g.fillAll (colour1.interpolatedWith (colour2, 0.5f));
    }
}

void PlaylistComponent::paintCell(juce::Graphics& g,
                                  int rowNumber,
                                  int columnId,
                                  int width,
                                  int height,
                                  bool rowIsSelected
                                 )
{
    if (rowNumber < getNumRows())
    {
        if (columnId == 1)
        {
            g.drawText(tracks[rowNumber].title,
                2,
                0,
                width - 4,
                height,
                juce::Justification::centredLeft,
                true
            );
        }
        if (columnId == 2)
        {
            g.drawText(tracks[rowNumber].length,
                2,
                0,
                width - 4,
                height,
                juce::Justification::centred,
                true
            );
        }
    }
}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
                                                      int columnId,
                                                      bool isRowSelected,
                                                      Component* existingComponentToUpdate)
{
    if (columnId == 3)
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{ "X" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(juce::Button* button)
{
    if (button == &importButton)
    {
        DBG("Load button clicked");
        importToLibrary();
        library.updateContent();
    }
    else if (button == &addToPlayer1Button)
    {
        DBG("Add to Player 1 clicked");
        loadInPlayer(deckGUI1);
    }
    else if (button == &addToPlayer2Button)
    {
        DBG("Add to Player 2 clicked");
        loadInPlayer(deckGUI2);
    }
    else
    {
        int id = std::stoi(button->getComponentID().toStdString());
        DBG(tracks[id].title + " removed from Library");
        deleteFromTracks(id);
        library.updateContent();
    }
}

void PlaylistComponent::loadInPlayer(DeckGUI* deckGUI)
{
    int selectedRow{ library.getSelectedRow() };
    if (selectedRow != -1)
    {
        DBG("Adding: " << tracks[selectedRow].title << " to Player");
        deckGUI->loadFile(tracks[selectedRow].URL);
    }
    else
    {
        DBG("Add to Deck Information:");
    }
}

void PlaylistComponent::importToLibrary()
{
    DBG("PlaylistComponent::importToLibrary called");

    //initialize file chooser
    juce::FileChooser chooser{ "Select files" };
    juce::FileChooser choosers{ "Select for file(s) to add to the library...", juce::File() , "*.mp3;*.wav;*.aiff" };
    auto fileChooserFlags =
    juce::FileBrowserComponent::canSelectFiles;
    //fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
    fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
    {
        for (const juce::File& file : chooser.getResults())
        {
            juce::String fileNameWithoutExtension{ file.getFileNameWithoutExtension() };
            if (!isInTracks(fileNameWithoutExtension)) // if not already loaded
            {
                Track newTrack{ file };
                juce::URL audioURL{ file };
                newTrack.length = getLength(audioURL) ;
                tracks.push_back(newTrack);
                DBG("loaded file: " << newTrack.title);
            }
            else // display info message
            {
                DBG("Load information:");
            }
        }
    });
}

bool PlaylistComponent::isInTracks(juce::String fileNameWithoutExtension)
{
    return (std::find(tracks.begin(), tracks.end(), fileNameWithoutExtension) != tracks.end());
}

void PlaylistComponent::deleteFromTracks(int id)
{
    tracks.erase(tracks.begin() + id);
}

juce::String PlaylistComponent::getLength(juce::URL audioURL)
{
    playerForParsingMetaData->loadURL(audioURL);
    double seconds{ playerForParsingMetaData->getLengthInSeconds() };
    juce::String minutes{ secondsToMinutes(seconds) };
    return minutes;
}

juce::String PlaylistComponent::secondsToMinutes(double seconds)
{
    //find seconds and minutes and make into string
    int secondsRounded{ int(std::round(seconds)) };
    juce::String min{ std::to_string(secondsRounded / 60) };
    juce::String sec{ std::to_string(secondsRounded % 60) };
    
    if (sec.length() < 2) // if seconds is 1 digit or less
    {
        //add '0' to seconds until seconds is length 2
        sec = sec.paddedLeft('0', 2);
    }
    return juce::String{ min + ":" + sec };
}

void PlaylistComponent::searchLibrary(juce::String searchText)
{
    DBG("Searching library for: " << searchText);
    if (searchText != "")
    {
        int rowNumber = whereInTracks(searchText);
        library.selectRow(rowNumber);
    }
    else
    {
        library.deselectAllRows();
    }
}

int PlaylistComponent::whereInTracks(juce::String searchText)
{
    // finds index where track title contains searchText
    auto it = find_if(tracks.begin(), tracks.end(),
        [&searchText](const Track& obj) {return obj.title.contains(searchText); });
    int i = -1;

    if (it != tracks.end())
    {
        i = std::distance(tracks.begin(), it);
    }

    return i;
}

void PlaylistComponent::saveLibrary()
{
    // create .txt to save library
    std::ofstream myPlaylist("myPlaylist.txt");
    
    // save library to file
    for (Track& t : tracks)
    {
        DBG(t.file.getFullPathName());
        myPlaylist << t.file.getFullPathName() << "," << t.length << "\n";
    }
    
    myPlaylist.close();
}

void PlaylistComponent::loadLibrary()
{
    // create input stream from saved library
    std::ifstream myPlaylist("myPlaylist.txt");
    std::string filePath;
    std::string length;

    // Read data, line by line
    if (myPlaylist.is_open())
    {
        DBG("playlist open");
        while (getline(myPlaylist, filePath, ',')) {
            juce::File file{ filePath };
            Track newTrack{ file };

            getline(myPlaylist, length);
            newTrack.length = length;
            tracks.push_back(newTrack);
        }
    }
    else if(!myPlaylist.is_open())
    {
        DBG("file not open");
    }
    myPlaylist.close();
}

