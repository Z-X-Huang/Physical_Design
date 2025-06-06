#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <cstdlib> 
#include <ctime>
#include <queue>
#include <stack>
#include <math.h>
#include <random>
#include <time.h>

using namespace std;

class BinaryTree;
struct Net;

class TreeNode{
public:
    TreeNode *leftchild;
    TreeNode *rightchild;
    TreeNode *parent;
    string str;
    int width;
    int height;
    int rotate;
    int x_cor;
    int y_cor;

    TreeNode() : leftchild(NULL),rightchild(NULL),parent(NULL),str(""),width(0),height(0),rotate(0),x_cor(0),y_cor(0){};
    TreeNode(string s) : leftchild(NULL),rightchild(NULL),parent(NULL),str(s),width(0),height(0){};

    friend class BTree;
};

class BTree{
public:
    TreeNode *root;         
    float cost;
    int area;
    float wirelength;
    int width;
    int height;
    BTree() : root(0),cost(0),area(0),wirelength(0.0),width(0),height(0){};
    BTree(TreeNode *node, float cost) : root(node),cost(cost),area(0),wirelength(0.0),width(0),height(0){};

    void Levelorder();
    TreeNode* leftmost(TreeNode *current);
    TreeNode* rightmost(TreeNode *current);
    void Insert_read(vector<TreeNode*> &node_list, int widthConstraint);
    void Insert(TreeNode* a);
    void Delete(TreeNode* a);
    void Packing(int widthConstraint, int heightConstraint, float aspectRatio, float alpha);
    void cal_wirelength(vector<Net*> net_list, vector<TreeNode*> &node_list);
    void swap(TreeNode* a, TreeNode* b);
};

struct Net {
	string name;
	vector<string> node_list;
};

class linkedNode {
public:
    int y;
    int x1;
    int x2;
    linkedNode* prev;
    linkedNode* next;

    linkedNode() : y(0),x1(0),x2(0),prev(NULL),next(NULL){};
    linkedNode(int y, int x1, int x2) : y(y),x1(x1),x2(x2),prev(NULL),next(NULL){};
};

void BTree::Levelorder(){
    queue<TreeNode* > q;
    q.push(this->root);                     
    int count = 0;
    while(!q.empty()) {                     
        count++;
        TreeNode* current = q.front();      
        q.pop();                          
        cout << current->str << " ";   

        if(current->leftchild != NULL) {   
            q.push(current->leftchild);
        }
        if(current->rightchild != NULL) {  
            q.push(current->rightchild);
        }
    }
    cout << endl;
}

TreeNode* BTree::leftmost(TreeNode *current){
    while (current->leftchild != NULL){
        current = current->leftchild;
    }
    return current;
}

TreeNode* BTree::rightmost(TreeNode *current){
    while (current->rightchild != NULL){
        current = current->rightchild;
    }
    return current;
}

void BTree::Insert_read(vector<TreeNode*> &node_list, int widthConstraint){
    int right_bound = 0;
    TreeNode* top;
    TreeNode* right;
    for(int i = 0; i < node_list.size(); i++) {
        if(this->root == NULL){
            this->root = node_list[i];
            right_bound += node_list[i]->width;
            right = node_list[i];
            top = node_list[i];
        }
        else {
            if(right_bound + node_list[i]->width <= widthConstraint) {
                right->leftchild = node_list[i];
                node_list[i]->parent = right;
                right_bound += node_list[i]->width;
                right = node_list[i];
            }
            else {
                top->rightchild = node_list[i];
                node_list[i]->parent = top;
                right = node_list[i];
                right_bound = node_list[i]->width;
                top = node_list[i];
            }
        }
    }
}

void BTree::Insert(TreeNode* a){
    int x = 0;
    int y = 0;
    TreeNode* temp = this->root;

    while(1) {
        x = rand();
        x = x % 2;

        if(x == 1) {
            if(temp->leftchild == NULL) {
                temp->leftchild = a;
                a->parent = temp;
                return;
            }
            else {
                temp = temp->leftchild;
            }
        }
        else {
            if(temp->rightchild == NULL) {
                temp->rightchild = a;
                a->parent = temp;
                return;
            }
            else {
                temp = temp->rightchild;
            }
        }
    }
}

