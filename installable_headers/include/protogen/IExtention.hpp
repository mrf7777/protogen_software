#ifndef PROTOGEN_IEXTENTION_HPP
#define PROTOGEN_IEXTENTION_HPP

#include <protogen/IAttributeStore.hpp>

#include <memory>

namespace protogen {

class IExtention {
public:
    enum class Initialization {
        Success,
        Failure,
    };

    virtual ~IExtention() = default;

    /**
     * Initialize your extention. This will be called before any other methods.
     */
    virtual Initialization initialize() = 0;

    /**
     * Return attribute store for this extention.
     */
    virtual std::shared_ptr<attributes::IAttributeStore> getAttributeStore() = 0;
};

} // namespace

#endif