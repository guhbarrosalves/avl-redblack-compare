#ifndef RED_BLACK_HPP
#define RED_BLACK_HPP

#include <string>

enum class Color { RED, BLACK };

struct RBNode {
    int key;
    Color color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;
};

class RedBlackTree {
public:
    RedBlackTree();
    ~RedBlackTree();
    RedBlackTree(const RedBlackTree&) = delete;
    RedBlackTree& operator=(const RedBlackTree&) = delete;

    bool insert(int key);        // false se a chave já existe
    bool remove(int key);        // false se a chave não existe
    bool search(int key) const;  // true se a chave existe
    void inOrder() const;        // imprime as chaves em ordem crescente
    void print() const;          // imprime a estrutura com cores ANSI

private:
    RBNode* root;
    RBNode* nil;  // sentinela (folha preta compartilhada)

    RBNode* findNode(int key) const;
    RBNode* minimum(RBNode* node) const;

    void rotateLeft(RBNode* x);
    void rotateRight(RBNode* x);
    void transplant(RBNode* u, RBNode* v);

    void fixInsert(RBNode* z);
    void fixRemove(RBNode* x);

    void destroy(RBNode* node);
    void inOrder(const RBNode* node) const;
    void print(const RBNode* node, const std::string& prefix, const char* label, bool isLast, bool isRoot) const;
};

#endif