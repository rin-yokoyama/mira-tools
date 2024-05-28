/**
 * @file mira_decoder.hpp
 * @author Rin Yokoyama (yokoyama@cns.s.u-tokyo.ac.jp)
 * @brief Decoder library for mira data
 * @version 0.1
 * @date 2023-11-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __MIRA_DECODER_HPP__
#define __MIRA_DECODER_HPP__

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "mira_constants.h"

namespace mira
{
    static const u_int32_t kDataSizeMask = 0x3fffff; // 22 bit for data size
    static const u_int32_t kChannelIdMask = 0xf00;
    static const u_int32_t kChannelIdShift = 8;

    struct ChannelData
    {
        int ch_ = -1;
        int efn_ = -1;
        int size_ = 0;
        int qdc_ = 0;
        int adc_ = 0;
        u_int16_t *waveform_ = nullptr;
        std::vector<u_int16_t> waveform_vec_;
    };

    struct EventData
    {
        u_int64_t ts_ = 0;
        u_int32_t event_id_ = 0;
        std::vector<mira::ChannelData> data_;
    };

    /**
     * @brief Calulate ADC (Max sample value)
     *
     * @param waveform : waveform vector
     * @return double : maximum element, baseline subtracted.
     */
    inline double calculate_adc(const std::vector<u_int16_t> &waveform, int polarity = 1)
    {
        if (waveform.empty())
            return 0;
        const int pre = waveform.size() * mira::kPrePulseRatio;
        const double ave = std::accumulate(waveform.begin(), waveform.begin() + pre, 0.0f) / pre;
        if (polarity > 0)
            return *std::max_element(waveform.begin(), waveform.end()) - ave;
        else
            return ave - (*std::min_element(waveform.begin(), waveform.end()));
    }

    /**
     * @brief Calculate QDC (Average value of the waveform samples between kPrePulseRatio and kAfterPulesRatio)
     *
     * @param waveform : waveform vector
     * @return double : qdc, baseline subtracted.
     */
    inline double calculate_qdc(const std::vector<u_int16_t> &waveform, int polarity = 1)
    {
        if (waveform.empty())
            return 0;
        const int pre = waveform.size() * mira::kPrePulseRatio;
        const int after = waveform.size() * mira::kAfterPulseRatio;
        const double ave = std::accumulate(waveform.begin() + pre, waveform.begin() + after, 0.0f) / (after - pre);
        const double ave_pre = std::accumulate(waveform.begin(), waveform.begin() + pre, 0.0f) / pre;
        if (polarity > 0)
            return ave - ave_pre;
        else
            return ave_pre - ave;
    }

    /**
     * @brief Get the data size from a ridf header
     *
     * @param val: header
     * @return data size in 4bytes
     */
    inline u_int32_t get_size32(u_int32_t val)
    {
        return (val & mira::kDataSizeMask) / 2;
    }

    /**
     * @brief Get the channel id from a segment header
     *
     * @param segment_header
     * @return channel id
     */
    inline int get_channel_id(u_int32_t segment_header)
    {
        return (segment_header & mira::kChannelIdMask) >> mira::kChannelIdShift;
    }

    /**
     * @brief Decode one event of mira data
     *
     * @param buff: pointer to the buffer head (exclude event header)
     * @param ch_flags: channel ids to read
     * @return int
     */
    mira::EventData decode_an_event(u_int32_t *buf, u_int32_t &size, const std::vector<int> &ch_flags);

    /**
     * @brief Docode a data block containing multiple event data
     *
     * @param buf: pointer to the buffer head
     * @param size: size of the buffer in 32bit
     * @param ch_flags: channel ids to read
     * @return std::vector<mira::EventData> array of decoded event data
     */
    std::vector<mira::EventData> decode_buffer(u_int32_t *buf, u_int32_t size, const std::vector<int> &ch_flags);

    /**
     * @brief Write event data contents to ofs in json format
     *
     * @param ofs: output file stream
     * @param data: a vector of decoded data
     */
    void write_event_data_to_json(std::ofstream &ofs, const std::vector<mira::EventData> &data);

    /**
     * @brief convert event data contents to string in json format
     *
     * @param data: a vector of decoded data
     */
    std::string event_data_to_json(const std::vector<mira::EventData> &data);
}

#endif /*__MIRA_DECODER_HPP__*/