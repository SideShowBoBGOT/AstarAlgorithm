#include <iostream>
#include <string>
#include<list>
#include<vector>
#include<algorithm>
#include<fstream>
using namespace std;
class Node {
public:
	int row;
	int col;
	int local_dist = -1;
	Node* parent_node;
	vector<Node*> neighbours;
	Node(){}
	Node(int row, int col, int max_dist) {
		this->row = row;
		this->col = col;
		this->local_dist = max_dist + 1;
	}
	static bool MinMaxComparator(Node* node1, Node* node2) {
		return node1->local_dist < node2->local_dist;
	}
};
class Graph {
public:
	vector<Node*>nodes;
	vector<string>map;
	Graph(string map_file) {
		ifstream file(map_file.c_str());
		while (!file.eof()) {
			string str;
			getline(file, str);
			this->map.push_back(str);
		}
		for (int i = 0; i < this->map.size(); i++)
		{
			for (int j = 0; j < this->map[i].length(); j++)
			{
				if (this->map[i][j] == ' ') {
					Node* node = new Node(i, j,
						this->map.size()*this->map[0].length());
					this->nodes.push_back(node);
				}
			}
		}
		for (auto i = this->nodes.cbegin(); i != this->nodes.cend(); i++)
		{
			for (auto j = this->nodes.cbegin(); j != this->nodes.cend(); j++)
			{
				int node_index = (*i)->row * this->map[0].size() + (*i)->col;
				int neighbour_index = (*j)->row * this->map[0].size() + (*j)->col;
				int diff = abs(neighbour_index - node_index);
				if (diff == this->map[0].size() || diff == 1) {
					(*i)->neighbours.push_back(*j);
				}
			}
		}
		file.close();
	}
	void draw() {
		for (int i = 0; i < this->map.size(); i++)
		{
			for (int j = 0; j < this->map[i].length(); j++)
			{
				if (this->map[i][j] == '#') {
					cout << '#';
				}
				else {
					cout << ' ';
				}
			}
			cout << endl;
		}
	}
	void draw(int start_index, int end_index) {
		list<Node*> path = this->find_path(start_index, end_index);
		for (int i = 0; i < this->map.size(); i++)
		{
			for (int j = 0; j < this->map[i].length(); j++)
			{
				if (this->map[i][j] == '#') {
					cout << '#';
				}
				else {
					bool is_index = false;
					for (auto k = path.cbegin(); k != path.cend()&&!is_index; k++)
					{
						if ((*k)->row == i && (*k)->col == j) {
							is_index = true;
							cout << '$';
						}
					}
					if (!is_index) {
						cout << ' ';
					}
				}
			}
			cout << endl;
		}

	}
	Node* find_node(int index) {
		int row = index / this->map[0].size();
		int col = index % this->map[0].size();
		Node *node = nullptr;
		for (auto i = 0; i < this->nodes.size() ; i++)
		{
			if (this->nodes[i]->col == col && this->nodes[i]->row == row) {
				node = this->nodes[i];
				break;
			}
		}
		return node;
	}
	list<Node*> find_path(int start_index, int end_index) {
		Node *start_node = this->find_node(start_index);
		Node *end_node = this->find_node(end_index);
		list<list<int>> tree_of_paths;
		vector<Node*> marked_nodes;
		marked_nodes.reserve(this->map.size() * this->map[0].length());
		vector<Node*> shortest_nodes;
		shortest_nodes.reserve(this->map.size() * this->map[0].length());
		marked_nodes.push_back(start_node);
		// distance of start node is obviously zero
		start_node->local_dist = 0;
		// parent node does not exist for start_node
		start_node->parent_node = nullptr;
		//algorithm itself
		// while end node not in shortest nodes 
		while (!count(shortest_nodes.cbegin(),
			shortest_nodes.cend(), end_node)) {
			auto min_node_iter = min_element(marked_nodes.cbegin(),
				marked_nodes.cend(), Node::MinMaxComparator);
			for (int i = 0; i < (*min_node_iter)->neighbours.size(); i++)
			{
				/*if node in neighbour nodes but not already in short nodes*/
				Node* neighbour_node = (*min_node_iter)->neighbours[i];
				if (!count(shortest_nodes.cbegin(),
					shortest_nodes.cend(), neighbour_node)) {
					
					int new_local_dist = (*min_node_iter)->local_dist + 1;
					// update distance
					if ( new_local_dist < neighbour_node->local_dist) {
						neighbour_node->local_dist = (*min_node_iter)->local_dist + 1;
						neighbour_node->parent_node = *min_node_iter;
					}
					// push neighbour node to mark nodes
					if (!count(marked_nodes.cbegin(),
						marked_nodes.cend(), neighbour_node)) {
						marked_nodes.push_back(neighbour_node);
					}
				}
				
			}
			// erase min_node from marked_nodes and push it to shortest nodes
			shortest_nodes.push_back(*min_node_iter);
			marked_nodes.erase(min_node_iter);
		}
		// get list of path via nodes
		list<Node*>path;
		path.push_back(end_node);
		Node* parent_node = end_node->parent_node;
		while (parent_node != nullptr) {
			path.push_back(parent_node);
			parent_node = parent_node->parent_node;
		}
		path.reverse();
		return path;
	}
};
int main()
{
	string file_name = "map.txt";
	Graph graph(file_name);
	graph.draw(100, 12);
	system("pause");
}


