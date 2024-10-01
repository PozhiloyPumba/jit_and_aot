#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

#include <vector>
#include <list>
#include "helpers/defines.hpp"
#include "BB.hpp"

namespace IRGen {

// For func
class Graph {
private:
    BB *firstBB_;
    BB *lastBB_;
    std::list<BB *> BBs_;
	std::string name_;
	std::vector<std::pair<InstrType, int>> params_;
	InstrType retType_;

public:
    Graph() : firstBB_(nullptr), lastBB_(nullptr) {}

	NO_COPY_NO_MOVE(Graph);
    ~Graph() = default;

    inline BB *GetFirstBB() { return firstBB_; }
    inline BB *GetLastBB() { return lastBB_; }
    inline void SetFirstBB(BB *bb) { firstBB_ = bb; }
    inline void SetLastBB(BB *bb) { lastBB_ = bb; }
	inline void SetName(const std::string &name) {name_ = name;}
	inline void SetRetType(InstrType retType) {retType_ = retType;}
	inline void SetParams(const std::vector<std::pair<InstrType, int>> &params) {params_ = params;}

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