void BTree::Delete(TreeNode* a) {
    if(a->leftchild == NULL && a->rightchild == NULL) {
        if(a->parent == NULL) {
            this->root = NULL;
        }
        else if(a == a->parent->leftchild) {
            a->parent->leftchild = NULL;
        }
        else {
             a->parent->rightchild = NULL;
        }
    }
    else if(a->leftchild != NULL && a->rightchild == NULL) {
        if(a->parent == NULL) {
            this->root = a->leftchild;
            a->leftchild->parent = NULL;
        }
        else if(a == a->parent->leftchild) {
            a->parent->leftchild = a->leftchild;
            a->leftchild->parent = a->parent;
        }
        else {
             a->parent->rightchild = a->leftchild;
             a->leftchild->parent = a->parent;
        }
    }
    else if(a->leftchild == NULL && a->rightchild != NULL) {
        if(a->parent == NULL) {
            this->root = a->rightchild;
            a->rightchild->parent = NULL;
        }
        else if(a == a->parent->leftchild) {
            a->parent->leftchild = a->rightchild;
            a->rightchild->parent = a->parent;
        }
        else {
             a->parent->rightchild = a->rightchild;
             a->rightchild->parent = a->parent;
        }
    }
    else {
        TreeNode* temp = this->leftmost(a);

        if(temp->rightchild == NULL) {
            temp->parent->leftchild = NULL;
        }
        else {
            temp->parent->leftchild = temp->rightchild;
            temp->rightchild = temp->parent;
        }

        if(a->parent == NULL) {
            this->root = temp;
            temp->parent = NULL;
        }
        else if(a == a->parent->leftchild) {
            a->parent->leftchild = temp;
            temp->parent = a->parent;
        }
        else {
            a->parent->rightchild = temp;
            temp->parent = a->parent;
        }
        if(a->leftchild != NULL) {
            temp->leftchild = a->leftchild;
            a->leftchild->parent = temp;
        }
        temp->rightchild = a->rightchild;
        a->rightchild->parent = temp;
    }
    a->parent = NULL;
    a->leftchild = NULL;
    a->rightchild = NULL;
}

void InsertionSort(vector<TreeNode*> &node_list){
    for (int i = 1; i < node_list.size(); i++) {
        TreeNode* temp = node_list[i];
        int key = node_list[i]->width * node_list[i]->height;
        int j = i - 1;
        while (j >= 0 && key > (node_list[j]->width * node_list[j]->height)) {
            node_list[j+1] = node_list[j];
            j--;
        }
        node_list[j+1] = temp;
    }
}

BTree* ReadFile(char argv[], unordered_map<string, Net*> &net_map, vector<TreeNode*> &node_list, 
                vector<Net*> &net_list, int &totalArea, int &widthConstraint, int &heightConstraint, float whitespace, float aspectRatio) {

    ifstream file1;
	file1.open(argv);

    string s = "";
    int ii = 0;
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;

    while(s != "MODULE") {
        file1 >> s;
    }

    BTree* tree = new BTree();

    while(1) {
        
        TreeNode* newTreeNode = new TreeNode();
        
        file1 >> s;
        s = s.substr(0, s.length() - 1);
        newTreeNode->str = s;
        file1 >> s >> s;

        if(s == "PARENT;") {
            delete newTreeNode;
            break;
        }

        file1 >> s >> ii >> ii >> x1 >> y1 >> ii >> ii >> x2 >> y2;
        newTreeNode->width = x1 - x2;
        newTreeNode->height = y1 - y2;
        if(newTreeNode->width > newTreeNode->height) {
            int t = newTreeNode->width;
            newTreeNode->width = newTreeNode->height;
            newTreeNode->height = t;
            newTreeNode->rotate = 1;
        }
        node_list.push_back(newTreeNode);
        totalArea += newTreeNode->width * newTreeNode->height;

        

        while(s != "ENDMODULE;") {
            getline(file1, s);
        }
        file1 >> s;
    }
    widthConstraint = sqrt(((1 + whitespace) * totalArea) / aspectRatio);
    heightConstraint = sqrt((1 + whitespace) * totalArea * aspectRatio);

    InsertionSort(node_list);
    tree->Insert_read(node_list, widthConstraint);

    while(s != " NETWORK;") { 
        getline(file1, s);  
    }

    while(1) {
        file1 >> s;
        if(s == "ENDNETWORK;") {
            break;
        }
        string ss = "";
        file1 >> ss;
        int valid = 1;
        while(valid) {
            file1 >> s;
            if(s.back() == ';') {
                s = s.substr(0, s.length() - 1);
                valid = 0;
            }

            if(net_map[s] == NULL) {
                Net* new_net = new Net;
                new_net->name = s;
                net_map[s] = new_net;
                new_net->node_list.push_back(ss);
                net_list.push_back(new_net);
            }
            else {
                net_map[s]->node_list.push_back(ss);
            }
        }
    }
    


    return tree;

}


