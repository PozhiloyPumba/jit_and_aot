#ifndef __ELIMINATE_CHECKS_PASS_HPP__
#define __ELIMINATE_CHECKS_PASS_HPP__

#include "instrDef.hpp"
#include "pass.hpp"

namespace IRGen {

class EliminateChecksPass : public Pass {
    constexpr static inline const char *EliminateChecksPassName = "EliminateChecksPass";

public:
	EliminateChecksPass() : Pass(EliminateChecksPassName) {}
    void Run(Function *func) const override;
};

} // namespace IRGen

#endif