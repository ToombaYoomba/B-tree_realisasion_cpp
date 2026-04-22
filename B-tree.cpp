#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class Token{ // токен строка - значение
public:
    string key;
    float value;

    Token(): key(""), value(0) {}

    Token(string k, float v): key(k), value(v) {}
};

bool compareTokens(const Token& a, const Token& b) {
    return a.key < b.key;
}

class Node{ // нода вектор токенов не жёсткий, вектор детей длиной 4 с nullptr
public:
    Node* parent;
    vector<Token> tokens;
    vector<Node*> children;

    Node(vector<Token> t): parent (nullptr), tokens(t), children(4, nullptr) {} 

    bool Allnullptr(){
        for (auto* ptr : children) {
            if (ptr != nullptr) {
                return false;
            }
        }
        return true;
    }

    void addChild(Node* child_node){
        this->children.push_back(child_node);
        child_node->parent = this;
    }
};

void rewireNode(Node* new_parent_Node, Node* new_child_Node, int direction){
    if (new_parent_Node == nullptr){
            new_child_Node->parent = nullptr;
            return;
    }
    if (new_child_Node == nullptr){
        return;
    }
    new_parent_Node->children[direction] = new_child_Node;
    new_child_Node->parent = new_parent_Node;
}

class BinaryTree { // по сути имеет root ноду и набор методов
public:
    Node* root;

    BinaryTree(Node* starting_node) : root(starting_node) {}

    void addNode(Token tok){ // передаю токен а не значение (будет в мейне)
        if (root == nullptr){ // если дерево пустое
            Node* nod = new Node(vector<Token> {tok});
            root = nod;
            return;
        }

        Node* curr_Node = root;
        while(1){
            if (!curr_Node->Allnullptr()){ // если есть дети
                int direction = curr_Node->tokens.size(); // дефолт в самую правую позицию, иначе раньше
                for (int i = 0; i < curr_Node->tokens.size(); i++){
                    if (tok.key < curr_Node->tokens[i].key){
                        direction = i;
                    }
                }
                if (curr_Node->children[direction] != nullptr){
                    curr_Node = curr_Node->children[direction]; // перешли на ребёнка
                }
                continue;
            }
            else{ // нет детей
                curr_Node->tokens.push_back(tok); // добавление токена в нужное место
                sort(curr_Node->tokens.begin(), curr_Node->tokens.end(), compareTokens);
            }

        }

        while (curr_Node->tokens.size() == 4){ // если переполнение
            if (!curr_Node->Allnullptr()){ // переполнение с детьми (бывает только от домино)
                vector<Token> left(curr_Node->tokens.begin(), curr_Node->tokens.begin() + 2);
                vector<Token> right = {curr_Node->tokens[3]};
                Token middle = curr_Node->tokens[2];
                
                Node* new_left = new Node(left);
                Node* new_right = new Node(right);

                if (root == curr_Node){ // если корневой
                    Node* new_root = new Node(vector<Token> {middle});
                    root = new_root;
                    vector<Node*> old_children = curr_Node->children;
                    curr_Node->children.clear();
                    root->addChild(new Node(left));
                    root->addChild(new Node(right));

                    root->children[0]->addChild(old_children[0]);
                    root->children[0]->addChild(old_children[1]);
                    root->children[0]->addChild(old_children[2]);

                    root->children[1]->addChild(old_children[3]);
                    root->children[1]->addChild(old_children[4]);

                    delete curr_Node;
                    return;
                }

                // закинули указатели родителю
                Node* par = curr_Node->parent;
                auto it = find(par->children.begin(), par->children.end(), curr_Node);
                par->children.erase(it);
                par->children.insert(it, {new_left, new_right});
                new_left->parent = par;
                new_right->parent = par;

                par->tokens.push_back(middle); // закинули в родителя токен
                sort(par->tokens.begin(), par->tokens.end(), compareTokens);

                curr_Node = par;
                continue;     
            }
            else{
                vector<Token> left(curr_Node->tokens.begin(), curr_Node->tokens.begin() + 2);
                vector<Token> right = {curr_Node->tokens[3]};
                Token middle = curr_Node->tokens[2];

                if (root == curr_Node){ // переполнение в корне
                    Node* new_root = new Node(vector<Token> {middle});
                    root = new_root;
                    root->children[0] = new Node(left);
                    root->children[1] = new Node(right);
                    delete curr_Node;
                    return;
                }

                Node* new_left = new Node(left);
                Node* new_right = new Node(right);

                // закинули указатели родителю
                Node* par = curr_Node->parent;
                auto it = find(par->children.begin(), par->children.end(), curr_Node);
                par->children.erase(it);
                par->children.insert(it, {new_left, new_right});
                new_left->parent = par;
                new_right->parent = par;

                par->tokens.push_back(middle); // закинули в родителя токен
                sort(par->tokens.begin(), par->tokens.end(), compareTokens);

                curr_Node = par; // перешли на родителя
                continue;
            }
        }
    }

    void printTree(Node* node, string prefix = "") {
        if (node != nullptr) {
            cout << prefix << "["; // текущие
            for (size_t i = 0; i < node->tokens.size(); i++) {
                cout << node->tokens[i].key;
                if (i < node->tokens.size() - 1) cout << ", ";
            }
            cout << "]" << endl;
            
            for (size_t i = 0; i < node->children.size(); i++) { // дети
                printTree(node->children[i], prefix + "    ");
            }
        }
    }

};