BTree* CopyTree(BTree* tree, vector<TreeNode*> &next_node_list) {
    BTree* tree2 = new BTree();
    tree2->cost = tree->cost;
    tree2->area = tree->area;
    tree2->wirelength = tree->wirelength;
    tree2->width = tree->width;
    tree2->height = tree->height;
    
    next_node_list.clear();

    TreeNode* a = new TreeNode();
    queue<TreeNode*> q1;
    queue<TreeNode*> q2;
    *a = *tree->root;
    a->leftchild = NULL;
    a->rightchild = NULL;

    tree2->root = a;
    next_node_list.push_back(a);
    q1.push(tree->root);
    q2.push(a);
    while(!q1.empty()) {
        TreeNode* temp1 = q1.front();
        TreeNode* temp2 = q2.front();
        q1.pop();
        q2.pop();
        if(temp1->leftchild != NULL) {
            TreeNode* b = new TreeNode();
            *b = *temp1->leftchild;
            b->parent = temp2;
            b->leftchild = NULL;
            b->rightchild = NULL;
            
            next_node_list.push_back(b);
            temp2->leftchild = b;
            q1.push(temp1->leftchild);
            q2.push(b);
        }
        if(temp1->rightchild != NULL) {
            TreeNode* b = new TreeNode();
            *b = *temp1->rightchild;
            b->parent = temp2;
            b->leftchild = NULL;
            b->rightchild = NULL;
            
            next_node_list.push_back(b);
            temp2->rightchild = b;
            q1.push(temp1->rightchild);
            q2.push(b);
        }
    }
    return tree2;
}

