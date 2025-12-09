#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
   : decks(), active_deck(1), auto_sync(false), bpm_tolerance(0)
{
    // Your implementation here
    decks[0] = nullptr;
    decks[1] = nullptr;
    std::cout << "[MixingEngineService] Initialized with "  << 2  << " empty decks" << std::endl;
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    // Your implementation here
    std::cout << "[MixingEngineService] Cleaning up decks..." << std::endl;
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i]) {
            delete decks[i];
            decks[i] = nullptr;
        }
    }
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    // Your implementation here
   
    std::cout << "\n=== Loading Track to Deck ===" << std::endl;
    PointerWrapper<AudioTrack> cloned_track = track.clone();
    if(!cloned_track){
        std::cerr << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone " << std::endl;
        return -1;
    }
    size_t target_deck = 1 - active_deck;
    std::cout << "[Deck Switch] Target deck: " << target_deck << std::endl;
    // Unload  target deck if occupied
    if(decks[target_deck] != nullptr){
        delete decks[target_deck];
        decks[target_deck] = nullptr;
    }
    cloned_track->load();
    cloned_track->analyze_beatgrid();

     //BPM Management
     if(decks[active_deck] != nullptr && auto_sync){
        int active_bpm = decks[active_deck]->get_bpm();
        int track_bpm = cloned_track->get_bpm();
        int bpm_diff = active_bpm - track_bpm;
        if(bpm_diff < 0) bpm_diff = -bpm_diff;

         if(bpm_diff > bpm_tolerance){
            sync_bpm(cloned_track); 
         }
    }
    decks[target_deck] = cloned_track.release();
    std::cout << "[Load Complete] '" << track.get_title() << "' is now loaded on deck " << target_deck << std::endl;

    active_deck = target_deck;
    std::cout << "[Active Deck] Switched to deck " << target_deck << std::endl;
    return target_deck;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    if(!decks[active_deck]) {
        return false;
    }
    if(!track){
        return false;
    }
    int active_bpm = decks[active_deck]->get_bpm();
    int track_bpm = track->get_bpm();
    int bpm_diff = std::abs(active_bpm - track_bpm); 
    return bpm_diff <= bpm_tolerance;
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    if(decks[active_deck] != nullptr && track){       
        int original_bpm = track->get_bpm();
        int active_bpm = decks[active_deck]->get_bpm();
        int new_bpm = (active_bpm + original_bpm) / 2;
        track->set_bpm(new_bpm);
        std::cout << "[Sync BPM] Syncing BPM from " << original_bpm << " to " << new_bpm << " bpm\n";
    }
}
