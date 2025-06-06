#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <time.h>
#include <algorithm>
extern "C" {
    #include "../flute-3.1/flute.h"
}



using namespace std;

class net;

class cell {
public:
	string name;
    float ll_x; //lowerleft
    float ll_y;
	float x; 
	float y;
	float width;
	float height;

	cell() : name(""), ll_x(0.0), ll_y(0.0), x(0.0), y(0.0), width(0.0), height(0.0) {};
};

class net {
public:
    string name;
	int degree;
    vector<int> x;
    vector<int> y;
	Tree t;

	net() : name(""), degree(0), x(0.0), y(0.0), t() {};
};

class horizontal_edge {
public:
    int x1;
    int x2;
    int y;
    float demand;
    float supply;

    horizontal_edge() : x1(0), x2(0), y(0), demand(0.0), supply(0.0) {}; 
};

class vertical_edge {
public:
    int y1;
    int y2;
    int x;
    float demand;
    float supply;

    vertical_edge() : y1(0), y2(0), x(0), demand(0.0), supply(0.0) {}; 
};

void ReadFile(char *argv[], vector<net*> &net_list, vector<vector<horizontal_edge*>> &horizontal_matrix, vector<vector<vertical_edge*>> &vertical_matrix,
              int &edgeWidth, int &edgeHeight, int &numOfHorizontal, int &numOfVertical) {
	int numOfNode = 0;
	int numOfNet = 0;
	string s = "";
	string ss = "";
    float x_off = 0.0;
    float y_off = 0.0;
	unordered_map<string, cell*> cell_map;

    string bench = (string)argv[2];
    bench = bench.substr(0, bench.find("."));

	string fileName1 = bench + ".nodes";
	string fileName2 = bench + ".pl";
	string fileName3 = bench + ".nets";
	string fileName4 = bench + ".route";

	/////////////////////////////
	// 讀進 node、pl 檔
	// 記錄下 cell 的 dimension 跟位置
	/////////////////////////////

	ifstream file1;
	file1.open(fileName1);
	ifstream file2;
	file2.open(fileName2);

	getline(file1, s);
	getline(file1, s);
	getline(file1, s);

	getline(file2, s);

	file1 >> s >> s >> numOfNode;
	file1 >> s >> s >> s;

	for(int i = 0; i < numOfNode; i++) {
		cell* new_cell = new cell();
		file1 >> s;
        if(s == "terminal" || s == "terminal_NI") {
            file1 >> s;
        }
        file1 >> new_cell->width >> new_cell->height;
        new_cell->name = s;

		file2 >> s ;
        if(s == "/FIXED" || s == "/FIXED_NI") {
            file2 >> s;
        }
        file2 >> new_cell->ll_x >> new_cell->ll_y >> s >> s;
        new_cell->x = new_cell->ll_x + 0.5 * new_cell->width;
        new_cell->y = new_cell->ll_y + 0.5 * new_cell->height;
		cell_map[new_cell->name] = new_cell;
	}
	file1.close();
	file2.close();

	/////////////////////////////
	// 讀進 net 檔
	// 記錄下各個 pin 點座標
	/////////////////////////////

	ifstream file3;
	file3.open(fileName3);

	getline(file3, s);
	getline(file3, s);
	getline(file3, s);

	file3 >> s >> s >> numOfNet;
	file3 >> s >> s >> s;
	
	for (int i = 0; i < numOfNet; i++) {
		net* new_net = new net();
		file3 >> s >> s >> new_net->degree >> new_net->name;
        if(new_net->degree == 1) {
            file3 >> s >> s >> s >> s >> s;
            continue;
        }
		for (int j = 0; j < new_net->degree; j++) {
			file3 >> s >> ss >> ss >> x_off >> y_off;
            new_net->x.push_back(cell_map[s]->x + x_off);
            new_net->y.push_back(cell_map[s]->y + y_off);
		}
		net_list.push_back(new_net);
	}
	file3.close();

	/////////////////////////////
	// 讀進 route 檔
	// 建立 edge 模型
	/////////////////////////////
    
	ifstream file4;
	file4.open(fileName4);

	getline(file4, s);
	getline(file4, s);
	getline(file4, s);
	
    int x = 0;
    int numOfObstacle = 0;
    float horizontalCapacity = 0.0;
    float verticalCapacity = 0.0;
    int x_cor = 0;
    int y_cor = 0;
    int numOfLayer = 0;
    vector<int> wire_width;
    vector<int> wire_spacing;
    vector<cell*> obstacle;

	file4 >> s >> ss >> numOfHorizontal >> numOfVertical >> numOfLayer;
    file4 >> s >> s;
    for(int i = 0; i < numOfLayer; i++) {
        file4 >> x;
    }
    file4 >> s >> s;
    for(int i = 0; i < numOfLayer; i++) {
        file4 >> x;
    }
    file4 >> s >> s;
    for(int i = 0; i < numOfLayer; i++) {
        file4 >> x;
        wire_width.push_back(x);
    }
    file4 >> s >> s;
    for(int i = 0; i < numOfLayer; i++) {
        file4 >> x;
        wire_spacing.push_back(x);
    }
    file4 >> s >> s;
    for(int i = 0; i < numOfLayer; i++) {
        file4 >> s;
    }
    file4 >> s >> s >> x_cor >> y_cor;
    file4 >> s >> s >> edgeWidth >> edgeHeight;

    for(int i = 0; i < numOfLayer; i++) {
        horizontalCapacity += (float)edgeWidth / ((float)wire_width[i] + (float)wire_spacing[i]);
    }
    for(int i = 0; i < numOfLayer; i++) {
        verticalCapacity += (float)edgeHeight / ((float)wire_width[i] + (float)wire_spacing[i]);
    }

    for(int i = 0; i < numOfVertical+1; i++) {
        vector<horizontal_edge*> temp;
        for(int j = 0; j < numOfHorizontal; j++) {
            horizontal_edge* new_hor = new horizontal_edge();
            new_hor->x1 = x_cor + j * edgeWidth;
            new_hor->x2 = x_cor + (j + 1) * edgeWidth;
            new_hor->y = y_cor + i * edgeHeight;
            new_hor->supply = horizontalCapacity;
            temp.push_back(new_hor);
        }
        horizontal_matrix.push_back(temp);
    }
    for(int i = 0; i < numOfHorizontal+1; i++) {
        vector<vertical_edge*> temp;
        for(int j = 0; j < numOfVertical; j++) {
            vertical_edge* new_hor = new vertical_edge();
            new_hor->y1 = y_cor + j * edgeHeight;
            new_hor->y2 = y_cor + (j + 1) * edgeHeight;
            new_hor->x = x_cor + i * edgeWidth;
            new_hor->supply = verticalCapacity;
            temp.push_back(new_hor);
        }
        vertical_matrix.push_back(temp);
    }
    
    file4 >> s >> s >> s;
    file4 >> s >> s >> s;

    file4 >> s >> s >> numOfObstacle;

    for(int i = 0; i < numOfObstacle; i++) {
        file4 >> s >> x >> x >> x >> x >> x;
        cell* c = cell_map[s];

        int ii = ceil(c->ll_y / edgeHeight);
        int jj = c->ll_x / edgeWidth;
        int temp_i = ii;
        int temp_j = jj;

        while (1) {
            if(horizontal_matrix[temp_i][temp_j]->x2 >= c->ll_x + c->width) {
                if(horizontal_matrix[temp_i][temp_j]->x1 <= c->ll_x) {
                    for(int j = 0; j < 4; j++) {
                        horizontal_matrix[temp_i][temp_j]->supply -= (float)c->width / ((float)wire_width[j] + (float)wire_spacing[j]);
                    }
                }
                else {
                    int temp_w = c->ll_x + c->width - horizontal_matrix[temp_i][temp_j]->x1;
                    for(int j = 0; j < 4; j++) {
                        horizontal_matrix[temp_i][temp_j]->supply -= (float)temp_w / ((float)wire_width[j] + (float)wire_spacing[j]);
                    }
                }
                temp_i++;
                temp_j = jj;
            }
            else {
                if(horizontal_matrix[temp_i][temp_j]->x1 <= c->ll_x) {
                    int temp_w = horizontal_matrix[temp_i][temp_j]->x2 - c->ll_x;
                    for(int j = 0; j < 4; j++) {
                        horizontal_matrix[temp_i][temp_j]->supply -= (float)temp_w / ((float)wire_width[j] + (float)wire_spacing[j]);
                    }
                }
                else {
                    for(int j = 0; j < 4; j++) {
                        horizontal_matrix[temp_i][temp_j]->supply -= (float)edgeWidth / ((float)wire_width[j] + (float)wire_spacing[j]);
                    }
                }
                temp_j++;
            }
            if(horizontal_matrix[temp_i][temp_j]->y > c->ll_y + c->height) {
                break;
            }
        }

        ii = ceil(c->ll_x / edgeWidth);
        jj = c->ll_y / edgeHeight;
        temp_i = ii;
        temp_j = jj;

        while (1) {
            if(vertical_matrix[temp_i][temp_j]->y2 >= c->ll_y + c->height) {
                if(vertical_matrix[temp_i][temp_j]->y1 <= c->ll_y) {
                    for(int j = 0; j < 4; j++) {
                        vertical_matrix[temp_i][temp_j]->supply -= (float)c->height / ((float)wire_width[j] + (float)wire_spacing[j]);
                    }
                }
                else {
                    int temp_w = c->ll_y + c->height - vertical_matrix[temp_i][temp_j]->y1;
                    for(int j = 0; j < 4; j++) {
                        vertical_matrix[temp_i][temp_j]->supply -= (float)temp_w / ((float)wire_width[j] + (float)wire_spacing[j]);
                    }
                }
                temp_i++;
                temp_j = jj;
            }
            else {
                if(vertical_matrix[temp_i][temp_j]->y1 <= c->ll_y) {
                    int temp_w = vertical_matrix[temp_i][temp_j]->y2 - c->ll_y;
                    for(int j = 0; j < 4; j++) {
                        vertical_matrix[temp_i][temp_j]->supply -= (float)temp_w / ((float)wire_width[j] + (float)wire_spacing[j]);
                    }
                }
                else {
                    for(int j = 0; j < 4; j++) {
                        vertical_matrix[temp_i][temp_j]->supply -= (float)edgeHeight / ((float)wire_width[j] + (float)wire_spacing[j]);
                    }
                }
                temp_j++;
            }
            if(vertical_matrix[temp_i][temp_j]->x > c->ll_x + c->width) {
                break;
            }
        }
        
        

    }
    file4.close();

	return;
}

