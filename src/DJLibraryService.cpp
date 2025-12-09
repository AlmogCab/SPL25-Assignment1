#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}


 // Destructor to clean up owned AudioTrack pointers   
DJLibraryService::~DJLibraryService() {
    for (auto track : library) {
        delete track;
    }
    library.clear();
}

//Copy Constructor for rule of 3
DJLibraryService::DJLibraryService(const DJLibraryService& other)
  : playlist(other.playlist), library() {
    for (const auto& track : other.library) {
        AudioTrack* track_copy = track->clone().release();
        library.push_back(track_copy);
    }
}

//Copy Assignment Operator for rule of 3
DJLibraryService& DJLibraryService::operator=(const DJLibraryService& other) {
    if (this == &other) {
        return *this;
    }
    // Clean up existing library tracks
    for (auto track : library) {
        delete track;
    }
    library.clear();
    playlist = other.playlist;
    for (const auto& track : other.library) {
        AudioTrack* track_copy = track->clone().release();
        library.push_back(track_copy);
    }
    return *this;
}


/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    //Todo: Implement buildLibrary method
    std::cout << "TODO: Implement DJLibraryService::buildLibrary method\n"<< library_tracks.size() << " tracks to be loaded into library.\n";
    int count = 0;
    for(auto& track_info : library_tracks){
        AudioTrack* created_track = nullptr;
        if(track_info.type == "MP3"){
            created_track = new MP3Track(track_info.title, track_info.artists, track_info.duration_seconds, track_info.bpm, track_info.extra_param1, track_info.extra_param2);
            std::cout << "MP3Track created: " << track_info.extra_param1 << "kbps" << std::endl;
        }
        else if(track_info.type == "WAV"){
           created_track = new WAVTrack(track_info.title, track_info.artists, track_info.duration_seconds, track_info.bpm, track_info.extra_param1, track_info.extra_param2);
            std::cout << "WAVTrack created: " << track_info.extra_param1 << "Hz/" << track_info.extra_param2 << "bit" << std::endl;
        } 
        if(created_track){
            library.push_back(created_track);
            count++;
        }
      }
    std::cout << " [INFO] Track library built: " << count << " tracks loaded " << std::endl;
 }

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    // Your implementation here
    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    // Your implementation here
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;
    Playlist new_playlist(playlist_name);
    size_t library_size = library.size();
    int count = 0;

    for(int index : track_indices){
        if(index < 1 || (size_t)index > library_size){
            std::cout <<"[WARNING] Invalid track index: " << index << std::endl;
            continue;
        }
        AudioTrack*  track_to_add = library[index - 1];
        PointerWrapper<AudioTrack> cloned_track = track_to_add->clone();
        if(!cloned_track.get()){
            std::cout << "[ERROR] Failed to clone track: " << track_to_add->get_title() << std::endl;
            continue;
        }
        cloned_track->load();
        cloned_track->analyze_beatgrid();
        new_playlist.add_track(cloned_track.release());
        count++;
    }

    playlist = new_playlist;
    std::cout << "[INFO] Playlist loaded: " << playlist_name <<"(" << count << " tracks )" << std::endl;
  }

/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    // Your implementation here
    std::vector<std::string> titles;
    std::vector<AudioTrack*> tracks = playlist.getTracks();
    for(auto track : tracks){
        titles.push_back(track->get_title());
    }
    return titles;
}