#ifndef __FUNCTION_HPP__
#define __FUNCTION_HPP__

#include <vector>
#include <list>
#include <string>
#include "helpers/defines.hpp"
#include "BB.hpp"

namespace IRGen {

// For func
class ParameterInstr;

class Function {
private:
    BB *firstBB_;
    BB *lastBB_;
    std::list<BB *> BBs_;
	std::string name_;
	std::vector<ParameterInstr *> params_;
	InstrType retType_;

public:
    Function() : firstBB_(nullptr), lastBB_(nullptr) {}

	NO_COPY_NO_MOVE(Function);
    ~Function() = default;

    inline BB *GetFirstBB() { return firstBB_; }
    inline BB *GetLastBB() { return lastBB_; }
    inline void SetFirstBB(BB *bb) { firstBB_ = bb; }
    inline void SetLastBB(BB *bb) { lastBB_ = bb; }
	inline void SetName(const std::string &name) {name_ = name;}
	inline void SetRetType(InstrType retType) {retType_ = retType;}
	inline void SetParams(const std::vector<ParameterInstr *> &params) { params_ = params;}
	inline ParameterInstr *GetParam(size_t i) { return params_[i];}

    void AddBB(BB *bb);
    void AssasinateBB(BB *bb);
    void AddBBAsPred(BB *newBB, BB *anchor);
    void AddBBAsSucc(BB *newBB, BB *anchor);
    void DeletePreds(BB *bb);
    void DeleteSuccs(BB *bb);
	void dump();
};

}

#endif