int potential(int l, int x, int u) {
    if(x >= l && x <= u) {
        return 1;
    }
    else {
        return 0;
    }
}

bool mycompare1(horizontal_edge* h1, horizontal_edge* h2) {
   return (h1->demand - h1->supply) > (h2->demand - h2->supply);
}

bool mycompare2(vertical_edge* v1, vertical_edge* v2) {
   return (v1->demand - v1->supply) > (v2->demand - v2->supply);
}

bool mycompare3(float f1, float f2){
   return f1 > f2;
}

void output(char *argv[], vector<horizontal_edge*> horizontal_overflow, vector<vertical_edge*> vertical_overflow,
            int widthConstraint, int heightConstraint, int edgeWidth, int edgeHeight) {
    string bench = (string)argv[2];
    bench = bench.substr(0, bench.find("."));

	string fileName1 = bench + "_Max_H_congestion.m";
	string fileName2 = bench + "_Max_V_congestion.m";

    ofstream file1;
	file1.open(fileName1);

    file1 << "axis equal;\n" << "hold on;\n" << "grid on;\n";
	file1 << "block_x=[0 0 " << widthConstraint << " " << widthConstraint << " 0];" << endl;
	file1 << "block_y=[0 " << heightConstraint << " " << heightConstraint << " 0 0];" << endl;
	file1 << "fill(block_x, block_y, 'b');" << endl;

    int bound = 0.1 * horizontal_overflow.size();
    for(int i = 0; i <= bound; i++) {
        file1 << "block_x=[" << horizontal_overflow[i]->x1 << " " << horizontal_overflow[i]->x1 << " ";
        file1 << horizontal_overflow[i]->x2 << " " << horizontal_overflow[i]->x2 << " " << horizontal_overflow[i]->x1 << "];" << endl;
        file1 << "block_y=[" << horizontal_overflow[i]->y - edgeHeight << " " << horizontal_overflow[i]->y + edgeHeight << " ";
        file1 << horizontal_overflow[i]->y + edgeHeight << " " << horizontal_overflow[i]->y - edgeHeight << " " << horizontal_overflow[i]->y - edgeHeight << "];" << endl;
        file1 << "fill(block_x, block_y, 'm');" << endl;
    }

    ofstream file2;
	file2.open(fileName2);

    file2 << "axis equal;\n" << "hold on;\n" << "grid on;\n";
	file2 << "block_x=[0 0 " << widthConstraint << " " << widthConstraint << " 0];" << endl;
	file2 << "block_y=[0 " << heightConstraint << " " << heightConstraint << " 0 0];" << endl;
	file2 << "fill(block_x, block_y, 'b');" << endl;

    bound = 0.1 * vertical_overflow.size();
    for(int i = 0; i <= bound; i++) {
        file2 << "block_x=[" << vertical_overflow[i]->x - edgeWidth << " " << vertical_overflow[i]->x - edgeWidth << " ";
        file2 << vertical_overflow[i]->x + edgeWidth << " " << vertical_overflow[i]->x + edgeWidth << " " << vertical_overflow[i]->x - edgeWidth << "];" << endl;
        file2 << "block_y=[" << vertical_overflow[i]->y1 << " " << vertical_overflow[i]->y2 << " ";
        file2 << vertical_overflow[i]->y2 << " " << vertical_overflow[i]->y1 << " " << vertical_overflow[i]->y1 << "];" << endl;
        file2 << "fill(block_x, block_y, 'm');" << endl;
    }

}