void BTree::Packing(int widthConstraint, int heightConstraint, float aspectRatio, float alpha) {
    float beta = 0;
    float left_boundary = 1000000;
    float bottom_boundary = 1000000;
    float right_boundary = -1000000;
    float top_boundary = -1000000;

    stack<TreeNode*> s;
    this->root->x_cor = 0;
    this->root->y_cor = 0;
    linkedNode* new_linkedNode1 = new linkedNode(this->root->y_cor + this->root->height, this->root->x_cor, this->root->x_cor + this->root->width);
    linkedNode* new_linkedNode2 = new linkedNode(0, this->root->x_cor + this->root->width, 1000000);
    new_linkedNode1->next = new_linkedNode2;
    new_linkedNode2->prev = new_linkedNode1;
    linkedNode* head = new_linkedNode1;

    if(this->root->x_cor < left_boundary) {
        left_boundary = this->root->x_cor;
    }
    if(this->root->y_cor < bottom_boundary) {
        bottom_boundary = this->root->y_cor;
    }
    if(this->root->x_cor + this->root->width > right_boundary) {
        right_boundary = this->root->x_cor + this->root->width;
    }
    if(this->root->y_cor + this->root->height > top_boundary) {
        top_boundary = this->root->y_cor + this->root->height;
    }

    if( this->root->rightchild != NULL) {
        s.push(this->root->rightchild);
    }
    if( this->root->leftchild != NULL) {
        s.push(this->root->leftchild);
    }

    while (s.size() > 0) {
        TreeNode *node = s.top();
        s.pop();

        if(node == node->parent->leftchild) {
            node->x_cor = node->parent->x_cor + node->parent->width;
        }
        else {
            node->x_cor = node->parent->x_cor;
        }

        linkedNode* temp = head;
        linkedNode* start = NULL;
        linkedNode* end = NULL;
        int y_max = 0;

        while(temp != NULL) {
            if(node->x_cor >= temp->x1 && node->x_cor < temp->x2) {
                start = temp;
                y_max = temp->y;
            }
            if(node->x_cor + node->width > temp->x1 && node->x_cor + node->width <= temp->x2) {
                end = temp;
                if(y_max < temp->y) {
                    y_max = temp->y;
                }
                break;
            }
            if(node->x_cor < temp->x1 && node->x_cor + node->width > temp->x2) {
                if(y_max < temp->y) {
                    y_max = temp->y;
                }
            }
            temp = temp->next;
        }
        node->y_cor = y_max;

        if(start == end) {
            if(start->x1 == node->x_cor && start->x2 == node->x_cor + node->width)  {
                start->y = node->y_cor + node->height;
            }
            else if(start->x1 == node->x_cor && start->x2 != node->x_cor + node->width)  {
                linkedNode* new_node = new linkedNode(start->y, node->x_cor + node->width, start->x2);
                start->x2 = node->x_cor + node->width;
                start->y = node->y_cor + node->height;
                new_node->prev = start;
                new_node->next = start->next;
                if(start->next != NULL) {
                    start->next->prev = new_node;
                }
                start->next = new_node;
            }
            else if(start->x1 != node->x_cor && start->x2 == node->x_cor + node->width)  {
                linkedNode* new_node = new linkedNode(node->y_cor + node->height, node->x_cor, node->x_cor + node->width);
                start->x2 = node->x_cor;
                new_node->prev = start;
                new_node->next = start->next;
                if(start->next != NULL) {
                    start->next->prev = new_node;
                }
                start->next = new_node;
            }
            else {
                linkedNode* new_end = new linkedNode(start->y, node->x_cor + node->width, start->x2);
                linkedNode* new_node = new linkedNode(node->y_cor + node->height, node->x_cor, node->x_cor + node->width);
                start->x2 = node->x_cor;
                new_end->prev = new_node;
                new_node->prev = start;
                new_node->next = new_end;
                new_end->next = start->next;
                if(start->next != NULL) {
                    start->next->prev = new_end;
                }
                start->next = new_node;
            }
        }
        else {
            if(start->x1 == node->x_cor && end->x2 == node->x_cor + node->width)  {
                start->y = node->y_cor + node->height;
                start->x2 = end->x2;
                linkedNode* te = start->next;
                linkedNode* tee;
                start->next = end->next;
                end->next->prev = start;
                while(te != start->next) {
                    tee = te->next;
                    delete te;
                    te = tee;
                }
            }
            else if(start->x1 == node->x_cor && end->x2 != node->x_cor + node->width)  {
                start->x2 = node->x_cor + node->width;
                start->y = node->y_cor + node->height;
                end->x1 = node->x_cor + node->width;
                linkedNode* te = start->next;
                linkedNode* tee;
                start->next = end;
                while(te != start->next) {
                    tee = te->next;
                    delete te;
                    te = tee;
                }
                end->prev = start;
            }
            else if(start->x1 != node->x_cor && end->x2 == node->x_cor + node->width)  {
                start->x2 = node->x_cor;
                end->x1 = node->x_cor;
                end->y = node->y_cor + node->height;
                linkedNode* te = start->next;
                linkedNode* tee;
                start->next = end;
                while(te != start->next) {
                    tee = te->next;
                    delete te;
                    te = tee;
                }
                end->prev = start;
            }
            else {
                linkedNode* new_node = new linkedNode(node->y_cor + node->height, node->x_cor, node->x_cor + node->width);
                start->x2 = node->x_cor;
                end->x1 = node->x_cor + node->width;
                new_node->prev = start;
                new_node->next = end;
                linkedNode* te = start->next;
                linkedNode* tee;
                while(te != end) {
                    tee = te->next;
                    delete te;
                    te = tee;
                }
                start->next = new_node;
                end->prev = new_node;
            }
        }

        if(node->x_cor < left_boundary) {
            left_boundary = node->x_cor;
        }
        if(node->y_cor < bottom_boundary) {
            bottom_boundary = node->y_cor;
        }
        if(node->x_cor + node->width > right_boundary) {
            right_boundary = node->x_cor + node->width;
        }
        if(node->y_cor + node->height > top_boundary) {
            top_boundary = node->y_cor + node->height;
        }

        if(node->rightchild != NULL) {
            s.push(node->rightchild);
        }
        if(node->leftchild != NULL) {
            s.push(node->leftchild);
        }
    }
    
    this->width = right_boundary - left_boundary;
    this->height = top_boundary - bottom_boundary;
    this->area = this->width * this->height;

    float asp = 0.0;
    if(this->width > this->height) {
        asp = (float)this->width / (float)this->height;
    }
    else {
        asp = (float)this->height / (float)this->width;
    }
    this->cost = alpha * (float)this->area + beta * this->wirelength + (1 - alpha - beta) * (float)pow((aspectRatio - asp), 2);
    
    linkedNode* t = head;
    linkedNode* tt;
    while(t != NULL) {
        tt = t->next;
        delete t;
        t = tt;
    }
}

