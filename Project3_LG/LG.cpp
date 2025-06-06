#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <time.h>
#include <omp.h>

using namespace std;

class net;

class cell {
public:
	string name;
	int initial_x; //lowerleft
	int initial_y;
	int final_x;
	int final_y;
	int width;
	int height;
	int type; // 0 is movable; 1 is fixed; 2 is terminal_NI, 3 is non-rectangular 
	vector<cell*> cell_list;
	vector<net*> net_list;

	cell() : name(""), initial_x(0), initial_y(0), final_x(0), final_y(0), width(0), height(0), type(0), cell_list(NULL), net_list(NULL) {};
};

class netNode {
public:
	cell* netcell;
	double x_off;
	double y_off;

	netNode() : netcell(NULL), x_off(0.0), y_off(0.0) {};
};

class net {
public:
	int degree;
	double wirelength;
	double leftmost;
	double bottommost;
	double rightmost;
	double topmost;
	string name;
	vector<netNode*> cell_list;

	net() : degree(0), wirelength(0.0), leftmost(0.0), bottommost(0.0), rightmost(0.0), topmost(0.0),  name(""), cell_list(NULL) {};
};

class cluster {
public:
	vector<cell*> cell_list; 
	int xc;
	int ec;
	int qc;
	int wc;
	int numOfCell;

	cluster() : cell_list(NULL), xc(0), ec(0), qc(0), wc(0), numOfCell(0) {}; 
};

class row {
public:
	int x;
	int y;
	int width;
	int height;
	int space;
	vector<cluster*> cluster_list; 
	int numOfCluster;
	vector<cell*> movable_cell_list;
	vector<row*> subrow_list;

	row() : x(0), y(0), width(0), height(0), space(0), cluster_list(NULL), numOfCluster(0), movable_cell_list(0), subrow_list(0) {}; 
};

void splitRow(vector<row*> &row_list, row* &new_row, cell* a) {
	row* temp_row = new_row;
	
	int i = 0;
	for(i = 0; i < new_row->subrow_list.size(); i++) {
		if(a->initial_x >= new_row->subrow_list[i]->x &&
			a->initial_x + a->width <= new_row->subrow_list[i]->x + new_row->subrow_list[i]->width) {
			temp_row = new_row->subrow_list[i];
			break;
		}
	}

	if(a->initial_x == temp_row->x && a->initial_x + a->width == temp_row->x + temp_row->width) {
		if(temp_row == new_row) {
			row_list.pop_back();
			return;
		}
		else {
			new_row->subrow_list.erase(new_row->subrow_list.begin() + i);
			return;
		}
	}
	if(a->initial_x == temp_row->x && a->initial_x + a->width != temp_row->x + temp_row->width) {
		if(temp_row == new_row) {
			row* new_row2 = new row();
			new_row2->y = temp_row->y;
			new_row2->height = temp_row->height;

			new_row2->x = a->initial_x + a->width;
			new_row2->width = temp_row->x + temp_row->width - a->initial_x - a->width;
			new_row2->space = new_row2->width;
			new_row->subrow_list.push_back(new_row2);
			return;
		}
		else {
			temp_row->width = temp_row->x + temp_row->width - a->initial_x - a->width;
			temp_row->space = temp_row->width;
			temp_row->x = a->initial_x + a->width;
			return;
		}
	}
	if(a->initial_x != temp_row->x && a->initial_x + a->width == temp_row->x + temp_row->width) {
		if(temp_row == new_row) {
			row* new_row2 = new row();
			new_row2->y = temp_row->y;
			new_row2->height = temp_row->height;

			new_row2->x = temp_row->x;
			new_row2->width = a->initial_x - temp_row->x;
			new_row2->space = new_row2->width;
			new_row->subrow_list.push_back(new_row2);
			return;
		}
		else {
			temp_row->width = a->initial_x - temp_row->x;
			temp_row->space = temp_row->width;
			return;
		}
	}
	if(a->initial_x != temp_row->x && a->initial_x + a->width != temp_row->x + temp_row->width) {
		if(temp_row == new_row) {
			row* new_row2 = new row();
			new_row2->y = temp_row->y;
			new_row2->height = temp_row->height;

			row* new_row3 = new row();
			new_row3->y = temp_row->y;
			new_row3->height = temp_row->height;

			new_row2->x = temp_row->x;
			new_row2->width = a->initial_x - temp_row->x;
			new_row2->space = new_row2->width;
			new_row->subrow_list.push_back(new_row2);

			new_row3->x = a->initial_x + a->width;
			new_row3->width = temp_row->x + temp_row->width - a->initial_x - a->width;
			new_row3->space = new_row3->width;
			new_row->subrow_list.push_back(new_row3);

			return;
		}
		else {
			row* new_row2 = new row();
			new_row2->y = temp_row->y;
			new_row2->height = temp_row->height;

			new_row2->x = a->initial_x + a->width;
			new_row2->width = temp_row->x + temp_row->width - a->initial_x - a->width;
			new_row2->space = new_row2->width;
			new_row->subrow_list.push_back(new_row2);

			temp_row->width = a->initial_x - temp_row->x;
			temp_row->space = temp_row->width;
			
			return;
		}
	}
}

