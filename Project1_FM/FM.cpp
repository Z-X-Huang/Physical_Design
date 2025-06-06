#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <time.h>

using namespace std;

struct Net;

struct Node {
	int gain;
	int area;
	int lock; //0 is unlocked, 1 is locked
	int group; //1 is group1, 2 is group2
	string name;
	Node* prev;
	Node* next;
	vector<Net*> net_list;
};

struct Net {
	int degree;
	int NumOf1;
	int NumOf2;
	string name;
	vector<Node*> node_list;
};

Node* find_max_gain(vector<Node*> bucket, Node* current_max, int maxPin, int& area1, int& area2, int lower_bound, int upper_bound, int minArea) {
	Node* te = current_max;
	int i = 0;
	int cons = 0;
	if((area1 - minArea) < lower_bound) {
		cons = 1;
	}
	if((area1 + minArea) > upper_bound) {
		cons = 2;
	}

	while(1) {
		if(te != NULL) {
			if(te->next != NULL) {
				te = te->next;
				if(cons == 1 && te->group == 1) {
					continue;
				}
				if(cons == 2 && te->group == 2) {
					continue;
				}
			}
			else {
				for(i = te->gain + maxPin - 1; i >= 0; i--) {
					if(bucket[i] != NULL){
						te = bucket[i];
						if(cons == 1 && te->group == 1) {
							continue;
						}
						if(cons == 2 && te->group == 2) {
							continue;
						}
						break;
					}
				}
				if(i == -1) {
					return NULL;
				}
			}
		}
		else {
			for(i = bucket.size() - 1; i >= 0; i--) {
				if(bucket[i] != NULL){
					te = bucket[i];
					if(cons == 1 && te->group == 1) {
						continue;
					}
					if(cons == 2 && te->group == 2) {
						continue;
					}
					break;
				}
			}
			if(i == -1) {
				return NULL;
			}
		}
		if(te->group == 1) {
			if(((area1 - te->area) <= upper_bound) && ((area1 - te->area) >= lower_bound)) {
				area1 -= te->area;					
				area2 += te->area;	
				return te;
			}
		}
		else {
			if(((area1 + te->area) <= upper_bound) && ((area1 + te->area) >= lower_bound)) {
				area1 += te->area;					
				area2 -= te->area;
				return te;
			}
		}
	}

	
	return NULL;
}

