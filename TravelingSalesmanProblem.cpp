//编译环境：Visual Studio 2019
//作    者：张翊<2018304010314><2018304010314@cau.edu.cn>
//最后修改：2020-6-19
#include <iostream>
#include <algorithm>
#include <queue>
#include <stack>
using namespace std;

#define SIZE 100    //节点个数上限
#define inf 0x7fffffff   //无穷大

int ub, lb;    //上界和下界
int n;//节点个数
int c[SIZE][SIZE];    //邻接矩阵
int res = inf;        //res为最终的路径长度

class Node {    //节点的数据结构，用于保存遍历到当前节点时的状态
public:
	int cur;    //当前节点的下标
	int cur_l;    //已走路径长度
	int lb;     //当前节点的估计下界
	int k;    //走过的节点个数
	Node* pre;    //存储前一个节点的位置,用于回溯路径
	bool visited[SIZE];     //用来保存哪些节点已经被访问过

	Node() {      //一个默认构造函数，主要用于初始化bool数组
		for (int i = 1; i <= n; i++) {
			visited[i] = false;
		}
	}
};

class Compare_Node_Pointer     //为Node*的优先队列重载的比较类
{
public:
	/* Node::priority 小的优先 */
	bool operator () (Node*& a, Node*& b) const
	{
		return a->lb > b->lb;    //priority_queue中接收的第三个参数为!cmp,因此此处为>号
	}
};

priority_queue<Node*, std::vector<Node*>, Compare_Node_Pointer> pt;  //pt表，第三个参数表示调用Compare_Node_Pointer比较类

int init_lb() {    //采用选取每行最小两个元素之和的办法取得一个初始的下界
	int lb = 0;
	for (int i = 1; i <= n; i++) {
		int temp[SIZE];
		for (int j = 1; j <= n; j++) {
			temp[j] = c[i][j];       //temp为与第i行相同的一个临时数组
		}
		sort(temp + 1, temp + n + 1);
		lb = lb + temp[1] + temp[2];    //取每行最小的两个元素
	}
	lb = (lb + 1) / 2;
	return lb;
}

bool dfs_visited[SIZE] = { 0 };     //用于保存贪心法中哪些节点已经遍历过

int dfs(int i, int j, int len) {    //i为当前节点，j为已走节点个数，len 为已走路径长度
	if (j == n) {
		return len + c[i][1];       //如果已经访问了n个节点，说明i为最后一个节点，直接用其连接第一个节点即可
	}
	int minlen = inf;               //采用贪心法，每一步都选取可走边中最短的那条
	int p;                          //每一步选取得的节点
	for (int k = 1; k <= n; k++) {
		if (dfs_visited[k] == false && minlen > c[i][k]) {
			minlen = c[i][k];
			p = k;
		}
	}
	dfs_visited[p] = true;         //修改下一个节点的访问状态为已访问
	return dfs(p, j + 1, len + minlen);
}

int init_ub() {                //采用贪心法求得一个初始的上界
	dfs_visited[1] = true;     //选取第1个点为起点
	ub = dfs(1, 1, 0);
	return ub;
}

int get_lb(Node p) {    //估计当前节点的下界
	int res = 0;
	res = p.cur_l * 2;
	int min1 = inf, min2 = inf;    //min1和min2代表了第1行和第k行中不在路径中的最小值
	for (int i = 1; i <= n; i++) {
		if (p.visited[i] == false && min1 > c[i][1]) {
			min1 = c[i][1];
		}
	}
	for (int i = 1; i <= n; i++) {
		if (p.visited[i] == false && min2 > c[p.cur][i]) {
			min2 = c[p.cur][i];
		}
	}
	res = res + min1 + min2;

	for (int i = 1; i <= n; i++) {      //第二个循环用于计算不在路径中的节点所对应行的最小的两个元素
		if (p.visited[i] == false) {    //该节点没有被访问过，就不在路径中
			min1 = min2 = inf;
			for (int j = 1; j <= n; j++)
			{
				if (min1 > c[i][j])
					min1 = c[i][j];
			}
			for (int j = 1; j <= n; j++)
			{
				if (min2 > c[j][i])
					min2 = c[j][i];
			}
			res += min1 + min2;
		}
	}
	return (res + 1) / 2;
}