void ReadFile(char *argv[], vector<cell*> &movable_cell_list, vector<cell*> &terminal_cell_list, vector<cell*> &terminalNI_cell_list,
			  vector<net*> &net_list, vector<row*> &row_list) {
	int count = 0;
	int numOfMovable = 0;
	int numOfTerminal = 0;
	int numOfNonRectangular = 0;
	int numOfNet = 0;
	int numOfRow = 0;
	string s = "";
	string ss = "";
	unordered_map<string, cell*> cell_map;

	string fileName1 = (string)argv[2] + ".nodes";
	string fileName2 = (string)argv[2] + ".nets";
	string fileName3 = (string)argv[2] + ".shapes";
	string fileName4 = (string)argv[2] + ".legal";
	string fileName5 = (string)argv[2] + ".scl";

	/////////////////////////////
	// 讀進 node、legal 檔
	// 記錄下 cell 的 dimension 跟初始位置
	/////////////////////////////

	ifstream file1;
	file1.open(fileName1);
	ifstream file4;
	file4.open(fileName4);

	getline(file1, s);
	getline(file1, s);
	getline(file1, s);

	getline(file4, s);
	getline(file4, s);
	getline(file4, s);

	file1 >> s >> s >> numOfMovable;
	file1 >> s >> s >> numOfTerminal;
	numOfMovable -= numOfTerminal;

	for(int i = 0; i < numOfMovable; i++) {
		cell* new_cell = new cell();
		file1 >> new_cell->name >> new_cell->width >> new_cell->height;
		file4 >> s >> new_cell->initial_x >> new_cell->initial_y >> s >> s;
		/////////
		new_cell->final_x = new_cell->initial_x;
		new_cell->final_y = new_cell->initial_y;
		/////////
		cell_map[new_cell->name] = new_cell;
		movable_cell_list.push_back(new_cell);
	}

	for(int i = 0; i < numOfTerminal; i++) {
		cell* new_cell = new cell();
		file1 >> new_cell->name >> new_cell->width >> new_cell->height >> s;
		if(s == "terminal") {
			new_cell->type = 1;
			terminal_cell_list.push_back(new_cell);
		}
		else {
			new_cell->type = 2;
			terminalNI_cell_list.push_back(new_cell);
		}
		file4 >> s >> new_cell->initial_x >> new_cell->initial_y >> s >> s;
		/////////
		new_cell->final_x = new_cell->initial_x;
		new_cell->final_y = new_cell->initial_y;
		/////////
		cell_map[new_cell->name] = new_cell;
	}
	file1.close();
	file4.close();

	/////////////////////////////
	// 讀進 net 檔
	// 記錄下各個 net 連接到的 cell
	// cell 沒有記錄連到的 net
	/////////////////////////////

	ifstream file2;
	file2.open(fileName2);

	getline(file2, s);
	getline(file2, s);
	getline(file2, s);

	file2 >> s >> s >> numOfNet;
	file2 >> s >> s >> s;
	
	for (int i = 0; i < numOfNet; i++) {
		net* new_net = new net();
		file2 >> s >> s >> new_net->degree >> new_net->name;

		for (int j = 0; j < new_net->degree; j++) {
			netNode* new_netNode = new netNode();
			file2 >> s;
			new_netNode->netcell = cell_map[s];
			cell_map[s]->net_list.push_back(new_net);
			file2 >> s >> s >> new_netNode->x_off >> new_netNode->y_off;
			new_net->cell_list.push_back(new_netNode);
		}

		net_list.push_back(new_net);
	}
	file2.close();

	/////////////////////////////
	// 讀進 shape 檔
	// 將非矩形切割成多塊矩形
	// 用一大塊矩形的cell_list存這些子矩形
	/////////////////////////////

	ifstream file3;
	file3.open(fileName3);

	getline(file3, s);
	getline(file3, s);
	getline(file3, s);
	
	file3 >> s >> s >> numOfNonRectangular;

	for(int i = 0; i < numOfNonRectangular; i++) {
		file3 >> s >> ss >> count;
		cell_map[s]->type = 3;

		for(int j = 0; j < count; j++) {
			cell* new_cell = new cell(); 
			file3 >> ss >> new_cell->initial_x >> new_cell->initial_y >> new_cell->width >> new_cell->height;
			cell_map[s]->cell_list.push_back(new_cell);
		}
	}
	file3.close();

	/////////////////////////////
	// 讀進 scl 檔
	// 紀錄 row 的資訊以及寬度
	/////////////////////////////

	ifstream file5;
	file5.open(fileName5);

	getline(file5, s);
	getline(file5, s);
	getline(file5, s);

	file5 >> s >> s >> numOfRow;

	int Sitewidth = 0;
	int NumSites = 0;
	for(int i = 0; i < numOfRow; i++) { 
		row* new_row = new row();
		file5 >> s >> s;
		file5 >> s >> s >> new_row->y;
		file5 >> s >> s >> new_row->height;
		file5 >> s >> s >> Sitewidth;
		file5 >> s >> s >> s;
		file5 >> s >> s >> s;
		file5 >> s >> s >> s;
		file5 >> s >> s >> new_row->x;
		file5 >> s >> s >> NumSites;
		new_row->width = Sitewidth * NumSites;
		new_row->space = Sitewidth * NumSites;
		file5 >> s; 
		row_list.push_back(new_row);

		for(int j = 0; j < terminal_cell_list.size(); j++) {
			if(new_row->y >= terminal_cell_list[j]->initial_y && new_row->y + new_row->height <= terminal_cell_list[j]->initial_y + terminal_cell_list[j]->height) {
				if(terminal_cell_list[j]->type == 1) {
					splitRow(row_list, new_row, terminal_cell_list[j]);
				}
				else {
					for(int k = 0; k < terminal_cell_list[j]->cell_list.size(); k++) {
						if(new_row->y >= terminal_cell_list[j]->cell_list[k]->initial_y && 
						   new_row->y + new_row->height <= terminal_cell_list[j]->cell_list[k]->initial_y + terminal_cell_list[j]->cell_list[k]->height) {
							splitRow(row_list, new_row, terminal_cell_list[j]->cell_list[k]);
							break;
						}
					}
				}
			}
		}
	}
	file5.close();
}

