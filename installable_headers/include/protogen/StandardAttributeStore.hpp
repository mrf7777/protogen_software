#ifndef STANDARDATTRIBUTESTORE_HPP
#define STANDARDATTRIBUTESTORE_HPP

#include <protogen/IAttributeStore.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <mutex>
#include <functional>

namespace protogen {

/**
 * A standard implementation of IAttributeStore that is suitable for most use-cases.
 * 
 * This implementation uses a map to store attributes and their access levels.
 * It is also thread-safe and the underlying data structure is protected by a mutex.
 * 
 * This implementation also provides additional methods that are not part of the IAttributeStore interface,
 * because they are required for the implementation to be managed completely. Rest assured, consumers of this class as an
 * IAttributeStore will not be able to access these methods.
 * 
 */
class StandardAttributeStore : public attributes::IAttributeStore {
public:
    using ValueAndAccess = std::pair<std::string, Access>;
    using Attributes = std::unordered_map<std::string, ValueAndAccess>;
    using PinnedAttributes = std::unordered_set<std::string>;

    using SetAttributePreCallback = std::function<bool(const std::string&, const std::string&)>;
    using SetAttributePostCallback = std::function<void(const std::string&, const std::string&, SetAttributeResult)>;
    using RemoveAttributePreCallback = std::function<bool(const std::string&)>;
    using RemoveAttributePostCallback = std::function<void(const std::string&, RemoveAttributeResult)>;

    StandardAttributeStore()
        : m_attributes(),
        m_pinnedAttributes(),
        m_setAttributePreCallback([](const std::string&, const std::string&){ return true; }),
        m_setAttributePostCallback([](const std::string& , const std::string&, SetAttributeResult){}),
        m_removeAttributePreCallback([](const std::string&){ return true; }),
        m_removeAttributePostCallback([](const std::string&, RemoveAttributeResult){})
    {}

    //
    // IAttributeStore implementation.
    //

    SetAttributeResult setAttribute(const std::string& key, const std::string& value) override {
        if(!m_setAttributePreCallback(key, value)) {
            return SetAttributeResult::UnsetBecauseImplementation;
        }

        SetAttributeResult result;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            result = _setAttribute(key, value);
        }
        m_setAttributePostCallback(key, value, result);
        return result;
    }
    std::optional<std::string> getAttribute(const std::string& key) const override {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_attributes.contains(key)) {
            if(readable(m_attributes.at(key).second)) {
                return {m_attributes.at(key).first};
            }
        }
        return {};
    }
    std::vector<std::string> listAttributes() const override {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> attributes;
        for(const auto& [key, _] : m_attributes) {
            attributes.push_back(key);
        }
        return attributes;
    }
    std::optional<Access> getAttributeAccess(const std::string& key) const override {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_attributes.contains(key)) {
            return m_attributes.at(key).second;
        } else {
            return {};
        }
    }
    RemoveAttributeResult removeAttribute(const std::string& key) override {
        if(!m_removeAttributePreCallback(key)) {
            return RemoveAttributeResult::KeptBecauseImplementation;
        }

        RemoveAttributeResult result;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            result = _removeAttribute(key);
        }
        m_removeAttributePostCallback(key, result);
        return result;
    }
    Access getDefaultAccess() const override {
        return Access::ReadWrite;
    }
    bool hasAttribute(const std::string& key) const {
        return m_attributes.contains(key);
    }

    //
    // Implementation details.
    //

    bool pin(const std::string& key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_attributes.contains(key)) {
            m_pinnedAttributes.insert(key);
            return true;
        }
        return false;
    }
    bool unpin(const std::string& key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_pinnedAttributes.contains(key)) {
            m_pinnedAttributes.erase(key);
            return true;
        }
        return false;
    }

    void adminSetAttribute(const std::string& key, const std::string& value, Access access, bool respect_precall = true) {
        if(!m_setAttributePreCallback(key, value) && respect_precall) {
            return;
        }
        
        SetAttributeResult result;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if(m_attributes.contains(key)) {
                m_attributes[key] = std::make_pair(value, access);
                result = SetAttributeResult::Updated;
            } else {
                m_attributes[key] = std::make_pair(value, access);
                result = SetAttributeResult::Created;
            }
        }
        m_setAttributePostCallback(key, value, result);
    }
    std::optional<std::string> adminGetAttribute(const std::string& key) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_attributes.contains(key)) {
            return m_attributes.at(key).first;
        }
        return {};
    }
    bool adminRemoveAttribute(const std::string& key, bool respect_precall = true) {
        if(!m_removeAttributePreCallback(key) && respect_precall) {
            return false;
        }

        std::size_t elements_erased;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            elements_erased = m_attributes.erase(key);
            m_pinnedAttributes.erase(key);
        }
        if(elements_erased > 0) {
            m_removeAttributePostCallback(key, RemoveAttributeResult::Removed);
            return true;
        } else {
            m_removeAttributePostCallback(key, RemoveAttributeResult::DoesNotExist);
            return false;
        }
    }

    void setPreCallbackForSetAttribute(const SetAttributePreCallback& callback) {
        m_setAttributePreCallback = callback;
    }
    void setPostCallbackForSetAttribute(const SetAttributePostCallback& callback) {
        m_setAttributePostCallback = callback;
    }
    void setPreCallbackForSetAttribute(const RemoveAttributePreCallback& callback) {
        m_removeAttributePreCallback = callback;
    }
    void setPostCallbackForRemoveAttribute(const RemoveAttributePostCallback& callback) {
        m_removeAttributePostCallback = callback;
    }

private:
    mutable std::mutex m_mutex;
    Attributes m_attributes;
    PinnedAttributes m_pinnedAttributes;
    SetAttributePreCallback m_setAttributePreCallback;
    SetAttributePostCallback m_setAttributePostCallback;
    RemoveAttributePreCallback m_removeAttributePreCallback;
    RemoveAttributePostCallback m_removeAttributePostCallback;

    static bool readable(Access access) {
        switch(access) {
        case Access::Read:
        case Access::ReadWrite:
            return true;
        case Access::Write:
            return false;
        default:
            return false;
        }
    }

    static bool writable(Access access) {
        switch(access) {
        case Access::Write:
        case Access::ReadWrite:
            return true;
        case Access::Read:
            return false;
        default:
            return false;
        }
    }

    SetAttributeResult _setAttribute(const std::string& key, const std::string& value) {
        if(m_attributes.contains(key)) {
            if(writable(m_attributes.at(key).second)) {
                m_attributes.at(key).first = value;
                return SetAttributeResult::Updated;
            } else {
                return SetAttributeResult::UnsetBecauseNotWritable;
            }
        } else {
            m_attributes[key] = std::make_pair(value, getDefaultAccess());
            return SetAttributeResult::Created;
        }
    }

    RemoveAttributeResult _removeAttribute(const std::string& key) {
        if(m_attributes.contains(key)) {
            if(writable(m_attributes.at(key).second)) {
                if(!m_pinnedAttributes.contains(key)) {
                    m_attributes.erase(key);
                    return RemoveAttributeResult::Removed;
                } else {
                    return RemoveAttributeResult::KeptBecauseImplementation;
                }
            } else {
                return RemoveAttributeResult::KeptBecauseNotWritable;
            }
        } else {
            return RemoveAttributeResult::DoesNotExist;
        }
    }
};

} // namespace

#endif