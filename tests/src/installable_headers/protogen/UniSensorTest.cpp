#include <chrono>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <protogen/ISensor.hpp>
#include <protogen/StandardAttributes.hpp>
#include <protogen/StandardAttributeStore.hpp>
#include <protogen/UniSensor.hpp>

using namespace protogen;
using namespace protogen::sensor;

class FakeSensor : public ISensor {
public:
    ~FakeSensor() = default;
    FakeSensor() {}
    Initialization initialize() override {
        return Initialization::Success;
    }
    std::optional<std::string> getAttribute(const std::string& key) const override {
        return m_attributes.getAttribute(key);
    }
    std::vector<std::string> listAttributes() const override {
        return m_attributes.listAttributes();
    }
    bool hasAttribute(const std::string& key) const override {
        return m_attributes.hasAttribute(key);
    }
    SetAttributeResult setAttribute(const std::string& key, const std::string& value) override {
        return m_attributes.setAttribute(key, value);
    }
    RemoveAttributeResult removeAttribute(const std::string& key) override {
        return m_attributes.removeAttribute(key);
    }

    std::vector<ChannelInfo> channels() const override {
        return m_channelInfo;
    }
    ReadResult read(const std::string& channel_id) override {
        if (m_channelValues.contains(channel_id)) {
            return ReadResult{
                std::chrono::system_clock::now(),
                m_channelValues[channel_id],
                {}
            };
        }
        return ReadResult{
            std::chrono::system_clock::now(),
            {},
            ReadError::ChannelNotFound
        };
    }

    // management of fake sensor
    void addChannel(const std::string& id, const std::string& description, ChannelValue value) {
        m_channelInfo.push_back({id, description});
        m_channelValues[id] = value;
    }
    void addChannel(const ChannelInfo& channel, ChannelValue value) {
        m_channelInfo.push_back(channel);
        m_channelValues[channel.id] = value;
    }

private:
    StandardAttributeStore m_attributes;
    std::vector<ChannelInfo> m_channelInfo;
    std::map<std::string, ChannelValue> m_channelValues;    // mock values to use for channel ids
};

class UniSensorTest : public testing::Test {
protected:
    UniSensorTest() {
        using namespace protogen::sensor;
        using namespace protogen::attributes;

        auto sensor1 = std::make_shared<FakeSensor>();
        auto sensor2 = std::make_shared<FakeSensor>();
        m_sensor1 = sensor1;
        m_sensor2 = sensor2;

        sensor1->addChannel("std:in.temperature", "Internal temperature measured in Celsius.", ChannelValue(20.0));
        sensor1->addChannel("std:out.temperature", "External temperature measured in Celsius.", ChannelValue(30.0));

        sensor2->addChannel("std:in.temperature", "Internal temperature measured in Celsius.", ChannelValue(25.0));
        sensor2->addChannel("std:in.humidity_relative", "External temperature measured in Celsius.", ChannelValue(35.0));
    }

    std::shared_ptr<ISensor> m_sensor1;
    std::shared_ptr<ISensor> m_sensor2;
    std::shared_ptr<UniSensor> m_uniSensor;
};

TEST_F(UniSensorTest, Sensor1FirstTest) {
    std::vector<std::shared_ptr<ISensor>> sensors{m_sensor1, m_sensor2};
    m_uniSensor = std::make_shared<UniSensor>(sensors);

    const auto value_in_temp = m_uniSensor->read("std:in.temperature");
    const auto value_out_temp = m_uniSensor->read("std:out.temperature");
    const auto value_in_humidity = m_uniSensor->read("std:in.humidity_relative");
    const auto value_non_existent = m_uniSensor->read("std:blah.blah");

    ASSERT_TRUE(value_in_temp.value.has_value());
    ASSERT_TRUE(value_out_temp.value.has_value());
    ASSERT_TRUE(value_in_humidity.value.has_value());
    ASSERT_FALSE(value_non_existent.value.has_value());

    EXPECT_EQ(value_in_temp.value.value(), ChannelValue(20.0));
    EXPECT_EQ(value_out_temp.value.value(), ChannelValue(30.0));
    EXPECT_EQ(value_in_humidity.value.value(), ChannelValue(35.0));
}

TEST_F(UniSensorTest, Sensor2FirstTest) {
    std::vector<std::shared_ptr<ISensor>> sensors{m_sensor2, m_sensor1};
    m_uniSensor = std::make_shared<UniSensor>(sensors);

    const auto value_in_temp = m_uniSensor->read("std:in.temperature");
    const auto value_out_temp = m_uniSensor->read("std:out.temperature");
    const auto value_in_humidity = m_uniSensor->read("std:in.humidity_relative");
    const auto value_non_existent = m_uniSensor->read("std:blah.blah");

    ASSERT_TRUE(value_in_temp.value.has_value());
    ASSERT_TRUE(value_out_temp.value.has_value());
    ASSERT_TRUE(value_in_humidity.value.has_value());
    ASSERT_FALSE(value_non_existent.value.has_value());

    EXPECT_EQ(value_in_temp.value.value(), ChannelValue(25.0));
    EXPECT_EQ(value_out_temp.value.value(), ChannelValue(30.0));
    EXPECT_EQ(value_in_humidity.value.value(), ChannelValue(35.0));
}