int main(int argc, char *argv[])
{
    clock_t START,END; 
    START = clock();

    int edgeWidth = 0;
    int edgeHeight = 0;
    int numOfHorizontal = 0;
    int numOfVertical = 0;
    vector<net*> net_list;
    vector<vector<horizontal_edge*>> horizontal_matrix;
    vector<vector<vertical_edge*>> vertical_matrix;
    vector<horizontal_edge*> horizontal_overflow;
    vector<vertical_edge*> vertical_overflow;
    vector<float> overflow;

    ReadFile(argv, net_list, horizontal_matrix, vertical_matrix, edgeWidth, edgeHeight, numOfHorizontal, numOfVertical);
    
    readLUT();
    
    for(int i = 0; i < net_list.size(); i++) {
        int* p1 = &net_list[i]->x[0];
        int* p2 = &net_list[i]->y[0];
        net_list[i]->t = flute(net_list[i]->degree, p1, p2, ACCURACY);
    }

    for(int i = 0; i < net_list.size(); i++) {
        for(int j = 0; j < 2 * net_list[i]->degree - 2; j++) {
            int x1 = 0;
            int y1 = 0;
            int x2 = 0;
            int y2 = 0;
            int ll_x = 0;
            int ll_y = 0;
            int ur_x = 0;
            int ur_y = 0;

            x1 = net_list[i]->t.branch[j].x;
            y1 = net_list[i]->t.branch[j].y;
            x2 = net_list[i]->t.branch[net_list[i]->t.branch[j].n].x;
            y2 = net_list[i]->t.branch[net_list[i]->t.branch[j].n].y;

            ll_x = min(x1, x2);
            ll_y = min(y1, y2);
            ur_x = max(x1, x2);
            ur_y = max(y1, y2);
            
            int ll_i = ceil((float)ll_y / (float)edgeHeight);
            int ll_j = ll_x / edgeWidth;
            int ur_i = floor((float)ur_y / (float)edgeHeight);
            int ur_j = ur_x / edgeWidth;

            for(int i = ll_i; i <= ur_i; i++) {
                for(int j = ll_j; j <= ur_j; j++) {
                    horizontal_matrix[i][j]->demand += 1 * potential(ll_y, horizontal_matrix[i][j]->y, ur_y) * (potential(horizontal_matrix[i][j]->x1, x1, horizontal_matrix[i][j]->x2) + potential(horizontal_matrix[i][j]->x1, x2, horizontal_matrix[i][j]->x2));
                }
            }
                
            ll_i = ceil((float)ll_x / (float)edgeWidth);
            ll_j = ll_y / edgeHeight; 
            ur_i = floor((float)ur_x / (float)edgeWidth);
            ur_j = ur_y / edgeHeight;

            for(int i = ll_i; i <= ur_i; i++) {
                for(int j = ll_j; j <= ur_j; j++) {
                    vertical_matrix[i][j]->demand += 1 * potential(ll_x, vertical_matrix[i][j]->x, ur_x) * (potential(vertical_matrix[i][j]->y1, y1, vertical_matrix[i][j]->y2) + potential(vertical_matrix[i][j]->y1, y2, vertical_matrix[i][j]->y2));
                }
            }
        }
    }

    
    

    for(int i = 0; i < horizontal_matrix.size(); i++) {
        for(int j = 0; j < horizontal_matrix[0].size(); j++) {
            if(horizontal_matrix[i][j]->demand > horizontal_matrix[i][j]->supply) {
                horizontal_overflow.push_back(horizontal_matrix[i][j]);
                overflow.push_back(horizontal_matrix[i][j]->demand - horizontal_matrix[i][j]->supply);
            }
        }
    }
    for(int i = 0; i < vertical_matrix.size(); i++) {
        for(int j = 0; j < vertical_matrix[0].size(); j++) {
            if(vertical_matrix[i][j]->demand > vertical_matrix[i][j]->supply) {
                vertical_overflow.push_back(vertical_matrix[i][j]);
                overflow.push_back(vertical_matrix[i][j]->demand - vertical_matrix[i][j]->supply);
            }
        }
    }

    

    sort(horizontal_overflow.begin(), horizontal_overflow.end(), mycompare1);
    sort(vertical_overflow.begin(), vertical_overflow.end(), mycompare2);
    output(argv, horizontal_overflow, vertical_overflow, numOfHorizontal * edgeWidth, numOfVertical * edgeHeight, edgeWidth, edgeHeight);

    sort(overflow.begin(), overflow.end(), mycompare3);

    float total = 0.0;
    int n1 = overflow.size() * 0.005;
    int n2 = overflow.size() * 0.01;
    int n3 = overflow.size() * 0.02;
    int n4 = overflow.size() * 0.05;
    float o1 = 0.0;
    float o2 = 0.0;
    float o3 = 0.0;
    float o4 = 0.0;

    for(int i = 0; i < overflow.size(); i++) {
        total += overflow[i];
        if(i < n1) {
            o1 += overflow[i];
        }
        if(i < n2) {
            o2 += overflow[i];
        }
        if(i < n3) {
            o3 += overflow[i];
        }
        if(i < n4) {
            o4 += overflow[i];
        }

    }

    o1 = o1 / (float)n1;
    o2 = o2 / (float)n2;
    o3 = o3 / (float)n3;
    o4 = o4 / (float)n4;

    END = clock();

    ofstream file;
	file.open(argv[4]);

    file << "runtime : " << (END - START) / CLOCKS_PER_SEC << " s" << endl;
    file << "total overflow : " << total << endl;
    file << "ACE\t\t0.50%\t1.00%\t2.00%\t5.00%" << endl;
    file << "   \t\t" << o1 << "\t" << o2 << "\t" << o3 << "\t" << o4 << endl;


    return 0;
}