void BTree::cal_wirelength(vector<Net*> net_list, vector<TreeNode*> &node_list) {
    float left_boundary = 1000000;
    float bottom_boundary = 1000000;
    float right_boundary = -1000000;
    float top_boundary = -1000000;
    for(int i = 0; i < net_list.size(); i++) {
        if(net_list[i]->node_list.size() > 1) {
            left_boundary = 1000000.0;
            bottom_boundary = 1000000.0;
            right_boundary = -1000000.0;
            top_boundary = -1000000.0;
            for(int j = 0; j < net_list[i]->node_list.size(); j++) {
                for(int k = 0; k < node_list.size(); k++) {
                    if(net_list[i]->node_list[j] == node_list[k]->str) {
                        if(node_list[k]->x_cor + node_list[k]->width/2 < left_boundary) {
                            left_boundary = (float)node_list[k]->x_cor + (float)node_list[k]->width/2;
                        }
                        if(node_list[k]->x_cor + node_list[k]->width/2 > right_boundary) {
                            right_boundary = (float)node_list[k]->x_cor + (float)node_list[k]->width/2;
                        }
                        if(node_list[k]->y_cor + node_list[k]->height/2 < bottom_boundary) {
                            bottom_boundary = (float)node_list[k]->y_cor + (float)node_list[k]->height/2;
                        }
                        if(node_list[k]->y_cor + node_list[k]->height/2 > top_boundary) {
                            top_boundary = (float)node_list[k]->y_cor + (float)node_list[k]->height/2;
                        }
                    }
                }
            }
            this->wirelength += (right_boundary - left_boundary);
            this->wirelength += (top_boundary - bottom_boundary);
        }
    }
}

void BTree::swap(TreeNode* a, TreeNode* b) {
    TreeNode* temp1 = new TreeNode();
    TreeNode* temp2 = new TreeNode();

    *temp1 = *a;
    *temp2 = *b;
    *a = *b;
    a->parent = temp1->parent;
    a->leftchild = temp1->leftchild;
    a->rightchild = temp1->rightchild;
    *b = *temp1;
    b->parent = temp2->parent;
    b->leftchild = temp2->leftchild;
    b->rightchild = temp2->rightchild;
}

