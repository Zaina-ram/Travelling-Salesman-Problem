#ifndef GNAKDLATHJSTHAJSRNAKSJDA
#define GNAKDLATHJSTHAJSRNAKSJDA

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
class LCATree
{
public:
	typedef void* NodeId; 
	typedef int PreorderId;

	LCATree(int node_num_max);
	~LCATree();

	PreorderId Add(NodeId i, NodeId i_parent);
	PreorderId AddRoot(NodeId i);

	PreorderId GetLCA(PreorderId i, PreorderId j);
	void GetPenultimateNodes(PreorderId& i, PreorderId& j);

private:
	int n, n_max, K, k_max;
	int** array;

	NodeId* buf0;
	int* buf1;
	NodeId* parent_current; 
	int* child_current;

	int* parents;

	int _GetLCA(int i, int j);
	int GetLCADirect(int i, int j);
};

inline LCATree::LCATree(int _n_max) : n_max(_n_max), array(NULL)
{
#ifdef LCA_BLOCKS
#else
	K = 1;
	n = 0;
#endif
	parents = new int[n_max];
	buf0 = new NodeId[n_max];
	buf1 = new int[n_max];
	parent_current = buf0;
	child_current = buf1;
}

inline LCATree::~LCATree()
{
	int k;
	delete [] parents;
	if (buf0) delete [] buf0;
	if (buf1) delete [] buf1;
	if (array)
	{
		for (k=1; k<=k_max; k++) delete [] array[k];
		delete [] array;
	}
}

inline LCATree::PreorderId LCATree::Add(NodeId i, NodeId i_parent)
{
	assert(n < n_max);

	if (n == 0)
	{
		*parent_current = i;
		*(++ parent_current) = i_parent;
		parents[0] = -1;
	}
	else
	{
		if (i == *parent_current)
		{
			int c = *child_current --;
			while ( 1 )
			{
				int c_next = parents[c];
				parents[c] = n;
				if (c_next < 0) break;
				c = c_next;
			}
			parent_current --;
		}
		if (i_parent == *parent_current) parents[n] = *child_current;
		else
		{
			*(++ parent_current) = i_parent;
			parents[n] = -1;
			child_current ++;
		}
	}
	*child_current = n;
	return n ++;
}


inline LCATree::PreorderId LCATree::AddRoot(NodeId i)
{
	assert(n < n_max);

	if (n > 0)
	{
		if (i != *parent_current || parent_current != buf0+1)
		{
			printf("Error in LCATree construction: wrong sequence of calls!\n");
			exit(1);
		}
		int c = *child_current --;
		while ( 1 )
		{
			int c_next = parents[c];
			parents[c] = n;
			if (c_next < 0) break;
			c = c_next;
		}
		child_current ++;
	}
	parents[n++] = -1;

	delete [] buf0;
	buf0 = NULL;
	delete [] buf1;
	buf1 = NULL;

	// initialize array
	int b, k = 1, block_num = (n-1)/K+1;
	if (block_num < 3) return n-1;
	int d = (block_num-1)/4;
	while (d) { k ++; d >>= 1; }
	k_max = k;

	array = new int*[k_max+1];
	array[0] = parents;
	for (k=1, d=2; k<=k_max; k++, d*=2)
	{
		array[k] = new int[block_num-d];
		if (k == 1)
		{
			for (b=0; b<block_num-d; b++) array[1][b] = GetLCADirect((b+1)*K-1, (b+d)*K);
		}
		else
		{
			for (b=0; b<block_num-d; b++)
			{
				int i = array[k-1][b];
				int j = array[k-1][b+d/2];
				if (i < j) i = j;
				j = array[1][b+d/2-1];
				array[k][b] = (i > j) ? i : j;
			}
		}
	}

	return n-1;
}

inline int LCATree::GetLCADirect(int i, int j)
{
	while (i < j) i = parents[i];
	return i;
}


inline int LCATree::_GetLCA(int i, int j)
{
#ifdef LCA_BLOCKS
#else

	if (j == i) return i;

	int k = 0, d = (j-i)/2;
	while (d) { k ++; d >>= 1; }
	int diff = 1<<k;
	return (array[k][i] > array[k][j-diff]) ? array[k][i] : array[k][j-diff];

#endif
}

inline LCATree::PreorderId LCATree::GetLCA(PreorderId i, PreorderId j)
{
	if (i > j) { PreorderId k = i; i = j; j = k; }
	return _GetLCA(i, j);
}

inline void LCATree::GetPenultimateNodes(PreorderId& _i, PreorderId& _j)
{
	int i, j, d, swap;
	if (_i < _j) { i = _i; j = _j; swap = 0; }
	else         { i = _j; j = _i; swap = 1; }
	int r = _GetLCA(i, j);
	assert(i!=r && j!=r);
	while (parents[i] != r)
	{
		int i0 = parents[i];
		d = (j - i0)/2;
		while ( (i=_GetLCA(i0, i0+d)) == r ) d /= 2;
	}
	while (parents[j] != r)
	{
		int j0 = parents[j];
		d = (r - j0)/2;
		while ( (j=_GetLCA(j0, j0+d)) == r ) d /= 2;
	}
	if (swap == 0) { _i = i; _j = j; }
	else           { _j = i; _i = j; }
}

#endif