bool mycompare(cell* c1, cell* c2){
   return c1->initial_x < c2->initial_x;
}

double Wirelength(vector<net*> net_list) {
	double leftmost = 0.0;
	double rightmost = 0.0;
	double bottommost = 0.0;
	double topmost = 0.0;
	double wirelength = 0.0;
 
	for(int i = 0; i < net_list.size(); i++) {
		leftmost = 1000000.0;
		rightmost = -1000000.0;
		bottommost = 1000000.0;
		topmost = -1000000.0;
		for(int j = 0; j < net_list[i]->cell_list.size(); j++) {
			if((float)net_list[i]->cell_list[j]->netcell->final_x + 0.5 * (float)net_list[i]->cell_list[j]->netcell->width + (float)net_list[i]->cell_list[j]->x_off < leftmost) {
				leftmost = (float)net_list[i]->cell_list[j]->netcell->final_x + 0.5 * (float)net_list[i]->cell_list[j]->netcell->width + (float)net_list[i]->cell_list[j]->x_off;
			}
			if((float)net_list[i]->cell_list[j]->netcell->final_x + 0.5 * (float)net_list[i]->cell_list[j]->netcell->width + (float)net_list[i]->cell_list[j]->x_off > rightmost) {
				rightmost = (float)net_list[i]->cell_list[j]->netcell->final_x + 0.5 * (float)net_list[i]->cell_list[j]->netcell->width + (float)net_list[i]->cell_list[j]->x_off;
			}
			if((float)net_list[i]->cell_list[j]->netcell->final_y + 0.5 * (float)net_list[i]->cell_list[j]->netcell->height + (float)net_list[i]->cell_list[j]->y_off < bottommost) {
				bottommost = (float)net_list[i]->cell_list[j]->netcell->final_y + 0.5 * (float)net_list[i]->cell_list[j]->netcell->height + (float)net_list[i]->cell_list[j]->y_off;
			}
			if((float)net_list[i]->cell_list[j]->netcell->final_y + 0.5 * (float)net_list[i]->cell_list[j]->netcell->height + (float)net_list[i]->cell_list[j]->y_off > topmost) {
				topmost = (float)net_list[i]->cell_list[j]->netcell->final_y + 0.5 * (float)net_list[i]->cell_list[j]->netcell->height + (float)net_list[i]->cell_list[j]->y_off;
			}
		}
		
		net_list[i]->wirelength = rightmost + topmost - leftmost - bottommost;

		wirelength = wirelength + rightmost + topmost - leftmost - bottommost;
	}

	return wirelength;
}

