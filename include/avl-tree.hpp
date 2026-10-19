#ifndef AVL_HPP
#define AVL_HPP

#include <string>

struct AVLNode {
    int key;
    int height;
    AVLNode* left;
    AVLNode* right;
};

class AVLTree {
private:
    AVLNode* root;

    static int height(const AVLNode* node);
    static int balanceFactor(const AVLNode* node);
    static void updateHeight(AVLNode* node);

    static AVLNode* rotateLeft(AVLNode* x);
    static AVLNode* rotateRight(AVLNode* y);
    static AVLNode* rebalance(AVLNode* node);
    static AVLNode* minimum(AVLNode* node);

    static AVLNode* insert(AVLNode* node, int key, bool& inserted);
    static AVLNode* remove(AVLNode* node, int key, bool& removed);

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


};

#endif