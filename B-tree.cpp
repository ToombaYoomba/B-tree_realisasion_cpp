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

    bool operator==(const Token& other) const {
        return key == other.key;
    }
};

bool compareTokens(const Token& a, const Token& b) {
    return a.key < b.key;
}

class Node{ // нода вектор токенов не жёсткий, вектор детей длиной 4 с nullptr
public:
    Node* parent;
    vector<Token> tokens;
    vector<Node*> children;

    Node(): parent(nullptr), tokens(), children() {}

    Node(vector<Token> t): parent(nullptr), tokens(t), children() {} 

    bool Allnullptr(){
        if (children.size() == 0){
            return true;
        }
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
private:
    pair<Node*, Token> findNodeAndToken(Node* curr_node, string str){
        if (curr_node == nullptr) {
            return {nullptr, Token()};
        }

        for (int i = 0; i < curr_node->tokens.size(); i++){
            if (curr_node->tokens[i].key == str){
                return {curr_node, curr_node->tokens[i]};
            }
        }

        for (int i = 0; i < curr_node->children.size(); i++){
            auto [found_node, found_token] = findNodeAndToken(curr_node->children[i], str);
            if (found_node != nullptr){
                return {found_node, found_token};
            }
        }

        return {nullptr, Token()};
    }

    pair<Node*, Token> findNodeAndToken(string str){
        return findNodeAndToken(root, str);
    }

    pair<Node*, Token> findMinSuccessor(Node* del_node, Token del_token){
        auto it = find(del_node->tokens.begin(), del_node->tokens.end(), del_token);
        int index = distance(del_node->tokens.begin(), it);

        Node* curr_node = del_node->children[index+1];

        while(1){
            if (!curr_node->Allnullptr()){
                curr_node = curr_node->children[0];
            }
            else{
                return {curr_node, curr_node->tokens[0]};
            }
        }
    }
public:
    Node* root;

    BinaryTree(Node* starting_node) : root(starting_node) {}

    Token findToken(Node* curr_node, string str){
        if (curr_node == nullptr) {
            return Token();
        }

        for (int i = 0; i < curr_node->tokens.size(); i++){
            if (curr_node->tokens[i].key == str){
                return curr_node->tokens[i];
            }
        }

        for (int i = 0; i < curr_node->children.size(); i++){
            Token curr_result = findToken(curr_node->children[i], str);
            if (curr_result.key != ""){
                return curr_result;
            }
        }

        return Token();
    }

    Token findToken(string str){
        return findToken(root, str);
    }



    void delToken(string str){
        auto [del_node, del_token] = findNodeAndToken(str);
        auto it = find(del_node->tokens.begin(), del_node->tokens.end(), del_token);
        int index = distance(del_node->tokens.begin(), it);

        if (del_node->Allnullptr()){ // нет детей
            if (del_node->tokens.size() > 1){ // не единственный в узле
                auto it = find(del_node->tokens.begin(), del_node->tokens.end(), del_token);
                del_node->tokens.erase(it);
                return;
            }
            // единственный в корне
            if (del_node == root){
                root = new Node();
                delete del_node;
                return;
            }
            // лист не в корне
            
            auto it = find(del_node->parent->children.begin(), del_node->parent->children.end(), del_node);
            int direction = distance(del_node->parent->children.begin(), it);
            // cout << direction << endl;
            if (direction - 1 > 0){ // если берём у левого
                if (del_node->parent->children[direction-1]->tokens.size() > 1){
                    Token new_tok = del_node->parent->tokens[direction-1];
                    Token new_replace_tok = del_node->parent->children[direction-1]->tokens[-1];
                    del_node->parent->tokens[direction-1] = new_replace_tok;
                    del_node->tokens[0] = new_tok;
                    del_node->parent->children[direction-1]->tokens.pop_back();
                    return;
                }
            }
            if (direction + 1 < del_node->parent->children.size()){ // если берём у правого
                if (del_node->parent->children[direction+1]->tokens.size() > 1){
                    Token new_tok = del_node->parent->tokens[direction];
                    Token new_replace_tok = del_node->parent->children[direction+1]->tokens[0];
                    del_node->parent->tokens[direction] = new_replace_tok;
                    del_node->tokens[0] = new_tok;
                    del_node->parent->children[direction+1]->tokens.erase(del_node->parent->children[direction+1]->tokens.begin());
                    return;
                }
            }
            
            // если слияние
            if (direction - 1 > 0){ // если сливаем с левым
                Token new_add_tok = del_node->parent->tokens[direction-1];
                del_node->parent->children[direction-1]->tokens.push_back(new_add_tok);
                auto it = find(del_node->parent->tokens.begin(), del_node->parent->tokens.end(), new_add_tok);
                del_node->tokens.erase(it);
                del_node->parent->children.erase(del_node->parent->children.begin() + direction);
                delete del_node;
                return;
            }
            if (direction + 1 < del_node->parent->children.size()){ // если сливаем с правым
                Token new_add_tok = del_node->parent->tokens[direction];
                del_node->parent->children[direction+1]->tokens.insert(del_node->parent->children[direction+1]->tokens.begin(), new_add_tok);
                auto it = find(del_node->parent->tokens.begin(), del_node->parent->tokens.end(), new_add_tok);
                del_node->parent->tokens.erase(it);
                del_node->parent->children.erase(del_node->parent->children.begin() + direction);
                delete del_node;
                return;
            }
        }
        // есть дети
        auto [successor_node, successor_token] = findMinSuccessor(del_node, del_token);
        delToken(successor_token.key);
        del_node->tokens[index] = successor_token;
        return;
    }

    void addToken(Token tok){ // передаю токен а не значение (будет в мейне)
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
                // cout << direction << endl;
                if (curr_Node->children[direction] != nullptr){
                    curr_Node = curr_Node->children[direction]; // перешли на ребёнка
                }
                // else{
                //     cout << "ERROR" << endl;
                //     return;
                // }
                continue;
            }
            else{ // нет детей
                curr_Node->tokens.push_back(tok); // добавление токена в нужное место
                sort(curr_Node->tokens.begin(), curr_Node->tokens.end(), compareTokens);
                break;
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
            else{ // переполнение без детей (у листов)
                vector<Token> left(curr_Node->tokens.begin(), curr_Node->tokens.begin() + 2);
                vector<Token> right = {curr_Node->tokens[3]};
                Token middle = curr_Node->tokens[2];

                if (root == curr_Node){ // переполнение в корне
                    Node* new_root = new Node(vector<Token> {middle});
                    root = new_root;
                    root->children[0] = new Node(left);
                    root->children[0]->parent = root;
                    root->children[1] = new Node(right);
                    root->children[1]->parent = root;
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

int main(){
    Token a = Token("a", 1);
    Token b = Token("b", 2);
    Token c = Token("c", 3);
    Token d = Token("d", 4);
    Token e = Token("e", 5);
    Token f = Token("f", 6);
    Token g = Token("g", 7);
    Token h = Token("h", 8);

    Node* f_node = new Node(vector<Token> {b, g});
    BinaryTree tree = BinaryTree(f_node);

    Node* one_node = new Node(vector<Token> {a});
    Node* two_node = new Node(vector<Token> {c, d, e});
    Node* three_node = new Node(vector<Token> {h});

    tree.root->addChild(one_node);
    tree.root->addChild(two_node);
    tree.root->addChild(three_node);

    tree.printTree(tree.root);

    string need = "e";

    Token found = tree.findToken(need);
    cout << found.value << endl;

    tree.addToken(f);

    tree.printTree(tree.root);

    tree.delToken("d");

    tree.printTree(tree.root);

    tree.delToken("b");

    tree.printTree(tree.root);


    // cout << tree.root->tokens[0].key << endl;

    // // if (tree.root->Allnullptr()){
    // //     cout << 1 << endl;
    // // }
    // // else{
    // //     cout << 0 << endl;
    // // }

    // tree.addToken(b);

    // cout << tree.root->tokens[1].key << endl;
    // tree.printTree(tree.root);

    // tree.addToken(c);
    // tree.addToken(d);

    // tree.printTree(tree.root);






    return 0;
}