void addCell(cluster* &clu, cell* &c) {
	clu->ec++;
	clu->qc = clu->qc + c->initial_x - clu->wc;
	clu->wc += c->width;
}

void addCluster(cluster* &c1, cluster* &c2) {
	c1->ec += c2->ec;
	c1->qc = c1->qc + c2->qc - (c2->ec * c1->wc);
	c1->wc += c2->wc;
	for(int i = 0; i < c2->cell_list.size(); i++) {
		c1->cell_list.push_back(c2->cell_list[i]);
	}
}

void collapse(row* &r, cluster* &c) {
	c->xc = c->qc / c->ec;

	if(c->xc < r->x) {
		c->xc = r->x;
	}
	if((c->xc + c->wc) > (r->x + r->width)) {
		c->xc = r->x + r->width - c->wc;
	}

	int p = r->cluster_list.size()-2;
	if(p >= 0 && r->cluster_list[p]->xc + r->cluster_list[p]->wc > c->xc) {
		addCluster(r->cluster_list[p], c);
		r->cluster_list.pop_back();
		collapse(r, r->cluster_list.back());
	}
}

void PlaceRow(row* &row, cell* &cell) {
	cluster* last_cluster = NULL;
	if(!row->cluster_list.empty()) {
		last_cluster = row->cluster_list[row->cluster_list.size()-1];
	}

	int m = 0;
	if((row->x + row->width) < (cell->initial_x + cell->width)) {
		m = (cell->initial_x + cell->width) - (row->x + row->width);
	}

	if(row->cluster_list.empty() || last_cluster->xc + last_cluster->wc <= cell->initial_x - m) {
		cluster* new_cluster = new cluster();
		if(row->x <= cell->initial_x - m) {
            new_cluster->xc = cell->initial_x - m;
        }
        else {
            new_cluster->xc = row->x;
        }
		new_cluster->cell_list.push_back(cell); 
		addCell(new_cluster, cell);
		row->cluster_list.push_back(new_cluster);
		cell->final_x = new_cluster->xc;
		cell->final_y = row->y;

		return;
	}
	else {
		addCell(last_cluster, cell);
		last_cluster->cell_list.push_back(cell);
		collapse(row, last_cluster);
	}


	/*if(row->cluster_list.empty() || last_cluster->xc + last_cluster->wc <= temp_x) {
		cluster* new_cluster = new cluster();
		new_cluster->xc = temp_x;
		new_cluster->cell_list.push_back(cell); 
		addCell(new_cluster, cell);
		row->cluster_list.push_back(new_cluster);
		cell->final_x = temp_x;
		cell->final_y = row->y;

		return;
	}
	else {
		count = last_cluster->xc;
		addCell(last_cluster, cell);
		last_cluster->cell_list.push_back(cell);
		collapse(row, last_cluster, count);
	}*/
	

	for(int i = 0; i < row->cluster_list.size(); i++) {
		int x = row->cluster_list[i]->xc;
		for(int j = 0; j < row->cluster_list[i]->cell_list.size(); j++) {
				row->cluster_list[i]->cell_list[j]->final_x = x;
				row->cluster_list[i]->cell_list[j]->final_y = row->y;
				x += row->cluster_list[i]->cell_list[j]->width;
		}
	}
}

