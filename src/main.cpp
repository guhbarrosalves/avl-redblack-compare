#include <iostream>
#include "avl-tree.hpp"
#include "rb-tree.hpp"


void Help(){
    std::cout << "-h displays this menu\n";
    std::cout << "INSERT value - insert value into the tree\n";
    std::cout << "REMOVE value - remove value from the tree\n";
    std::cout << "SEARCH value - search value in the tree\n";
    std::cout << "INORDER - displays the keys in ascending order\n";
    std::cout << "PRINT - displays the tree\n";
    std::cout << "HEIGHT - displays the height of tree\n";
    std::cout << "END - ends the program\n";
}

template <typename Tree>
void Execute(Tree& tree) {

    
    Help();
    std::string comand;
    while(std::cin >> comand){
        if(comand == "END")
            break;

        else if(comand == "INSERT"){
            int value;
            std::cin >> value;
            if(tree.insert(value)) std::cout << "inserted\n";
            else std::cout << "already existis, insertion ignored\n";
        }
        else if(comand == "REMOVE"){
            int value;
            std::cin >> value;
            if(tree.remove(value)) std::cout << "removed\n";
            else std::cout << "does not exist, nothing removed\n";
        }
        else if(comand == "SEARCH"){
            int value;
            std::cin >> value;
            if(tree.search(value)) std::cout << "found\n";
            else std::cout << "not found\n";
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
            std::cout << "unknown command\n";
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