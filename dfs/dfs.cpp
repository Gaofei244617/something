#include <iostream>
#include <stack>
#include <vector>

using Graph = std::vector<std::vector<bool>>; // 二维数组

// 深度优先搜索(非递归方式)
std::vector<int> dfs(const Graph& graph, int start)
{
	std::vector<int> ret;

	size_t node_num = graph.size();
	std::vector<bool> visited(node_num, false);
	std::stack<int> _stack;

	visited[start] = true; // 搜索到的节点
	_stack.push(start);

	int s_top;
	while (!_stack.empty())
	{
		s_top = _stack.top();
		_stack.pop();

		ret.push_back(s_top);

		// 向前搜索
		for (int i = 0; i < node_num; i++)
		{
			if (s_top != i && graph[i][s_top] && !visited[i])
			{
				visited[i] = true;
				_stack.push(i);
			}
		}
	}

	return ret;
}

int main(int argc, const char* argv[])
{
	int node_num = 10; // 结点数

	// 使用一个临接矩阵表示图的连接情况
	std::vector<std::vector<bool>> graph(node_num, std::vector<bool>(node_num, false));

	graph[0][1] = graph[1][0] = true;
	graph[0][5] = graph[5][0] = true;
	graph[0][7] = graph[7][0] = true;
	graph[1][2] = graph[2][1] = true;
	graph[1][3] = graph[3][1] = true;
	graph[1][5] = graph[5][1] = true;
	graph[2][3] = graph[3][2] = true;
	graph[2][7] = graph[7][2] = true;
	graph[2][4] = graph[4][2] = true;
	graph[0][3] = graph[3][0] = true;
	graph[3][4] = graph[4][3] = true;
	graph[3][5] = graph[5][3] = true;
	graph[3][7] = graph[7][3] = true;
	graph[4][5] = graph[5][4] = true;

	graph[8][9] = graph[9][8] = true;
	graph[6][8] = graph[8][6] = true;
	graph[6][9] = graph[9][6] = true;

	for (int i = 0; i < graph.size(); i++)
	{
		for (int j = 0; j < graph.size(); j++)
		{
			std::cout << graph[i][j] << " ";
		}
		std::cout << std::endl;
	}

	//
	std::cout << "-----------------" << std::endl;
	auto ret = dfs(graph, 1);
	for (const int id : ret)
	{
		std::cout << id << " ";
	}
	std::cout << std::endl;

	return 0;
}