void testCollapse(row* r, int &ec, int &qc, int &wc, int &xc, int count) {
	xc = qc / ec;

	if(xc < r->x) {
		xc = r->x;
	}
	if((xc + wc) > (r->x + r->width)) {
		xc = r->x + r->width - wc;
	}

	int temp = 1000000;
	cluster* temp_c = NULL;
	for(int i = 0; i < r->cluster_list.size(); i++) {
		if(((r->cluster_list[i]->xc + r->cluster_list[i]->wc) > xc) && ((r->cluster_list[i]->xc + r->cluster_list[i]->wc) <= count)) {
			// if(r->cluster_list[i] == c) {
			// 	continue;
			// }
			if((count - r->cluster_list[i]->xc) < temp) {
				temp = count - r->cluster_list[i]->xc;
				temp_c = r->cluster_list[i];
			}
		}
	}

	if(temp_c != NULL) {
		qc = temp_c->qc + qc - (ec * temp_c->wc);
		ec += temp_c->ec;
		wc += temp_c->wc;

		count = temp_c->xc;
		testCollapse(r, ec, qc, wc, xc, count);
	}
}

void testPlaceRow(row* row, cell* &cell) {
	int temp_x = 0;
	int count = 0;
	int ec = 0;
	int qc = 0;
	int wc = 0;
	int xc = 0;

	cluster* last_cluster = NULL;
	if(!row->cluster_list.empty()) {
		last_cluster = row->cluster_list[row->cluster_list.size()-1];
	}

	if(cell->initial_x < row->x) {
		if(last_cluster != NULL) {
			temp_x = last_cluster->xc + last_cluster->wc - 1;
		}
		else {
			temp_x = row->x;
		}
	}
	else if(cell->initial_x + cell->width > row->x + row->width) {
		if(last_cluster != NULL) {
			temp_x = last_cluster->xc + last_cluster->wc - 1;
		}
		else {
			temp_x = row->x + row->width - cell->width;
		}
	}
	else {
		temp_x = cell->initial_x; 
	}
	
	if(row->cluster_list.empty() || last_cluster->xc + last_cluster->wc <= temp_x) {
		cell->final_x = temp_x;
		cell->final_y = row->y;
		return;
	}
	else {
		count = last_cluster->xc;
		ec = last_cluster->ec;
		qc = last_cluster->qc;
		wc = last_cluster->wc;
		ec++;
		qc = qc + cell->initial_x -wc;
		wc += cell->width;

		testCollapse(row, ec, qc, wc, xc, count);
	}
	cell->final_x = xc + wc - cell->width;
	cell->final_y = row->y;
	return;
}

