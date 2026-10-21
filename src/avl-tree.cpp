#include "avl-tree.hpp"

#include <algorithm>
#include <iostream>

AVLTree::AVLTree() : root(nullptr) {}

AVLTree::~AVLTree() {
    destroy(root);
}

void AVLTree::destroy(AVLNode* node) {
    if (node == nullptr) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

// ------------------------------------------------------------ auxiliares

int AVLTree::height(const AVLNode* node) {
    return node ? node->height : 0;
}

int AVLTree::balanceFactor(const AVLNode* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

void AVLTree::updateHeight(AVLNode* node) {
    contador.up_height++;
    node->height = 1 + std::max(height(node->left), height(node->right));
}

AVLNode* AVLTree::minimum(AVLNode* node) {
    while (node->left != nullptr) node = node->left;
    return node;
}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    contador.rot++;
    AVLNode* y = x->right;
    x->right = y->left;
    y->left = x;
    updateHeight(x);
    updateHeight(y);
    return y;
}

AVLNode* AVLTree::rotateRight(AVLNode* y) {
    contador.rot++;
    AVLNode* x = y->left;
    y->left = x->right;
    x->right = y;
    updateHeight(y);
    updateHeight(x);
    return x;
}

AVLNode* AVLTree::rebalance(AVLNode* node) {
    updateHeight(node);
    int bf = balanceFactor(node);

    if (bf > 1) {                                   // pesada à esquerda
        if (balanceFactor(node->left) < 0)          // caso esquerda-direita
            node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (bf < -1) {                                  // pesada à direita
        if (balanceFactor(node->right) > 0)         // caso direita-esquerda
            node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

// ---------------------------------------------------------------- busca

bool AVLTree::search(int key) const {
    const AVLNode* cur = root;
    while (cur != nullptr) {
        contador.compare++;
        if (key == cur->key) return true;
        cur = (key < cur->key) ? cur->left : cur->right;
    }
    return false;
}

// -------------------------------------------------------------- inserção

bool AVLTree::insert(int key) {
    bool inserted = false;
    root = insert(root, key, inserted);
    return inserted;
}

AVLNode* AVLTree::insert(AVLNode* node, int key, bool& inserted) {
    if (node == nullptr) {
        inserted = true;
        return new AVLNode{key, 1, nullptr, nullptr};
    }
    contador.compare++;
    if (key < node->key) node->left = insert(node->left, key, inserted);
    else if (key > node->key) node->right = insert(node->right, key, inserted);
    else return node;  // valor repetido

    return rebalance(node);
}

// --------------------------------------------------------------- remoção

bool AVLTree::remove(int key) {
    bool removed = false;
    root = remove(root, key, removed);
    return removed;
}

AVLNode* AVLTree::remove(AVLNode* node, int key, bool& removed) {
    if (node == nullptr) return nullptr;  // chave inexistente

    contador.compare++;
    if (key < node->key) {
        node->left = remove(node->left, key, removed);
    } else if (key > node->key) {
        node->right = remove(node->right, key, removed);
    } else {
        removed = true;
        if (node->left == nullptr || node->right == nullptr) {
            AVLNode* child = node->left ? node->left : node->right;
            delete node;
            return child;
        }
        AVLNode* successor = minimum(node->right);
        node->key = successor->key;
        bool ignored = false;
        node->right = remove(node->right, successor->key, ignored);
    }
    return rebalance(node);
}

// ------------------------------------------------------------- percurso

void AVLTree::inOrder() const {
    if (root == nullptr) {
        std::cout << "Árvore vazia.\n";
        return;
    }
    inOrder(root);
    std::cout << "\n";
}

void AVLTree::inOrder(const AVLNode* node) {
    if (node == nullptr) return;
    inOrder(node->left);
    std::cout << node->key << " ";
    inOrder(node->right);
}

// ------------------------------------------------------------ impressão

void AVLTree::print() const {
    if (root == nullptr) {
        std::cout << "Árvore vazia.\n";
        return;
    }
    print(root, "", "", true, true);
}

// Mostra o filho direito (D) primeiro e o esquerdo (E) depois.
void AVLTree::print(const AVLNode* node, const std::string& prefix, const char* label, bool isLast, bool isRoot) {
    if (node == nullptr) return;

    std::cout << prefix;
    if (!isRoot) std::cout << (isLast ? "└── " : "├── ") << label;
    std::cout << " " << node->key << " (fb=" << balanceFactor(node) << ")\n";

    std::string childPrefix = prefix;
    if (!isRoot) childPrefix += isLast ? "    " : "│   ";

    print(node->right, childPrefix, "D:", node->left == nullptr, false);
    print(node->left, childPrefix, "E:", true, false);
}

//-------------------------------------------------------------- altura

int AVLTree::treeHeight() const {
    return height(root);
}