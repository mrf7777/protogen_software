#ifndef PROTOGEN_IINISIALIZABLE_HPP
#define PROTOGEN_IINISIALIZABLE_HPP

namespace protogen {

class IInitializable {
public:
    enum class Initialization {
        Success,
        Failure,
    };

    virtual ~IInitializable() = default;

    /**
     * Initialize your object. This is intended called before any other methods.
     */
    virtual Initialization initialize() = 0;
};
}

#endif