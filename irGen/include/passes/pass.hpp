#ifndef __PASS_HPP__
#define __PASS_HPP__

#include "function.hpp"

namespace IRGen {
class Pass {
public:
    Pass(const std::string &name) : name_{name} {}
    virtual void Run(Function *func) const = 0;
    inline const std::string &GetName() const {
        return name_;
    }

protected:
    std::string name_{};
};

} // namespace IRGen

#endif