void outputMatlab(char argv[], vector<TreeNode*> node_list, int widthConstraint, int heightConstraint) {
    ofstream file2;
	file2.open(argv);

	file2 << "axis equal;\n" << "hold on;\n" << "grid on;\n";
	file2 << "block_x=[0 0 " << widthConstraint << " " << widthConstraint << " 0];" << endl;
	file2 << "block_y=[0 " << heightConstraint << " " << heightConstraint << " 0 0];" << endl;
	file2 << "fill(block_x, block_y, 'c');" << endl;

	for (int i = 0; i < node_list.size(); i++)
	{
        file2 << "block_x=[" << node_list[i]->x_cor << " " << node_list[i]->x_cor << " ";
        file2 << node_list[i]->x_cor + node_list[i]->width << " " << node_list[i]->x_cor + node_list[i]->width << " " << node_list[i]->x_cor << "];" << endl;
        file2 << "block_y=[" << node_list[i]->y_cor << " " << node_list[i]->y_cor + node_list[i]->height << " ";
        file2 << node_list[i]->y_cor + node_list[i]->height << " " << node_list[i]->y_cor << " " << node_list[i]->y_cor << "];" << endl;
        file2 << "fill(block_x, block_y, 'y');" << endl;
	}
}

void outputOut(vector<TreeNode*> node_list, char argv[]) {
    ofstream file3;
	file3.open(argv);

    for(int i = 0; i < node_list.size(); i++) {
        file3 << node_list[i]->str << " " << node_list[i]->x_cor << " " << node_list[i]->y_cor << " " << node_list[i]->rotate << endl;
    }
}

