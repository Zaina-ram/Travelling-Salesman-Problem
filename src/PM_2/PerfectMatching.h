#ifndef HALSKDJDFHALSJASFDFASJGLA
#define HALSKDJDFHALSJASFDFASJGLA

#include <assert.h>
#include <limits.h>
#include "block.h"
class PerfectMatching
{
public:

#ifdef PERFECT_MATCHING_DOUBLE
#else
	typedef int REAL;
	#define PM_INFTY (INT_MAX/2)
#endif

	typedef int NodeId;
	typedef int EdgeId;

	PerfectMatching(int nodeNum, int edgeNumMax);
	~PerfectMatching();

	EdgeId AddEdge(NodeId i, NodeId j, REAL cost);

	void Solve(bool finish=true);

	int GetSolution(EdgeId e);
	NodeId GetMatch(NodeId i);

	void GetDualSolution(int* blossom_parents, REAL* twice_y);
	int GetBlossomNum();

	void StartUpdate();
	void FinishUpdate();

	REAL GetTwiceSum(NodeId i);
	EdgeId AddNewEdge(NodeId i, NodeId j, REAL cost, bool do_not_add_if_positive_slack=true);
	void UpdateCost(EdgeId e, REAL delta_cost);

	struct Options
	{
		Options() : fractional_jumpstart(true),
		            dual_greedy_update_option(0),
		            dual_LP_threshold(0.00),
		            update_duals_before(false),
		            update_duals_after(false),
		            single_tree_threshold(1.00),
		            verbose(true)
		{}

		bool	fractional_jumpstart;

		int 	dual_greedy_update_option;

		double	dual_LP_threshold;

		bool	update_duals_before;
		bool	update_duals_after;

		double	single_tree_threshold;

		bool	verbose;
	} options;

	void Save(char* filename, int format=0); 

private:
	struct Node;
	struct Arc;
	struct Edge;
	struct Tree;
	struct TreeEdge;
	struct PQPointers;
	struct EdgeIterator;
	struct TreeEdgeIterator;
	struct LCATreeX;

	Node*	nodes;
	Edge*	edges;
	char*	edges_orig;
	DBlock<Node>* blossoms;
	Tree*	trees;
	DBlock<TreeEdge>* tree_edges;
	struct ExpandTmpItem
	{
		Node*	i;
		Node*	blossom_parent;
		Node*	blossom_grandparent;
	};
	Block<ExpandTmpItem>* expand_tmp_list;

	int		node_num;
	int		edge_num, edge_num_max;
	int		tree_num, tree_num_max;

	Node*	removed_first;
	int		blossom_num;
	int		removed_num;

	void*	pq_buf;

	bool	first_solve;

	struct Stat
	{
		int		shrink_count;
		int		expand_count;
		int		grow_count;
		double	shrink_time;
		double	expand_time;
		double	dual_time;
	} stat;

	void InitGreedy(bool allocate_trees=true);

	void InitGlobal();
	Node* FindBlossomRootInit(Edge* a0);
	void ShrinkInit(Edge* a0, Node* tree_root);
	void ExpandInit(Node* b);
	void AugmentBranchInit(Node* i0, Node* tree_root);

	void Finish();

	void ProcessNegativeEdge(Edge* a);

	void GetRealEndpoints(Edge* a, Node*& tail, Node*& head);
	Node* FindBlossomRoot(Edge* a0);
	void Shrink(Edge* a0);
	void Expand(Node* b);
	void Augment(Edge* a0);
	void AugmentBranch(Node* i0);
	void GrowNode(Node* i);
	void GrowTree(Node* root, bool new_subtree);
	bool ProcessEdge00(Edge* a, bool update_boundary_edge=true);
	void ProcessSelfloop(Node* b, Edge* a);

	void AddTreeEdge(Tree* t0, Tree* t1);

	void ComputeEpsSingle();
	void ComputeEpsCC();
	void ComputeEpsSCC();
	void ComputeEpsGlobal();
	bool UpdateDuals();

	void FreeRemoved();
	void CommitEps();

	void ReallocateEdges();

	void PrintAll();
};

int CheckPerfectMatchingOptimality(int node_num, int edge_num, int* edges, int* weights, PerfectMatching* pm, PerfectMatching::REAL threshold=(PerfectMatching::REAL)(1e-10));

double ComputePerfectMatchingCost(int node_num, int edge_num, int* edges, int* weights, PerfectMatching* pm);

#endif
