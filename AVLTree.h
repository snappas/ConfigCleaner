
#ifndef AVLTREE_H
#define AVLTREE_H
#include <string>
template<class T>
struct AVLNode {
    T info;
    int bfactor; //balance factor
    AVLNode<T> *llink;
    AVLNode<T> *rlink;
};
template<class T>
class AVLTree
{
private:
    AVLNode<T>* root;
    char* name;
public:
    AVLTree();
    AVLTree(char* treename);
    void rotateToLeft(AVLNode<T>* &root);
    void rotateToRight(AVLNode<T>* &root);
    void balanceFromLeft(AVLNode<T>* &root);
    void balanceFromRight(AVLNode<T>* &root);
    void insertIntoAVL(AVLNode<T>* &root, AVLNode<T> *newNode, bool& isTaller);
    void insert(const T &newItem);

    bool search(const T& searchItem) const;
    char* getName();
    //void setName();
};

template<class T>
AVLTree<T>::AVLTree()
{
    root = NULL;
}
template<class T>
AVLTree<T>::AVLTree(char* treename)
{
    name = new char[strlen(treename)+1];
    strcpy(name,treename);
    root = NULL;
}
template<class T>
char* AVLTree<T>::getName(){
    return name;
}
template <class T>
void AVLTree<T>::rotateToLeft(AVLNode<T>* &root)
{
    AVLNode<T> *p; //pointer to the root of the
//right subtree of root
    if (root == NULL) {
        cerr << "Error in the tree" << endl;
    } else if (root->rlink == NULL)
        cerr << "Error in the tree:"
             <<" No right subtree to rotate." << endl;
    else {
        p = root->rlink;
        root->rlink = p->llink; //the left subtree of p becomes
//the right subtree of root
        p->llink = root;
        root = p; //make p the new root node
    }
}//rotateLeft

template <class T>
void AVLTree<T>::rotateToRight(AVLNode<T>* &root)
{
    AVLNode<T> *p; //pointer to the root of
//the left subtree of root
    if (root == NULL) {
        cerr << "Error in the tree" << endl;
    } else if (root->llink == NULL)
        cerr << "Error in the tree:"
             << " No left subtree to rotate." << endl;
    else {
        p = root->llink;
        root->llink = p->rlink; //the right subtree of p becomes
//the left subtree of root
        p->rlink = root;
        root = p; //make p the new root node
    }
}//end rotateRight

template <class T>
void AVLTree<T>::balanceFromLeft(AVLNode<T>* &root)
{
    AVLNode<T> *p;
    AVLNode<T> *w;
    p = root->llink; //p points to the left subtree of root
    switch (p->bfactor) {
    case -1:
        root->bfactor = 0;
        p->bfactor = 0;
        rotateToRight(root);
        break;
    case 0:
        cerr << "Error: Cannot balance from the left." << endl;
        break;
    case 1:
        w = p->rlink;
        switch (w->bfactor) { //adjust the balance factors
        case -1:
            root->bfactor = 1;
            p->bfactor = 0;
            break;
        case 0:
            root->bfactor = 0;
            p->bfactor = 0;
            break;
        case 1:
            root->bfactor = 0;
            p->bfactor = -1;
        }//end switch
        w->bfactor = 0;
        rotateToLeft(p);
        root->llink = p;
        rotateToRight(root);
    }//end switch;
}//end balanceFromLeft

template <class T>
void AVLTree<T>::balanceFromRight(AVLNode<T>* &root)
{
    AVLNode<T> *p;
    AVLNode<T> *w;
    p = root->rlink; //p points to the left subtree of root
    switch (p->bfactor) {
    case -1:
        w = p->llink;
        switch (w->bfactor) { //adjust the balance factors
        case -1:
            root->bfactor = 0;
            p->bfactor = 1;
            break;
        case 0:
            root->bfactor = 0;
            p->bfactor = 0;
            break;
        case 1:
            root->bfactor = -1;
            p->bfactor = 0;
        }//end switch
        w->bfactor = 0;
        rotateToRight(p);
        root->rlink = p;
        rotateToLeft(root);
        break;
    case 0:
        cerr << "Error: Cannot balance from the left." << endl;
        break;
    case 1:
        root->bfactor = 0;
        p->bfactor = 0;
        rotateToLeft(root);
    }//end switch;
}//end balanceFromRight

template <class T>
void AVLTree<T>::insertIntoAVL(AVLNode<T>* &root,
                               AVLNode<T> *newNode, bool& isTaller)
{
    if (root == NULL) {
        root = newNode;
        isTaller = true;
    } else if (root->info.compare(newNode->info)==0) {
		//cout << "DUP: " << root->info << endl;
        cerr << "No duplicates are allowed." << endl;
    } else if (root->info.compare(newNode->info)>0) //newItem goes in
//the left subtree
    {
        insertIntoAVL(root->llink, newNode, isTaller);
        if (isTaller) //after insertion, the subtree grew in height
            switch (root->bfactor) {
            case -1:
                balanceFromLeft(root);
                isTaller = false;
                break;
            case 0:
                root->bfactor = -1;
                isTaller = true;
                break;
            case 1:
                root->bfactor = 0;
                isTaller = false;
            }//end switch
    }//end if
    else {
        insertIntoAVL(root->rlink, newNode, isTaller);
        if (isTaller) //after insertion, the subtree grew in
//height
            switch (root->bfactor) {
            case -1:
                root->bfactor = 0;
                isTaller = false;
                break;
            case 0:
                root->bfactor = 1;
                isTaller = true;
                break;
            case 1:
                balanceFromRight(root);
                isTaller = false;
            }//end switch
    }//end else
}//insertIntoAVL

template <class T>
void AVLTree<T>::insert(const T &newItem)
{
    bool isTaller = false;
    AVLNode<T> *newNode;
    newNode = new AVLNode<T>;
    newNode->info = newItem;
    newNode->bfactor = 0;
    newNode->llink = NULL;
    newNode->rlink = NULL;
    insertIntoAVL(root, newNode, isTaller);
}


template <class T>
bool AVLTree<T>::search(const T& searchItem) const
{
    AVLNode<T> *current;
    bool found = false;
    if (root == NULL) {
        //cerr << "Cannot search the empty tree." << endl;
    } else {
        current = root;
        while (current != NULL && !found) {
            //cout << current->info << endl;
            if (current->info.compare(searchItem)==0) {
                found = true;
            } else if (current->info.compare(searchItem)>0) {
                current = current->llink;
            } else {
                current = current->rlink;
            }
        }//end while
    }//end else
    return found;
}//end search
#endif