double wireCost(cell* &cell, int type) {
	double leftmost = 0.0;
	double rightmost = 0.0;
	double bottommost = 0.0;
	double topmost = 0.0;
	double wirelength = 0.0;

	for(int i = 0; i < cell->net_list.size(); i++) {
		leftmost = 1000000.0;
		rightmost = -1000000.0;
		bottommost = 1000000.0;
		topmost = -1000000.0;
		for(int j = 0; j < cell->net_list[i]->cell_list.size(); j++) {
			if((float)cell->net_list[i]->cell_list[j]->netcell->final_x + 0.5 * (float)cell->net_list[i]->cell_list[j]->netcell->width + (float)cell->net_list[i]->cell_list[j]->x_off < leftmost) {
				leftmost = (float)cell->net_list[i]->cell_list[j]->netcell->final_x + 0.5 * (float)cell->net_list[i]->cell_list[j]->netcell->width + (float)cell->net_list[i]->cell_list[j]->x_off;
			}
			if((float)cell->net_list[i]->cell_list[j]->netcell->final_x + 0.5 * (float)cell->net_list[i]->cell_list[j]->netcell->width + (float)cell->net_list[i]->cell_list[j]->x_off > rightmost) {
				rightmost = (float)cell->net_list[i]->cell_list[j]->netcell->final_x + 0.5 * (float)cell->net_list[i]->cell_list[j]->netcell->width + (float)cell->net_list[i]->cell_list[j]->x_off;
			}
			if((float)cell->net_list[i]->cell_list[j]->netcell->final_y + 0.5 * (float)cell->net_list[i]->cell_list[j]->netcell->height + (float)cell->net_list[i]->cell_list[j]->y_off < bottommost) {
				bottommost = (float)cell->net_list[i]->cell_list[j]->netcell->final_y + 0.5 * (float)cell->net_list[i]->cell_list[j]->netcell->height + (float)cell->net_list[i]->cell_list[j]->y_off;
			}
			if((float)cell->net_list[i]->cell_list[j]->netcell->final_y + 0.5 * (float)cell->net_list[i]->cell_list[j]->netcell->height + (float)cell->net_list[i]->cell_list[j]->y_off > topmost) {
				topmost = (float)cell->net_list[i]->cell_list[j]->netcell->final_y + 0.5 * (float)cell->net_list[i]->cell_list[j]->netcell->height + (float)cell->net_list[i]->cell_list[j]->y_off;
			}
		}
		if(type == 1) {
			cell->net_list[i]->wirelength = rightmost + topmost - leftmost - bottommost;
		}

		wirelength = wirelength + rightmost + topmost - leftmost - bottommost;
	}
	
	return wirelength;
}