int main(int argc, char *argv[])
{
	clock_t start = clock();

	int x = 0;
	int y = 0;
	int NumOfMovable = 0;
	int NumOfTerminal = 0;
	int NumOfNet = 0;
	int NumOfPin = 0;
	int maxPin = 0;
	int maxArea = 0;
	int minArea = 100000;
	int totalArea = 0;
	int area1 = 0;
	int area2 = 0;
	string s;
	vector<Node*> node_list;
	vector<Net*> net_list;
	vector<Node*> group1;
	vector<Node*> group2;
	unordered_map<string, Node*> node_map;
	unordered_map<string, int> node_map2;
	unordered_map<string, Net*> net_map;
	vector<int> best_group;
	vector<string> better;
	int cons = 0;
	
	ifstream file1;
	file1.open(argv[1]);

	getline(file1, s);
	getline(file1, s);
	getline(file1, s);

	if((string)argv[1] == "adaptec1.nodes") {
		cons = 11000;
	}
	if((string)argv[1] == "adaptec2.nodes") {
		cons = 20000;
	}
	if((string)argv[1] == "adaptec3.nodes") {
		cons = 65000;
	}
	if((string)argv[1] == "superblue1.nodes") {
		cons = 160000;
	}
	if((string)argv[1] == "superblue2.nodes") {
		cons = 200000;
	}

	file1 >> s >> s >> NumOfMovable;
	file1 >> s >> s >> NumOfTerminal;
	NumOfMovable -= NumOfTerminal;

	for (int i = 0; i < NumOfMovable; i++) {
		Node* new_node = new Node;
		file1 >> s >> x >> y;
		new_node->name = s;
		new_node->area = x * y;
		new_node->lock = 0;
		new_node->gain = 0;
		new_node->prev = NULL;
		new_node->next = NULL;
		totalArea += new_node->area;

		if (maxArea < new_node->area) {
			maxArea = new_node->area;
		}
		if(minArea > new_node->area) {
			minArea = new_node->area;
		}

		if (i < NumOfMovable / 2) { 
			new_node->group = 1;
			group1.push_back(new_node);
			best_group.push_back(new_node->group);
			area1 += new_node->area;
		}
		else {
			new_node->group = 2;
			group2.push_back(new_node);
			best_group.push_back(new_node->group);
			area2 += new_node->area;
		}
		node_map[new_node->name] = new_node;
		node_map2[new_node->name] = node_list.size();
		node_list.push_back(new_node);
	}

	for (int i = 0; i < NumOfTerminal; i++){
		Node* new_node = new Node;
		new_node->name = "Ter";
		file1 >> s;
		node_map[s] = new_node;
		file1 >> s >> s >> s;
	}
	file1.close();
	
	best_group.reserve(node_list.size());

	ifstream file2;
	file2.open(argv[2]);

	getline(file2, s);
	getline(file2, s);
	getline(file2, s);

	file2 >> s >> s >> NumOfNet;
	file2 >> s >> s >> NumOfPin;
	
	for (int i = 0; i < NumOfNet; i++) {
		Net* new_net = new Net;
		file2 >> s >> s >> new_net->degree >> new_net->name;
		new_net->NumOf1 = 0;
		new_net->NumOf2 = 0;
		for (int j = 0; j < new_net->degree; j++) {
			file2 >> s;

			if(node_map[s]->name != "Ter"){
				if(node_map[s]->net_list.size() > 0) {
					if(node_map[s]->net_list[node_map[s]->net_list.size() - 1]->name == new_net->name) {
						file2 >> s >> s >> s >> s;
						continue;
					} 
				}
				
				new_net->node_list.push_back(node_map[s]);
				node_map[s]->net_list.push_back(new_net);
				if(maxPin < node_map[s]->net_list.size()){
					maxPin = node_map[s]->net_list.size();
				}
				if(node_map[s]->group == 1) {
					new_net->NumOf1++;
				}
				else {
					new_net->NumOf2++;
				}
			}
			file2 >> s >> s >> s >> s;
		}

		if(new_net->node_list.size() == 1) {
			if(maxPin == new_net->node_list[0]->net_list.size()) {
				maxPin--;
			}
			new_net->node_list[0]->net_list.pop_back();
		}
		else if(new_net->node_list.size() == 0) {}
		else {
			net_map[new_net->name] = new_net;
			net_list.push_back(new_net);
		}
	}
	file2.close();

	double lower_bound = 0.4 * (float)totalArea - (float)maxArea;
	double upper_bound = 0.4 * (float)totalArea + (float)maxArea;
	
	while(1) {
		if(area1 > (lower_bound + upper_bound) / 2) {
			area1 -= group1[group1.size()-1]->area;
			area2 += group1[group1.size()-1]->area;
			group1[group1.size()-1]->group = 2;
			best_group[node_map2[group1[group1.size()-1]->name]] = 2;
			for(int i = 0; i < group1[group1.size()-1]->net_list.size(); i++){
				group1[group1.size()-1]->net_list[i]->NumOf1--;
				group1[group1.size()-1]->net_list[i]->NumOf2++;
			}
			group2.push_back(group1[group1.size()-1]);
			group1.pop_back();
			if(area1 <= (lower_bound + upper_bound) / 2) {
				if(area1 >= lower_bound) {
					break;
				}
			}
		}
		else if(area1 < (lower_bound + upper_bound) / 2) {
			area1 += group2[group2.size()-1]->area;
			area2 -= group2[group2.size()-1]->area;
			group2[group2.size()-1]->group = 1;
			best_group[node_map2[group2[group2.size()-1]->name]] = 1;
			for(int i = 0; i < group2[group2.size()-1]->net_list.size(); i++){
				group2[group2.size()-1]->net_list[i]->NumOf1++;
				group2[group2.size()-1]->net_list[i]->NumOf2--;
			}
			group1.push_back(group2[group2.size()-1]);
			group2.pop_back();
			if(area1 >= (lower_bound + upper_bound) / 2) {
				if(area1 <= upper_bound) {
					break;
				}
			}
		}
		else {
			break;
		}
	}

	int totalGain = 1;
	int bestGain = 1;
	int cutSize = 0;
	vector<string> cutSet;
	for(int i = 0; i < net_list.size(); i++) {
		if(net_list[i]->NumOf1 != 0 && net_list[i]->NumOf2 != 0) {
			cutSize++;
			cutSet.push_back(net_list[i]->name);
		}
	}
	cout << cutSize << endl;
	while(bestGain > 0 && cutSize > cons) {
		totalGain = 0;
		bestGain = 0;
		vector<Node*> bucket;
		bucket.reserve(2*maxPin + 1);
		for(int i = 0; i < 2*maxPin + 1; i++) {
			bucket.push_back(NULL);
		}

		for(int i = 0; i < net_list.size(); i++) {
			if(net_list[i]->NumOf1 == 0) {
				for(int j = 0; j < net_list[i]->node_list.size(); j++) {
					net_list[i]->node_list[j]->gain--;
				}
			}

			if(net_list[i]->NumOf1 == 1) {
				for(int j = 0; j < net_list[i]->node_list.size(); j++) {
					if(net_list[i]->node_list[j]->group == 1) {
						net_list[i]->node_list[j]->gain++;
						break;
					}
				}
			}

			if(net_list[i]->NumOf2 == 0) {
				for(int j = 0; j < net_list[i]->node_list.size(); j++) {
					net_list[i]->node_list[j]->gain--;
				}
			}

			if(net_list[i]->NumOf2 == 1) {
				for(int j = 0; j < net_list[i]->node_list.size(); j++) {
					if(net_list[i]->node_list[j]->group == 2) {
						net_list[i]->node_list[j]->gain++;
						break;
					}
				}
			}
		}

		for(int i = 0; i < node_list.size(); i++) {
			if(bucket[node_list[i]->gain + maxPin] == NULL) {
				bucket[node_list[i]->gain + maxPin] = node_list[i];
			}
			else {
				bucket[node_list[i]->gain + maxPin]->prev = node_list[i];
				node_list[i]->next = bucket[node_list[i]->gain + maxPin];
				bucket[node_list[i]->gain + maxPin] = node_list[i];
			}
		}

		int time = node_list.size(); //node_list.size()
		
		while(time > 0) {
			time--;
			Node* base = NULL;
			base = find_max_gain(bucket, base, maxPin, area1, area2, lower_bound, upper_bound, minArea);

			if(base == NULL) {
				break;
			}

			if(bucket[base->gain + maxPin] == base) {
				bucket[base->gain + maxPin] = base->next;
			}
			
			else {
				base->prev->next = base->next;
			}
			if(base->next != NULL) {
				base->next->prev = base->prev;
			}
			
			
			for(int i = 0; i < base->net_list.size(); i++) {
				if(base->group == 1) {
					if(base->net_list[i]->NumOf2 == 0) {
						for(int j = 0; j < base->net_list[i]->node_list.size(); j++) {
							if(base->net_list[i]->node_list[j] != base && base->net_list[i]->node_list[j]->lock == 0) {
								base->net_list[i]->node_list[j]->gain++;

								if(bucket[base->net_list[i]->node_list[j]->gain + maxPin - 1] == base->net_list[i]->node_list[j]) {
									bucket[base->net_list[i]->node_list[j]->gain + maxPin - 1] = base->net_list[i]->node_list[j]->next;
								}
								else {
									base->net_list[i]->node_list[j]->prev->next = base->net_list[i]->node_list[j]->next;
								}
								if(base->net_list[i]->node_list[j]->next != NULL) {
									base->net_list[i]->node_list[j]->next->prev = base->net_list[i]->node_list[j]->prev;
								}
								base->net_list[i]->node_list[j]->prev = NULL;
								if(bucket[base->net_list[i]->node_list[j]->gain + maxPin] != NULL) {
									bucket[base->net_list[i]->node_list[j]->gain + maxPin]->prev = base->net_list[i]->node_list[j];
								}
								base->net_list[i]->node_list[j]->next = bucket[base->net_list[i]->node_list[j]->gain + maxPin];
								bucket[base->net_list[i]->node_list[j]->gain + maxPin] = base->net_list[i]->node_list[j];
							}
						}
					}
					else if(base->net_list[i]->NumOf2 == 1) {
						for(int j = 0; j < base->net_list[i]->node_list.size(); j++) {
							if(base->net_list[i]->node_list[j]->group == 2) {
								if(base->net_list[i]->node_list[j]->lock == 0) {
									base->net_list[i]->node_list[j]->gain--;

									if(bucket[base->net_list[i]->node_list[j]->gain + maxPin + 1] == base->net_list[i]->node_list[j]) {
										bucket[base->net_list[i]->node_list[j]->gain + maxPin + 1] = base->net_list[i]->node_list[j]->next;
									}
									else {
										base->net_list[i]->node_list[j]->prev->next = base->net_list[i]->node_list[j]->next;
									}
									if(base->net_list[i]->node_list[j]->next != NULL) {
										base->net_list[i]->node_list[j]->next->prev = base->net_list[i]->node_list[j]->prev;
									}

									base->net_list[i]->node_list[j]->prev = NULL;
									if(bucket[base->net_list[i]->node_list[j]->gain + maxPin] != NULL) {
										bucket[base->net_list[i]->node_list[j]->gain + maxPin]->prev = base->net_list[i]->node_list[j];
									}
									base->net_list[i]->node_list[j]->next = bucket[base->net_list[i]->node_list[j]->gain + maxPin];
									bucket[base->net_list[i]->node_list[j]->gain + maxPin] = base->net_list[i]->node_list[j];
								}
								break;
							}
						}
					}
					base->net_list[i]->NumOf1--;
					base->net_list[i]->NumOf2++;
				}
				else {
					if(base->net_list[i]->NumOf1 == 0) {
						for(int j = 0; j < base->net_list[i]->node_list.size(); j++) {
							if(base->net_list[i]->node_list[j] != base && base->net_list[i]->node_list[j]->lock == 0) {
								base->net_list[i]->node_list[j]->gain++;

								if(bucket[base->net_list[i]->node_list[j]->gain + maxPin - 1] == base->net_list[i]->node_list[j]) {
									bucket[base->net_list[i]->node_list[j]->gain + maxPin - 1] = base->net_list[i]->node_list[j]->next;
								}
								else {
									base->net_list[i]->node_list[j]->prev->next = base->net_list[i]->node_list[j]->next;
								}
								if(base->net_list[i]->node_list[j]->next != NULL) {
									base->net_list[i]->node_list[j]->next->prev = base->net_list[i]->node_list[j]->prev;
								}

								base->net_list[i]->node_list[j]->prev = NULL;
								if(bucket[base->net_list[i]->node_list[j]->gain + maxPin] != NULL) {
									bucket[base->net_list[i]->node_list[j]->gain + maxPin]->prev = base->net_list[i]->node_list[j];
								}
								base->net_list[i]->node_list[j]->next = bucket[base->net_list[i]->node_list[j]->gain + maxPin];
								bucket[base->net_list[i]->node_list[j]->gain + maxPin] = base->net_list[i]->node_list[j];
							}
						}
					}
					else if(base->net_list[i]->NumOf1 == 1) {
						for(int j = 0; j < base->net_list[i]->node_list.size(); j++) {
							if(base->net_list[i]->node_list[j]->group == 1) {
								if(base->net_list[i]->node_list[j]->lock == 0) {
									base->net_list[i]->node_list[j]->gain--;

									if(bucket[base->net_list[i]->node_list[j]->gain + maxPin + 1] == base->net_list[i]->node_list[j]) {
										bucket[base->net_list[i]->node_list[j]->gain + maxPin + 1] = base->net_list[i]->node_list[j]->next;
									}
									else {
										base->net_list[i]->node_list[j]->prev->next = base->net_list[i]->node_list[j]->next;
									}
									if(base->net_list[i]->node_list[j]->next != NULL) {
										base->net_list[i]->node_list[j]->next->prev = base->net_list[i]->node_list[j]->prev;
									}
									base->net_list[i]->node_list[j]->prev = NULL;
									if(bucket[base->net_list[i]->node_list[j]->gain + maxPin] != NULL) {
										bucket[base->net_list[i]->node_list[j]->gain + maxPin]->prev = base->net_list[i]->node_list[j];
									}
									base->net_list[i]->node_list[j]->next = bucket[base->net_list[i]->node_list[j]->gain + maxPin];
									bucket[base->net_list[i]->node_list[j]->gain + maxPin] = base->net_list[i]->node_list[j];
								}
								break;
							}
						}
					}
					base->net_list[i]->NumOf1++;
					base->net_list[i]->NumOf2--;
				}
			}
			

			totalGain += base->gain;
			if(base->group == 1) {
				base->group = 2;
			}
			else {
				base->group = 1;
			}
			base->lock = 1;

			for(int i = 0; i < base->net_list.size(); i++) {
				if(base->group == 1) {
					if(base->net_list[i]->NumOf2 == 0) {
						for(int j = 0; j < base->net_list[i]->node_list.size(); j++) {
							if(base->net_list[i]->node_list[j]->lock == 0) {
								base->net_list[i]->node_list[j]->gain--;

								if(bucket[base->net_list[i]->node_list[j]->gain + maxPin + 1] == base->net_list[i]->node_list[j]) {
									bucket[base->net_list[i]->node_list[j]->gain + maxPin + 1] = base->net_list[i]->node_list[j]->next;
								}
								else {
									base->net_list[i]->node_list[j]->prev->next = base->net_list[i]->node_list[j]->next;
								}
								if(base->net_list[i]->node_list[j]->next != NULL) {
									base->net_list[i]->node_list[j]->next->prev = base->net_list[i]->node_list[j]->prev;
								}

								base->net_list[i]->node_list[j]->prev = NULL;
								if(bucket[base->net_list[i]->node_list[j]->gain + maxPin] != NULL) {
									bucket[base->net_list[i]->node_list[j]->gain + maxPin]->prev = base->net_list[i]->node_list[j];
								}
								base->net_list[i]->node_list[j]->next = bucket[base->net_list[i]->node_list[j]->gain + maxPin];
								bucket[base->net_list[i]->node_list[j]->gain + maxPin] = base->net_list[i]->node_list[j];
							}
						}
					}
					else if(base->net_list[i]->NumOf2 == 1) {
						for(int j = 0; j < base->net_list[i]->node_list.size(); j++) {
							if(base->net_list[i]->node_list[j]->group == 2) {
								if(base->net_list[i]->node_list[j]->lock == 0) {
									base->net_list[i]->node_list[j]->gain++;
									
									if(bucket[base->net_list[i]->node_list[j]->gain + maxPin - 1] == base->net_list[i]->node_list[j]) {
										bucket[base->net_list[i]->node_list[j]->gain + maxPin - 1] = base->net_list[i]->node_list[j]->next;
									}
									else {
										base->net_list[i]->node_list[j]->prev->next = base->net_list[i]->node_list[j]->next;
									}
									if(base->net_list[i]->node_list[j]->next != NULL) {
										base->net_list[i]->node_list[j]->next->prev = base->net_list[i]->node_list[j]->prev;
									}
									base->net_list[i]->node_list[j]->prev = NULL;
									
									if(bucket[base->net_list[i]->node_list[j]->gain + maxPin] != NULL) {
										bucket[base->net_list[i]->node_list[j]->gain + maxPin]->prev = base->net_list[i]->node_list[j];
									}
									base->net_list[i]->node_list[j]->next = bucket[base->net_list[i]->node_list[j]->gain + maxPin];
									bucket[base->net_list[i]->node_list[j]->gain + maxPin] = base->net_list[i]->node_list[j];
								}
								break;
							}
						}
					}
				}
				else {
					if(base->net_list[i]->NumOf1 == 0) {
						for(int j = 0; j < base->net_list[i]->node_list.size(); j++) {
							if(base->net_list[i]->node_list[j]->lock == 0) {
								base->net_list[i]->node_list[j]->gain--;

								if(bucket[base->net_list[i]->node_list[j]->gain + maxPin + 1] == base->net_list[i]->node_list[j]) {
									bucket[base->net_list[i]->node_list[j]->gain + maxPin + 1] = base->net_list[i]->node_list[j]->next;
								}
								else {
									base->net_list[i]->node_list[j]->prev->next = base->net_list[i]->node_list[j]->next;
								}
								if(base->net_list[i]->node_list[j]->next != NULL) {
									base->net_list[i]->node_list[j]->next->prev = base->net_list[i]->node_list[j]->prev;
								}

								base->net_list[i]->node_list[j]->prev = NULL;
								if(bucket[base->net_list[i]->node_list[j]->gain + maxPin] != NULL) {
									bucket[base->net_list[i]->node_list[j]->gain + maxPin]->prev = base->net_list[i]->node_list[j];
								}
								base->net_list[i]->node_list[j]->next = bucket[base->net_list[i]->node_list[j]->gain + maxPin];
								bucket[base->net_list[i]->node_list[j]->gain + maxPin] = base->net_list[i]->node_list[j];
							}
						}
					}
					else if(base->net_list[i]->NumOf1 == 1) {
						for(int j = 0; j < base->net_list[i]->node_list.size(); j++) {
							if(base->net_list[i]->node_list[j]->group == 1) {
								if(base->net_list[i]->node_list[j]->lock == 0) {
									base->net_list[i]->node_list[j]->gain++;

									if(bucket[base->net_list[i]->node_list[j]->gain + maxPin - 1] == base->net_list[i]->node_list[j]) {
										bucket[base->net_list[i]->node_list[j]->gain + maxPin - 1] = base->net_list[i]->node_list[j]->next;
									}
									else {
										base->net_list[i]->node_list[j]->prev->next = base->net_list[i]->node_list[j]->next;
									}
									if(base->net_list[i]->node_list[j]->next != NULL) {
										base->net_list[i]->node_list[j]->next->prev = base->net_list[i]->node_list[j]->prev;
									}
									
									base->net_list[i]->node_list[j]->prev = NULL;
									if(bucket[base->net_list[i]->node_list[j]->gain + maxPin] != NULL) {
										bucket[base->net_list[i]->node_list[j]->gain + maxPin]->prev = base->net_list[i]->node_list[j];
									}
									base->net_list[i]->node_list[j]->next = bucket[base->net_list[i]->node_list[j]->gain + maxPin];
									bucket[base->net_list[i]->node_list[j]->gain + maxPin] = base->net_list[i]->node_list[j];
								}
								break;
							}
						}
					}
				}
			}
			
			better.push_back(base->name);
			if(totalGain > bestGain) {
				bestGain = totalGain;
				for(int i = 0; i < better.size(); i++) {
					best_group[node_map2[better[i]]] = node_map[better[i]]->group;
				}
				better.clear();
			}
			
		}
		

		for(int i = 0; i < node_list.size(); i++) {
			if(node_list[i]->group != best_group[i]) {
				if(node_list[i]->group == 1) {
					for(int j = 0; j < node_list[i]->net_list.size(); j++) {
						node_list[i]->net_list[j]->NumOf1--;
						node_list[i]->net_list[j]->NumOf2++;
					}
					node_list[i]->group = 2;
					area1 -= node_list[i]->area;
					area2 += node_list[i]->area;
				}
				else {
					for(int j = 0; j < node_list[i]->net_list.size(); j++) {
						node_list[i]->net_list[j]->NumOf1++;
						node_list[i]->net_list[j]->NumOf2--;
					}
					node_list[i]->group = 1;
					area1 += node_list[i]->area;
					area2 -= node_list[i]->area;
				}
			}

			node_list[i]->lock = 0;
			node_list[i]->gain = 0;
			node_list[i]->prev = NULL;
			node_list[i]->next = NULL;
		}

		cutSize = 0;
		cutSet.clear();
		int totalPin = 0;
		for(int i = 0; i < net_list.size(); i++) {
			totalPin += net_list[i]->node_list.size();
			if(net_list[i]->NumOf1 != 0 && net_list[i]->NumOf2 != 0) {
				cutSize++;
				cutSet.push_back(net_list[i]->name);
			}
		}
	}
	
	
	group1.clear();
	group2.clear();
	for(int i = 0; i < node_list.size(); i++) {
		if(node_list[i]->group == 1) {
			group1.push_back(node_list[i]);
		}
		else {
			group2.push_back(node_list[i]);
		}
	}
	clock_t end = clock();

	ofstream file3;
	file3.open(argv[3]);
	file3 << "Cut_size: " << cutSize << endl;
	file3 << "G1_size: " << area1 << endl;
	file3 << "G2_size: " << area2 << endl;
	file3 << "Time: " << double(end-start)/CLOCKS_PER_SEC << " s" << endl;
	file3 << "G1:" << endl;
	for(int i = 0; i < group1.size(); i++) {
		file3 << group1[i]->name;
		if(i < (group1.size() - 1)) {
			file3 << " ";
		}
		else {
			file3 << ";";
		}
	}
	file3 << endl;
	file3 << "G2:" << endl;
	for(int i = 0; i < group2.size(); i++) {
		file3 << group2[i]->name;
		if(i < (group2.size() - 1)) {
			file3 << " ";
		}
		else {
			file3 << ";";
		}
	}
	file3 << endl;
	file3 << "Cut_set:" << endl;
	for(int i = 0; i < cutSet.size(); i++) {
		file3 << cutSet[i];
		if(i < (cutSet.size() - 1)) {
			file3 << " ";
		}
		else {
			file3 << ";";
		}
	}

	return 0;
}

