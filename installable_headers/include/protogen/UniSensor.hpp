#ifndef PROTOGEN_UNISENSOR_HPP
#define PROTOGEN_UNISENSOR_HPP

#include <vector>
#include <unordered_map>
#include <memory>
#include <set>
#include <chrono>

#include <protogen/ISensor.hpp>
#include <protogen/StandardAttributeStore.hpp>
#include <protogen/StandardAttributes.hpp>

namespace protogen::sensor {

/**
 * A sensor interface that combines multiple sensors into one.
 * Sensors are expected to be initialized before they are added to the UniSensor.
 * The UniSensor always has a blank id. It is an adapter for multiple sensors.
 * The `initialize` method of the UniSensor does nothing.
 * If more than one sensor provides the same channel, the first sensor in the
 * list that provides the channel will be used.
 */
class UniSensor : public ISensor {
public:
    explicit UniSensor(const std::vector<std::shared_ptr<ISensor>>& sensors) : m_channels(), m_attributes(new StandardAttributeStore()) {
        for(const auto& sensor : sensors) {
            const auto channels = sensor->channels();
            for(const auto& channel : channels) {
                if(m_channels.contains(channel.id)) {
                    continue;
                }
                m_channels[channel.id] = std::make_pair(channel, sensor);
            }
        }

        m_attributes->setAttribute(attributes::A_ID, "");
        m_attributes->setAttribute(attributes::A_NAME, "UniSensor");
        m_attributes->setAttribute(attributes::A_DESCRIPTION, "A sensor that combines multiple sensors into one. Does not have an ID.");
    }

    Initialization initialize() override {
        return Initialization::Success;
    }

    /**
     * Get the channels of all sensors.
     * If a channel is provided by more than one sensor, the first sensor
     * in the list that provides the channel will be used.
     */
    std::vector<ChannelInfo> channels() const override {
        std::vector<ChannelInfo> all_channels;
        for(const auto& [_, channel_sensor_pair] : m_channels) {
            all_channels.push_back(channel_sensor_pair.first);
        }
        return all_channels;
    }

    std::optional<ChannelInfo> channelInfo(const std::string& channel_id) const override {
        if(m_channels.contains(channel_id)) {
            return m_channels.at(channel_id).first;
        }
        return {};
    }

    ReadResult read(const std::string& channel_id) override {
        if(m_channels.contains(channel_id)) {
            return m_channels.at(channel_id).second->read(channel_id);
        }
        return ReadResult{
            std::chrono::system_clock::now(),
            {},
            ReadError::ChannelNotFound
        };
    }

    std::optional<std::string> getAttribute(const std::string& key) const override {
        return m_attributes->getAttribute(key);
    }

    std::vector<std::string> listAttributes() const override {
        return m_attributes->listAttributes();
    }

    bool hasAttribute(const std::string& key) const override {
        return m_attributes->hasAttribute(key);
    }

    SetAttributeResult setAttribute(const std::string& key, const std::string& value) override {
        return m_attributes->setAttribute(key, value);
    }
    
    RemoveAttributeResult removeAttribute(const std::string& key) override {
        return m_attributes->removeAttribute(key);
    }

private:
    using ChannelSensorPair = std::pair<ChannelInfo, std::shared_ptr<ISensor>>;
    std::unordered_map<std::string, ChannelSensorPair> m_channels;
    std::shared_ptr<StandardAttributeStore> m_attributes;
};

} // namespace

#endif