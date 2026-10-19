#include "rb-tree.hpp"

#include <iostream>

namespace {
const char* RESET = "\033[0m";
const char* RED_STYLE = "\033[1;31m";          // texto vermelho em negrito
const char* BLACK_STYLE = "\033[1;30;47m";     // texto preto sobre fundo branco (visível em terminal escuro)
}

RedBlackTree::RedBlackTree() {
    nil = new RBNode{0, Color::BLACK, nullptr, nullptr, nullptr};
    nil->left = nil->right = nil->parent = nil;
    root = nil;
}

RedBlackTree::~RedBlackTree() {
    destroy(root);
    delete nil;
}

void RedBlackTree::destroy(RBNode* node) {
    if (node == nil) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

RBNode* RedBlackTree::findNode(int key) const {
    RBNode* cur = root;
    while (cur != nil) {
        if (key == cur->key) return cur;
        cur = (key < cur->key) ? cur->left : cur->right;
    }
    return nil;
}

RBNode* RedBlackTree::minimum(RBNode* node) const {
    while (node->left != nil) node = node->left;
    return node;
}

void RedBlackTree::rotateLeft(RBNode* x) {
    RBNode* y = x->right;
    x->right = y->left;
    if (y->left != nil) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == nil) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void RedBlackTree::rotateRight(RBNode* x) {
    RBNode* y = x->left;
    x->left = y->right;
    if (y->right != nil) y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == nil) root = y;
    else if (x == x->parent->right) x->parent->right = y;
    else x->parent->left = y;
    y->right = x;
    x->parent = y;
}

void RedBlackTree::transplant(RBNode* u, RBNode* v) {
    if (u->parent == nil) root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
}

// ---------------------------------------------------------------- busca

bool RedBlackTree::search(int key) const {
    return findNode(key) != nil;
}

// -------------------------------------------------------------- inserção

bool RedBlackTree::insert(int key) {
    RBNode* parent = nil;
    RBNode* cur = root;
    while (cur != nil) {
        parent = cur;
        if (key == cur->key) return false;  // valor repetido
        cur = (key < cur->key) ? cur->left : cur->right;
    }

    RBNode* z = new RBNode{key, Color::RED, nil, nil, parent};
    if (parent == nil) root = z;
    else if (key < parent->key) parent->left = z;
    else parent->right = z;

    fixInsert(z);
    return true;
}

void RedBlackTree::fixInsert(RBNode* z) {
    while (z->parent->color == Color::RED) {
        RBNode* gp = z->parent->parent;
        if (z->parent == gp->left) {
            RBNode* uncle = gp->right;
            if (uncle->color == Color::RED) {
                z->parent->color = Color::BLACK;
                uncle->color = Color::BLACK;
                gp->color = Color::RED;
                z = gp;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotateLeft(z);
                }
                z->parent->color = Color::BLACK;
                z->parent->parent->color = Color::RED;
                rotateRight(z->parent->parent);
            }
        } else {
            RBNode* uncle = gp->left;
            if (uncle->color == Color::RED) {
                z->parent->color = Color::BLACK;
                uncle->color = Color::BLACK;
                gp->color = Color::RED;
                z = gp;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotateRight(z);
                }
                z->parent->color = Color::BLACK;
                z->parent->parent->color = Color::RED;
                rotateLeft(z->parent->parent);
            }
        }
    }
    root->color = Color::BLACK;
}

// --------------------------------------------------------------- remoção

bool RedBlackTree::remove(int key) {
    RBNode* z = findNode(key);
    if (z == nil) return false;  // chave inexistente

    RBNode* y = z;
    Color yOriginalColor = y->color;
    RBNode* x;

    if (z->left == nil) {
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == nil) {
        x = z->left;
        transplant(z, z->left);
    } else {
        y = minimum(z->right);
        yOriginalColor = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    delete z;
    if (yOriginalColor == Color::BLACK) fixRemove(x);

    nil->parent = nil;  // restaura a sentinela
    return true;
}

void RedBlackTree::fixRemove(RBNode* x) {
    while (x != root && x->color == Color::BLACK) {
        if (x == x->parent->left) {
            RBNode* w = x->parent->right;
            if (w->color == Color::RED) {
                w->color = Color::BLACK;
                x->parent->color = Color::RED;
                rotateLeft(x->parent);
                w = x->parent->right;
            }
            if (w->left->color == Color::BLACK && w->right->color == Color::BLACK) {
                w->color = Color::RED;
                x = x->parent;
            } else {
                if (w->right->color == Color::BLACK) {
                    w->left->color = Color::BLACK;
                    w->color = Color::RED;
                    rotateRight(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = Color::BLACK;
                w->right->color = Color::BLACK;
                rotateLeft(x->parent);
                x = root;
            }
        } else {
            RBNode* w = x->parent->left;
            if (w->color == Color::RED) {
                w->color = Color::BLACK;
                x->parent->color = Color::RED;
                rotateRight(x->parent);
                w = x->parent->left;
            }
            if (w->right->color == Color::BLACK && w->left->color == Color::BLACK) {
                w->color = Color::RED;
                x = x->parent;
            } else {
                if (w->left->color == Color::BLACK) {
                    w->right->color = Color::BLACK;
                    w->color = Color::RED;
                    rotateLeft(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = Color::BLACK;
                w->left->color = Color::BLACK;
                rotateRight(x->parent);
                x = root;
            }
        }
    }
    x->color = Color::BLACK;
}

// ------------------------------------------------------------- percurso

void RedBlackTree::inOrder() const {
    if (root == nil) {
        std::cout << "Árvore vazia.\n";
        return;
    }
    inOrder(root);
    std::cout << "\n";
}

void RedBlackTree::inOrder(const RBNode* node) const {
    if (node == nil) return;
    inOrder(node->left);
    std::cout << node->key << " ";
    inOrder(node->right);
}

// ------------------------------------------------------------ impressão

void RedBlackTree::print() const {
    if (root == nil) {
        std::cout << "Árvore vazia.\n";
        return;
    }
    print(root, "", "", true, true);
}

// Mostra o filho direito (D) primeiro e o esquerdo (E) depois.
void RedBlackTree::print(const RBNode* node, const std::string& prefix, const char* label, bool isLast, bool isRoot) const {
    if (node == nil) return;

    std::cout << prefix;
    if (!isRoot) std::cout << (isLast ? "└── " : "├── ") << label;

    const bool red = (node->color == Color::RED);
    std::cout << (red ? RED_STYLE : BLACK_STYLE)
              << " " << node->key << (red ? " (R) " : " (B) ")
              << RESET << "\n";

    std::string childPrefix = prefix;
    if (!isRoot) childPrefix += isLast ? "    " : "│   ";

    print(node->right, childPrefix, "D:", node->left == nil, false);
    print(node->left, childPrefix, "E:", true, false);
}