#include "mira_decoder.hpp"
#include <sstream>

mira::EventData mira::decode_an_event(u_int32_t *buf, u_int32_t &size, const std::vector<int> &ch_flags)
{
    int idx = -1;
    auto next = [&buf, &idx]()
    { return buf[++idx]; };
    auto skip = [&idx](int n)
    { idx = idx + n; };

    const auto event_size = mira::get_size32(next());
    u_int32_t addr = next();
    const u_int32_t event_n = next();
    const u_int64_t ts = (((u_int64_t)next()) << 32) + next();

    mira::EventData eventData;
    eventData.event_id_ = event_n;
    eventData.ts_ = ts;
    eventData.data_.clear();

    while (idx < event_size - 1)
    {
        const auto segment_size = mira::get_size32(next());
        if (!segment_size)
            break;
        const auto end = idx + segment_size - 1;
        if (end > event_size - 1)
            break;
        addr = next();
        const auto segment_header = next();
        const auto channel_id = mira::get_channel_id(segment_header);
        if (std::find(ch_flags.begin(), ch_flags.end(), channel_id) != ch_flags.end())
        {
            ChannelData data;
            void *array = buf + idx + 1;
            data.ch_ = channel_id;
            data.efn_ = addr;
            data.size_ = 2 * (segment_size - 3);
            data.waveform_ = (u_int16_t *)(array);
            if (mira::kQDCADCFlag)
            {
                data.waveform_float_.insert(data.waveform_float_.end(), (float *)array, ((float *)array) + data.size_);
                data.qdc_ = mira::calculate_qdc(data.waveform_float_);
                data.adc_ = mira::calculate_adc(data.waveform_float_);
            }
            eventData.data_.emplace_back(data);
            skip(segment_size - 3);
        }
        else
        {
            skip(segment_size - 3);
        }
    }
    size = size + event_size;
    return eventData;
}

std::vector<mira::EventData> mira::decode_buffer(u_int32_t *buf, u_int32_t size, const std::vector<int> &ch_flags)
{
    std::vector<mira::EventData> buff_data;
    if (size < 1)
        return buff_data;
    u_int32_t idx = 0;
    while (idx < size)
    {
        const u_int32_t event_size = mira::get_size32(buf[idx]);
        if (!event_size)
            break;
        const u_int32_t addr = buf[idx + 1];
        u_int32_t idx_evnt = 2;
        while (idx_evnt < event_size - 1)
        {
            buff_data.emplace_back(mira::decode_an_event(buf + idx + idx_evnt, idx_evnt, ch_flags));
        }
        idx = idx + event_size;
    }
    return buff_data;
}

void mira::write_event_data_to_json(std::ofstream &ofs, const std::vector<mira::EventData> &data)
{
    ofs << event_data_to_json(data);
}

std::string mira::event_data_to_json(const std::vector<mira::EventData> &data)
{
    std::ostringstream ofs;
    ofs << "[" << std::endl;
    for (const auto &evt : data)
    {
        ofs << "{" << std::endl;
        ofs << "\"event_id\": " << evt.event_id_ << "," << std::endl;
        ofs << "\"ts\": " << evt.ts_ << "," << std::endl;
        ofs << "\"data\": [" << std::endl;
        for (const auto &ch_data : evt.data_)
        {
            ofs << "{\"efn\": " << ch_data.efn_ << "," << std::endl;
            ofs << "\"channel\": " << ch_data.ch_ << "," << std::endl;
            if (mira::kQDCADCFlag)
            {
                ofs << "\"qdc\": " << ch_data.qdc_ << "," << std::endl;
                ofs << "\"adc\": " << ch_data.adc_ << "," << std::endl;
            }
            if (!ch_data.waveform_float_.empty())
            {
                ofs << "\"waveform\": [";
                for (const auto &sample : ch_data.waveform_float_)
                {
                    ofs << sample << ",";
                }
            }
            else
            {
                ofs << "\"waveform\": [";
                for (int i = 0; i < ch_data.size_; ++i)
                {
                    ofs << ch_data.waveform_[i] << ",";
                }
            }
            ofs.seekp(-1, std::ios_base::cur);
            ofs << "]},\n";
        }
        ofs.seekp(-2, std::ios_base::cur);
        ofs << "\n]},\n";
    }
    ofs.seekp(-2, std::ios_base::cur);
    ofs << "\n]" << std::endl;

    return ofs.str();
}