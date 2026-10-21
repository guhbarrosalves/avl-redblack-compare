#ifndef AVL_HPP
#define AVL_HPP

#include <string>

struct AVLNode {
    int key;
    int height;
    AVLNode* left;
    AVLNode* right;
};

struct AVL_cont {
    unsigned long long compare{};
    unsigned long long rot{};
    unsigned long long up_height{};

};

class AVLTree {
private:

    AVLNode* root;
    AVL_cont mutable contador{};

    static int height(const AVLNode* node);
    static int balanceFactor(const AVLNode* node);
    void updateHeight(AVLNode* node);

    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rebalance(AVLNode* node);
    static AVLNode* minimum(AVLNode* node);

    AVLNode* insert(AVLNode* node, int key, bool& inserted);
    AVLNode* remove(AVLNode* node, int key, bool& removed);

    static void destroy(AVLNode* node); 
    static void inOrder(const AVLNode* node);
    static void print(const AVLNode* node, const std::string& prefix, const char* label, bool isLast, bool isRoot);

public:

    AVLTree();
    ~AVLTree();
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    bool insert(int key);        // false se a chave já existe
    bool remove(int key);        // false se a chave não existe
    bool search(int key) const;  // true se a chave existe
    void inOrder() const;        // imprime as chaves em ordem crescente
    void print() const;          // imprime a estrutura (chave e fator de balanceamento)

    AVL_cont read_cont() const { return contador; }
    void zero_cont () { contador = {};}

    int treeHeight() const;


};

#endif