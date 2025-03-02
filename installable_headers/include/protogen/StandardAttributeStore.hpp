#ifndef STANDARDATTRIBUTESTORE_HPP
#define STANDARDATTRIBUTESTORE_HPP

#include <protogen/IAttributeStore.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <utility>
#include <mutex>
#include <shared_mutex>

namespace protogen {

/**
 * A standard implementation of IAttributeStore that is suitable for most
 * use-cases. This implementation is thread-safe and can be shared and used
 * by multiple threads at once.
 * 
 */
class StandardAttributeStore : public attributes::IAttributeStore {
public:
    StandardAttributeStore()
        : m_attributes()
    {}

    SetAttributeResult setAttribute(const std::string& key, const std::string& value) override {
        std::unique_lock lock(m_mutex);
        if(m_attributes.contains(key)) {
            m_attributes[key] = value;
            return SetAttributeResult::Updated;
        } else {
            m_attributes[key] = value;
            return SetAttributeResult::Created;
        }
    }

    std::optional<std::string> getAttribute(const std::string& key) const override {
        std::shared_lock lock(m_mutex);
        if(m_attributes.contains(key)) {
            return m_attributes.at(key);
        } else {
            return {};
        }
    }

    std::vector<std::string> listAttributes() const override {
        std::unique_lock lock(m_mutex);
        std::vector<std::string> attributes;
        for(const auto& [key, _] : m_attributes) {
            attributes.push_back(key);
        }
        return attributes;
    }

    RemoveAttributeResult removeAttribute(const std::string& key) override {
        std::unique_lock lock(m_mutex);
        if(m_attributes.contains(key)) {
            m_attributes.erase(key);
            return RemoveAttributeResult::Removed;
        } else {
            return RemoveAttributeResult::DoesNotExist;
        }
    }

    bool hasAttribute(const std::string& key) const {
        std::shared_lock lock(m_mutex);
        return m_attributes.contains(key);
    }

private:
    mutable std::shared_mutex m_mutex;
    std::unordered_map<std::string, std::string> m_attributes;
};

} // namespace

#endif