int main(int argc, char *argv[]) {
    clock_t START,END, ttt; 
    START = clock();

    float aspectRatio = (float)atoi(argv[2]);
    float whitespace = (float)atoi(argv[3]) / 100;
    int totalArea = 0;
    int widthConstraint = 0;
    int heightConstraint = 0;
    float temperature = 0.0;
    float decreasingFactor = 0.99;
    int MT = 1;
    int uphill = 0;
    int reject = 0;
    float alpha = 0.0;
    int op = 2;
    vector<TreeNode*> node_list;
    vector<TreeNode*> initial_node_list;
    vector<Net*> net_list;
    vector<TreeNode*> next_node_list;
    vector<TreeNode*> best_node_list;
	unordered_map<string, Net*> net_map;

    if (string(argv[1]) == "ami33.yal")
    {
        alpha = 0.0000001;
    }
    else if (string(argv[1]) == "ami49.yal")
    {
        alpha = 0.000000001;
    }
    else
    {
        alpha = 0.0000001;
        op = 3;
        temperature = 100000.0;
    }

    BTree* initial;
    BTree* best;
    BTree* next;
    BTree* tree = ReadFile(argv[1], net_map, node_list, net_list, totalArea, widthConstraint, heightConstraint, whitespace, aspectRatio);
    int N = 5 * node_list.size();

    tree->Packing(widthConstraint, heightConstraint, aspectRatio, alpha);
    initial = CopyTree(tree, initial_node_list);
    best = CopyTree(tree, best_node_list);
    outputMatlab(argv[6], best_node_list, widthConstraint, heightConstraint);

    cout << ": " << time(NULL) << endl;
    srand(time(NULL));//time(NULL)
    int x = 0;
    int y = 0;
    int z = 0;
    float prob = rand() / RAND_MAX;
    int count = 1;
    int count_ini = 1;
    
    while(((reject/(float)MT) < 0.95 && temperature > 0.01) || best->width > widthConstraint || best->height > heightConstraint) { 

        ttt = clock();
        if((ttt - START) / CLOCKS_PER_SEC > 600) {
            break;
        }
        MT = 0; 
        uphill = 0; 
        reject = 0; 

        
        if(count % 1000000 == 0) {
            count_ini++;
            cout << "                  count " << count_ini << endl;
            if(count_ini == 50) {
                break;
            }
            delete tree;
            for(int i = 0; i < node_list.size(); i++) {
                delete node_list[i];
            }
            tree = CopyTree(initial, initial_node_list);

            best->cost = 1000;

            delete next;
            for(int i = 0; i < next_node_list.size(); i++) {
                delete next_node_list[i];
            }
            next = CopyTree(initial, initial_node_list);
        }

        while(MT < 2 * N && uphill < N) {
            count++;

            if(count % 1000000 == 0) {
                break;
            }
            
            MT++;
            delete next;
            for(int i = 0; i < next_node_list.size(); i++) {
                delete next_node_list[i];
            }
            next = CopyTree(tree, next_node_list);

            x = rand();
            x = x % op;
            if(x == 2) {
                y = rand();
                y = y % next_node_list.size();

                int temp = next_node_list[y]->width;
                next_node_list[y]->width = next_node_list[y]->height;
                next_node_list[y]->height = temp;
                if(next_node_list[y]->rotate == 0) {
                    next_node_list[y]->rotate = 1;
                }
                else {
                    next_node_list[y]->rotate = 0;
                }
            }
            if(x == 0) {
                y = rand();
                y = y % next_node_list.size();
                /*int c = 0;
                while(next_node_list[y]->x_cor + next_node_list[y]->width > widthConstraint || next_node_list[y]->y_cor + next_node_list[y]->height > heightConstraint) {
                    y = rand();
                    y = y % next_node_list.size();
                    c++;
                    if(c == 5) {
                        break;
                    }
                }*/

                next->Delete(next_node_list[y]);
                next->Insert(next_node_list[y]);
            }
            else {
                y = rand();
                y = y % next_node_list.size();
                z = rand();
                z = z % next_node_list.size();
                
                while(z == y) {
                    z = rand();
                    z = z % next_node_list.size();
                }
                next->swap(next_node_list[y], next_node_list[z]);
            }

            next->Packing(widthConstraint, heightConstraint, aspectRatio, alpha);
            
            if(next->width <= widthConstraint && next->height <= heightConstraint) {
                cout << count << endl;
                cout << best->cost << " " << next->cost << endl;
                cout << next->width << " " << next->height << endl;
                cout << widthConstraint << " " << heightConstraint << endl;
                cout << "FUCK" << endl;
                delete best;
                for(int i = 0; i < best_node_list.size(); i++) {
                    delete best_node_list[i];
                }
                best = CopyTree(next, best_node_list);
                break;
            }
            
            float difference = next->cost - tree->cost;
            float ram = (double)rand() / (double)RAND_MAX;

            if(difference < 0) {
                delete tree;
                for(int i = 0; i < node_list.size(); i++) {
                    delete node_list[i];
                }
                tree = CopyTree(next, node_list);

                if(next->cost < best->cost) {
                    cout << count << endl;
                    cout << best->cost << " " << next->cost << endl;
                    cout << next->width << " " << next->height << endl;
                    cout << widthConstraint << " " << heightConstraint << endl;
                    delete best;
                    for(int i = 0; i < best_node_list.size(); i++) {
                        delete best_node_list[i];
                    }
                    best = CopyTree(next, best_node_list);

                    if(best->width <= widthConstraint && best->height <= heightConstraint) {
                        break;
                    }
                }
            }
            else if(prob < exp(-difference / temperature)) {
                uphill++;
                delete tree;
                for(int i = 0; i < node_list.size(); i++) {
                    delete node_list[i];
                }
                tree = CopyTree(next, node_list);

            }
            else {
                reject++;
            }
        }
        
        if(best->width <= widthConstraint && best->height <= heightConstraint) {
            break;
        }

    }

    outputMatlab(argv[6], best_node_list, widthConstraint, heightConstraint);

    outputOut(best_node_list, argv[5]);

    best->cal_wirelength(net_list, best_node_list);
    
    ofstream file4;
	file4.open(argv[4]);

    END = clock();

    file4 << "Benchmark: " << argv[1] << endl;
    file4 << "ratio: " << argv[2] << endl;
    file4 << "whitespace: " << argv[3] << "%" << endl;
    file4 << "Runtime: " << (END - START) / CLOCKS_PER_SEC << "s" << endl;
    file4 << "Wirelength: " << best->wirelength << endl;
    file4 << "Chip_width: " << best->width << endl;
    file4 << "Chip_height: " << best->height << endl;


    return 0; 
}