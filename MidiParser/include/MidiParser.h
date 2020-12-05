#pragma once

#include <string>
#include <tuple>
#include <vector>

#include "MidiTrack.h"
#include "Instruments.h"

class MidiParser {
private:
    uint8_t* m_Buffer = nullptr;
    size_t m_ReadPosition = 0;

    std::vector<TempoEvent> m_TempoList;
    std::vector<MidiTrack> m_TrackList;

    uint16_t m_Format = 0, m_TrackCount = 0, m_Division = 0;
    uint32_t m_TotalTicks = 0;  // Duration of MIDI file in ticks
    uint64_t m_Duration = 0;  // Duration of MIDI fild in microseconds

    MidiEventType m_RunningStatus = MidiEventType::None;  // Current running status

    bool m_ErrorStatus = true;  // True if no error
public:
    MidiParser() = default;
    MidiParser(const std::string& file);

    ~MidiParser() = default;

    bool Open(const std::string& file);

    inline uint16_t GetFormat() const { return m_Format; }
    inline uint16_t GetDivision() const { return m_Division; }
    inline uint16_t GetTrackCount() const { return m_TrackCount; }

    inline uint32_t GetDurationSeconds() { return (uint32_t)(m_Duration / 1000000); }
    std::pair<uint32_t, uint32_t> GetDurationMinutesAndSeconds();

    MidiTrack& operator[](size_t index) { return m_TrackList[index]; }

    std::vector<MidiTrack>::iterator begin() { return m_TrackList.begin(); }
    std::vector<MidiTrack>::iterator end() { return m_TrackList.end(); }

    std::vector<MidiTrack>::reverse_iterator rbegin() { return m_TrackList.rbegin(); }
    std::vector<MidiTrack>::reverse_iterator rend() { return m_TrackList.rend(); }

    std::vector<MidiTrack>::const_iterator cbegin() { return m_TrackList.cbegin(); }
    std::vector<MidiTrack>::const_iterator cend() { return m_TrackList.cend(); }
private:
    enum class MidiEventStatus : int8_t {
        Error,
        Success,
        End
    };
private:
    bool ReadFile();
    bool ReadTrack();
    MidiEventStatus ReadEvent(MidiTrack& track);  // Reads a single event

    inline MidiTrack& AddTrack() { return m_TrackList.emplace_back(); }

    int32_t ReadVariableLengthValue();  // Returns -1 if invalid

    // Reads type T from file and converts to little endian if necessary
    template<typename T>
    T ReadInteger(T* destination = nullptr);
    inline void ReadBytes(void* buffer, size_t size);

    inline float TicksToMicroseconds(uint32_t ticks, uint32_t tempo);

    inline void Error(const std::string& msg);
};