int main(int argc, char *argv[])
{
	// g++ -o LG_E24096205 LG_E24096205.cpp
	// ./LG_E24096205 -bench superblue1 -output LG_E24096205_superblue1.pl
	clock_t START,END, ttt; 
    START = clock();

	int count = 0;
	int best_cost = 1000000;
	int current_cost = 0;
	row* best_row = NULL;
	int up_row_index = 0;
	int down_row_index = 0;
	int leftbound = 0;
	int rightbound = 0;
	bool swit = 1;
	int index = 0;

	vector<cell*> movable_cell_list;
	vector<cell*> terminal_cell_list;
	vector<cell*> terminalNI_cell_list;
	vector<net*> net_list;
	vector<row*> row_list;

	END = clock();
    ReadFile(argv, movable_cell_list, terminal_cell_list, terminalNI_cell_list, net_list, row_list);
	ttt = clock();
	cout << (ttt - END) / CLOCKS_PER_SEC << endl;

	sort(movable_cell_list.begin(), movable_cell_list.end(), mycompare);

	END = clock();
	for(int i = 0; i < movable_cell_list.size(); i++) { //movable_cell_list.size()
		if(i % 100000 == 0) {
			ttt = clock();
			cout << (ttt - END) / CLOCKS_PER_SEC << endl;
			cout << "i = " << i << endl;
		}

		count = 0;
		best_cost = 2147483647;
		current_cost = 0;
		best_row = NULL;

		up_row_index = (movable_cell_list[i]->initial_y - row_list[0]->y) / row_list[0]->height;
		down_row_index = (movable_cell_list[i]->initial_y - row_list[0]->y) / row_list[0]->height - 1;
		swit = 1;
		index = 0;

		while(up_row_index <= row_list.size()-1 || down_row_index >= 0) {
			if(count >= 20 && best_row != NULL) { //7 3374
				break;
			}

			if(swit) {
				index = up_row_index;
				up_row_index++;
			}
			else {
				index = down_row_index;
				down_row_index--;
			}
			count++;

			if(row_list[index]->subrow_list.empty()) {
				if(row_list[index]->space < movable_cell_list[i]->width) {
					if(up_row_index <= row_list.size()-1 && !swit) {
						swit = 1;
						continue;
					}
					if(down_row_index >= 0 && swit) {
						swit = 0;
					}
					continue;
				}

				testPlaceRow(row_list[index], movable_cell_list[i]);

				current_cost = wireCost(movable_cell_list[i], 0);

				if(current_cost < best_cost) {
					best_cost = current_cost;
					best_row = row_list[index];
				}
			}
			else {
				leftbound = movable_cell_list[i]->initial_x - 0.03 * row_list[index]->width;
				rightbound = movable_cell_list[i]->initial_x + 0.03 * row_list[index]->width;

				for(int k = 0; k < row_list[index]->subrow_list.size(); k++) {
					if((row_list[index]->subrow_list[k]->x + row_list[index]->subrow_list[k]->width) < leftbound || row_list[index]->subrow_list[k]->x > rightbound) {
						continue;
					}
					if(row_list[index]->subrow_list[k]->space < movable_cell_list[i]->width) {
						continue;
					}

					testPlaceRow(row_list[index]->subrow_list[k], movable_cell_list[i]);

					current_cost = wireCost(movable_cell_list[i], 0);

					if(current_cost < best_cost) {
						best_cost = current_cost;
						best_row = row_list[index]->subrow_list[k];
					}
				}
			}
			
			if(up_row_index <= row_list.size()-1 && !swit) {
				swit = 1;
				continue;
			}
			if(down_row_index >= 0 && swit) {
				swit = 0;
			}
		}

		best_row->movable_cell_list.push_back(movable_cell_list[i]);
		PlaceRow(best_row, movable_cell_list[i]);
		best_row->space -= movable_cell_list[i]->width;
	}


	cout << fixed << Wirelength(net_list) << endl;

	ofstream file;
	file.open((string)argv[4]);

	file << "UCLA pl 1.0" << endl;
	file << "# Created  :  Sep 17 2012" << endl;
	file << "# User     :  Natarajan Viswanathan, IBM Corporation, Austin (nviswan@us.ibm.com)" << endl << endl;

    for(int i = 0; i < movable_cell_list.size(); i++) {
        file << movable_cell_list[i]->name << "\t\t" << movable_cell_list[i]->final_x << "\t" << movable_cell_list[i]->final_y << "\t:\tN" << endl;
    }
	for(int i = 0; i < terminal_cell_list.size(); i++) {
		file << terminal_cell_list[i]->name << "\t\t" << terminal_cell_list[i]->final_x << "\t" << terminal_cell_list[i]->final_y << "\t:\tN\t/FIXED" << endl;
	}
	for(int i = 0; i < terminalNI_cell_list.size(); i++) {
		file << terminalNI_cell_list[i]->name << "\t\t" << terminalNI_cell_list[i]->final_x << "\t" << terminalNI_cell_list[i]->final_y << "\t:\tN\t/FIXED_NI" << endl;
	}
	file.close();

	ttt = clock();
	cout << (ttt - START) / CLOCKS_PER_SEC << endl;

	return 0;
}

