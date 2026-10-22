#include <iostream>
#include "avl-tree.hpp"
#include "rb-tree.hpp"

template <typename Tree>
void Execute(Tree& tree) {
    std::string comand;
    while(std::cin >> comand){
        if(comand == "END")
            break;

        else if(comand == "INSERT"){
            int value;
            std::cin >> value;
            if(tree.insert(value)) std::cout << "inserted\n\n";
            else std::cout << "already existis, insertion ignored\n\n";
        }
        else if(comand == "REMOVE"){
            int value;
            std::cin >> value;
            if(tree.remove(value)) std::cout << "removed\n\n";
            else std::cout << "does not exist, nothing removed\n\n";
        }
        else if(comand == "SEARCH"){
            int value;
            std::cin >> value;
            if(tree.search(value)) std::cout << "found\n\n";
            else std::cout << "not found\n\n";
        }
        else if(comand == "INORDER"){
            tree.inOrder();
        }
        else if(comand == "PRINT"){
            tree.print();
        }
        else if(comand == "HEIGHT"){
            std::cout << "Height: " << tree.treeHeight() << "\n\n";
        }
    }

}

int main(){
    std::string type;
    std::cout << "Chose the type of tree: avl / rb\n";
    std::getline(std::cin, type);

    if(type == "avl"){
        AVLTree tree;
        Execute(tree);
    }
    
    else if(type == "rb"){
        RedBlackTree tree;
        Execute(tree);
    }

    return 0;
}