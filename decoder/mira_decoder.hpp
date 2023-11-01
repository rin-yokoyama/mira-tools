
#ifndef __MIRA_DECODER_HPP__
#define __MIRA_DECODER_HPP__

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

namespace mira
{
    static const u_int32_t kDataSizeMask = 0x3fffff;
    static const u_int32_t kChannelIdMask = 0xf00;
    static const u_int32_t kChannelIdShift = 8;

    struct ChannelData
    {
        int ch_ = -1;
        int size_ = 0;
        u_int32_t *waveform_ = nullptr;
    };

    struct EventData
    {
        u_int64_t ts_ = 0;
        u_int32_t event_id_ = 0;
        std::vector<mira::ChannelData> data_;
    };

    /**
     * @brief Get the data size from a ridf header
     *
     * @param val: header
     * @return data size in 4bytes
     */
    u_int32_t get_size32(u_int32_t val)
    {
        return (val & mira::kDataSizeMask) / 2;
    }

    /**
     * @brief Get the channel id from a segment header
     *
     * @param segment_header
     * @return channel id
     */
    int get_channel_id(u_int32_t segment_header)
    {
        return (segment_header & mira::kChannelIdMask) >> mira::kChannelIdShift;
    }

    /**
     * @brief Decode one event of mira data
     *
     * @param buff: pointer to the buffer head
     * @param ch_flags: channel ids to read
     * @return int
     */
    mira::EventData decode_an_event(u_int32_t *buf, const std::vector<int> &ch_flags);

    std::vector<mira::EventData> decode_buffer(u_int32_t *buf, u_int32_t size, const std::vector<int> &ch_flags);

}

#endif /*__MIRA_DECODER_HPP__*/