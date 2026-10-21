#include <iostream>
#include "avl-tree.hpp"
#include "rb-tree.hpp"


void Help(){
    std::cout << "-h displays this menu\n";
    std::cout << "INSERT value - insert value into the tree\n";
    std::cout << "REMOVE value - remove value from the tree\n";
    std::cout << "SEARCH value - search value in the tree\n";
    std::cout << "INORDER - \n";
    std::cout << "PRINT - displays the tree\n";
    std::cout << "END - ends the program\n";
    std::cout << "HEIGHT - displays the height of tree\n";
}

template <typename Tree>
void Execute(Tree& tree) {
    std::string comand;
    while(std::cin >> comand){
        if(comand == "END")
            break;

        else if(comand == "INSERT"){
            int value;
            std::cin >> value;
            tree.insert(value);
        }
        else if(comand == "REMOVE"){
            int value;
            std::cin >> value;
            tree.remove(value);
        }
        else if(comand == "SEARCH"){
            int value;
            std::cin >> value;
            tree.search(value);
        }
        else if(comand == "INORDER"){
            tree.inOrder();
        }
        else if(comand == "PRINT"){
            tree.print();
        }
        else if(comand == "HEIGHT"){
            std::cout << "Height: " << tree.treeHeight() << "\n";
        }
        else if(comand == "-h"){
            Help();
        }
        else{
            std::cout << "unknown comand\n";
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