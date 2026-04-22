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
};

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
                if (curr_Node->tokens.size() == 4){ // переполнение с детьми (бывает только от домино)!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    continue;
                }

                int direction = curr_Node->tokens.size(); // дефолт в самую правую позицию, иначе раньше
                for (int i = 0; i < curr_Node->tokens.size(); i++){
                    if (tok.key < curr_Node->tokens[i].key){
                        direction = i;
                    }
                }
                if (curr_Node->children[direction] != nullptr){
                    curr_Node = curr_Node->children[direction]; // перешли на ребёнка
                }
            }
            else{ // нет детей
                curr_Node->tokens.push_back(tok); // добавление токена в нужное место
                sort(curr_Node->tokens.begin(), curr_Node->tokens.end(), compareTokens);

                if (curr_Node->tokens.size() == 4){ // если переполнение
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

                    par->tokens.push_back(middle); // закинули в родителя токен
                    sort(par->tokens.begin(), par->tokens.end(), compareTokens);

                    curr_Node = par; // перешли на родителя
                }
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
