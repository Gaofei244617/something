#include <iostream>
#include <vector>

using Graph = std::vector<std::vector<bool>>; // 二维数组

void dfs(int u, const Graph& graph, std::vector<bool>& vis, std::vector<int>& ret)
{
	vis[u] = true;
	ret.push_back(u);

	for (int v = 0; v < graph.size(); v++)
	{
		if (u != v && !vis[v] && graph[u][v])
		{
			dfs(v, graph, vis, ret);
		}
	}
}

std::vector<int> DFS(const Graph& graph, int start)
{
	std::vector<int> ret;
	std::vector<bool> vis(graph.size(), false);

	dfs(start, graph, vis, ret);

	return ret;
}

int main(void)
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

	auto ret = DFS(graph, 1);
	for (const auto& it : ret)
	{
		std::cout << it << " ";
	}
	std::cout << std::endl;

	return 0;
}