void solution() {
	ub = init_ub();   //对上界进行初始化
	lb = init_lb();	  //对下界进行初始化 
	Node* min_node = NULL;   //用于保存当前找到的最小的可行解
	int last = 0;            //用于保存当前找到的最小的可行解的最后一个节点下标
	Node* start = new Node();
	start->k = 1;      //已经走了1个节点
	start->cur = 1;    //start节点对应的下标为1
	start->visited[1] = true;    //第一个节点已经访问过
	start->cur_l = 0;  //将起点的已经过路径置0
	start->lb = lb;    //将起点的下界置为初值
	start->pre = NULL;
	pt.push(start);  
	while (pt.size()) {
		Node* now = (pt.top());   //优先队列总是弹出lb最小的节点，贪心法的体现
		pt.pop();
		if (now->k == n - 1) {    //若到cur时已经遍历了n-1个节点，说明只剩下最后一个节点未被访问
			int next;             //next用来存放最后一个节点的下标
			for (int i = 1; i <= n; i++) {
				if (now->visited[i] == false) {
					next = i;
					break;
				}
			}
			int total = now->cur_l + c[now->cur][next] + c[next][1];    //计算当前可行解的总花费
			if (total <= now->lb) {    //优先队列总是返回lb最小的节点，若计算出的total比当前的最小值还小，
				res = total;           //则说明已经找到极值,通过指针逆序回溯输出路径
				cout << 0 << " " << next-1 << " ";    //由上面可以知道，最后一个节点是起点1，倒数第二个节点是next
				Node* p = now;
				while (p != NULL) {
					cout << p->cur-1 << " ";         //回溯输出路径，发现题目是0到n-1...
					p = p->pre;                      //所以所有下标-1
				}
				return;
			}
			else {
				ub = min(total, ub);   //如果当前的可行解非极值，那么我们利用这个可行解来缩小上界
				if (total < res) {
					min_node = now;    //保存比当前解要小的节点以及此时的最后一个节点
					last = next;
				}
				res = min(total, res);
				continue;
			}

		}
	//若当前节点不是倒数第二个节点，则对其进行BFS遍历，遍历其所有子节点
		for (int i = 1; i <= n; i++) {
			if (now->visited[i] == false) {              //如果该节点没有被访问过，则可作为下一步的候选点
				Node* next = new Node();   
				next->pre = now;
				next->cur_l = now->cur_l + c[now->cur][i];
				next->cur = i;
				next->k = now->k + 1;
				for (int j = 1; j <= n; j++) {
					next->visited[j] = now->visited[j];    //下一个节点已访问的节点与当前节点一致
				}
				next->visited[i] = true;    //更新访问节点
				next->lb = get_lb(*next);
				if (next->lb > ub) {
					continue;    //如果计算得出下界大于当前的上界，则舍去该节点
				}
				else {
					pt.push(next);//否则就将它加入到pt表中
				}
			}
		}
	}
	cout << 0 << " " << last-1 << " ";      //若程序遍历完整个空间树均未发现符合要求的解
	while (min_node != NULL) {			    //则会在所有可行解中输出代价最小的一条路径
		cout << min_node->cur-1 << " ";
		min_node = min_node->pre;
	}
}

int main() {
	cin >> n;
	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= n; j++) {
			cin >> c[i][j];
			if (i == j) {
				c[i][j] = inf;
			}
		}
	}

	solution();

	return 0;
}

/*
5
1 3 1 5 8
3 1 6 7 9
1 6 1 4 2
5 7 4 1 3
8 9 2 3 1

4
4 3  2  1
3 5  3  10
2 3  7  12
1 10 12 9

4
3 5 7 8
5 6 2 1
7 2 8 4
8 1 4 9
*/
