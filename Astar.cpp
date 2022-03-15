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
	double h_value;
	Node* parent_node;
	vector<Node*> neighbours;
	Node(){}
	Node(int row, int col, int max_dist) {
		this->row = row;
		this->col = col;
		this->local_dist = max_dist + 1;
	}
	static bool DijkstraComparator(Node* node1, Node* node2) {
		return node1->local_dist < node2->local_dist;
	}
	static bool AStarComparator(Node* node1, Node* node2) {
		return (node1->local_dist + node1->h_value) < (node2->local_dist + node2->h_value);
	}
};
class Graph {
public:
	vector<Node*>nodes;
	vector<string>map;
	int start_index;
	int end_index;
	Graph(string map_file) {
		// get map
		ifstream file(map_file.c_str());
		while (!file.eof()) {
			string str;
			getline(file, str);
			this->map.push_back(str);
		}
		// determine indexes of start point and end point
		for (int i = 0; i < this->map.size(); i++)
		{
			for (int j = 0; j < this->map[i].length(); j++)
			{
				if (this->map[i][j] == '1') {
					this->start_index = i * this->map[0].size() + j;
				}
				if (this->map[i][j] == '2') {
					this->end_index = i * this->map[0].size() + j;
				}
			}
		}
		// initialize nodes
		for (int i = 0; i < this->map.size(); i++)
		{
			for (int j = 0; j < this->map[i].length(); j++)
			{
				if (this->map[i][j] != '#') {
					Node* node = new Node(i, j,
						this->map.size() * this->map[0].length());
					int end_row = end_index / this->map[0].size();
					int end_col = end_index % this->map[0].size();
					node->h_value = sqrt((end_row - node->row) ^ 2 + (end_col - node->col) ^ 2);
					this->nodes.push_back(node);
				}
			}
		}
		// initialize neighbours
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
	void dijkstra_path() {
		list<Node*> path = this->dijkstra_algorithm();
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
							cout << '*';
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
	void a_star_path() {
		list<Node*> path = this->a_star_algorithm();
		for (int i = 0; i < this->map.size(); i++)
		{
			for (int j = 0; j < this->map[i].length(); j++)
			{
				if (this->map[i][j] == '#') {
					cout << '#';
				}
				else {
					bool is_index = false;
					for (auto k = path.cbegin(); k != path.cend() && !is_index; k++)
					{
						if ((*k)->row == i && (*k)->col == j) {
							is_index = true;
							cout << '*';
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
	list<Node*> dijkstra_algorithm() {
		Node *start_node = this->find_node(this->start_index);
		Node *end_node = this->find_node(this->end_index);
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
				marked_nodes.cend(), Node::DijkstraComparator);
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
	list<Node*> a_star_algorithm() {
		Node* start_node = this->find_node(this->start_index);
		Node* end_node = this->find_node(this->end_index);
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
				marked_nodes.cend(), Node::AStarComparator);
			for (int i = 0; i < (*min_node_iter)->neighbours.size(); i++)
			{
				/*if node in neighbour nodes but not already in short nodes*/
				Node* neighbour_node = (*min_node_iter)->neighbours[i];
				if (!count(shortest_nodes.cbegin(),
					shortest_nodes.cend(), neighbour_node)) {

					int new_local_dist = (*min_node_iter)->local_dist + 1;
					// update distance
					if (new_local_dist <= neighbour_node->h_value) {
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
	graph.dijkstra_path();
	cout << endl;
	graph.a_star_path();
	system("pause");
}


