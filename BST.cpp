#include<iostream>
#include<fstream>
#include<cstdio>
#include<stack>
using namespace std;

class Node{
public:
    int key;
    int height; // 노드를 루트로 하는 서브트리의 높이
    Node* left;
    Node* right;

    Node():left(nullptr), right(nullptr){}
};
Node* getNode();
bool insertBST(Node** T, int newKey);
bool deleteBST(Node** T, int deleteKey);
void inorderBST(Node* T);
void updateHeight(stack<Node*>& stk);
pair<Node*, Node*> maxNode(Node* T, stack<Node*>& stk);
pair<Node*, Node*> minNode(Node* T, stack<Node*>& stk);
pair<Node*, Node*> searchBST(Node* T, int searchKey, stack<Node*>& stk);
int noNodes(Node* T);
int height(Node* T);

int main(){
    Node* root = nullptr;
    ifstream in("./bst.txt");
    if(in.fail()){
        perror("open file : bst.txt");
        exit(1);
    }
    while(!in.eof()){
        char cmd;
        int key;
        in >> cmd >> key;
        switch(cmd){
            case 'i':
                if(insertBST(&root, key)){
                    inorderBST(root);
                }else{
                    printf("%c %d : The key already exists",cmd, key);
                }
                break;
            case 'd':
                if(deleteBST(&root, key)){
                    inorderBST(root);
                }else{
                    printf("%c %d : The key does not exist",cmd, key);
                }
                break;
            default:
                perror("command not found");
                exit(1);
        }
        cout << endl;
    }
    return 0;
}

Node* getNode(){
    return new Node();
}

bool insertBST(Node** T, int newKey){
    Node* p = *T;
    Node* q = nullptr;
    stack<Node*> stk;
    // 부모 노드를 스택에 저장하는 동안 새로운 키를 삽입할 위치를 찾는다.
    while(p != nullptr){
        if(newKey == (p)->key) return false;
        q = p;
        stk.push(q);
        if(newKey < (p)->key) p = (p)->left;
        else p = (p)->right;
    }
    // 새로운 노드를 생성한다.
    Node* newNode = getNode();
    newNode -> key = newKey;

    if(*T == nullptr){
        (*T) = newNode;
    }else if(newKey < (q)->key){
        (q)->left = newNode;
    }else{
        (q)->right = newNode;
    }

    // 삽입한 노드 조상들의 높이를 업데이트한다.
    updateHeight(stk);
    return true;
}


bool deleteBST(Node** T, int deleteKey){
    Node* deleteNode; // 삭제할 노드
    Node* parentOfDeleteNode; // 삭제할 노드의 부모 노드
    stack<Node*> stk; // 높이를 업데이트 하기 위한 임시 스택
    pair<Node*, Node*> searchResult = searchBST(*T, deleteKey, stk);

    parentOfDeleteNode = searchResult.first;
    deleteNode = searchResult.second;

    /* base case */
    if(deleteNode == nullptr) return false;
    // 삭제하려는 노드에 자식이 있는지, 있다면 어느 쪽에 있는지 찾아낸 다음, 삭제를 진행한다.

    // 자식이 없는 경우
        //// 삭제하려는 노드와 부모의 연결을 끊는다 부모가 삭제하려는 노드와 연결 방향에 대한 자식 포인터를 null로 만든다
        //// 노드를 삭제한다.
    if(deleteNode->left == nullptr && deleteNode->right == nullptr) {
        if (parentOfDeleteNode == nullptr) (*T) = nullptr; // 삭제하려는 노드가 root 노드인 경우 루트노드를 null로 만든다
        else if (parentOfDeleteNode->left == deleteNode) parentOfDeleteNode->left = nullptr; // 부모노드의 왼쪽에 있었다면 부모노드의 왼쪽 방향에 대한 자식포인터를 null로 만든다.
        else parentOfDeleteNode->right = nullptr;
        delete deleteNode;
        return true;
    }// 삭제하려는 노드의 차수가 0 일때
    /* recursive step */
    // 삭제하려는 노드가 차수가 2인 경우
    ///// 대체해야할 노드를 찾는다.
    else if(deleteNode->left != nullptr && deleteNode->right != nullptr){
        stk.push(deleteNode);
        Node* swapNode;
        Node* parentOfSwapNode;
        bool flag = true; // left 일때 true
        pair<Node*, Node*> result;
        if(deleteNode->left->height < deleteNode->right->height){ // 오른쪽 서브트리의 높이가 더 큰 경우
            result = minNode(deleteNode->right, stk); // 오른쪽 서브트리에서 min 값을 찾는다
            flag = false;
        }else if(deleteNode->left->height > deleteNode->right->height){
            result = maxNode(deleteNode->left, stk);
        }else{
            if(noNodes(deleteNode->left) >= noNodes(deleteNode->right)){
                result = maxNode(deleteNode->left, stk);
            }else{
                result = minNode(deleteNode->right, stk);
                flag = false;
            }
        }
        parentOfSwapNode = result.first;
        swapNode = result.second;
        //값을 덮어쓰기 한다.
        deleteNode->key = swapNode->key;
        if(flag) deleteBST(&(deleteNode->left), swapNode->key);
        else deleteBST(&(deleteNode->right), swapNode->key);
    }
    else{ // 차수가 1일때
        if(deleteNode->left != nullptr){
            if(parentOfDeleteNode == nullptr) *T = deleteNode->left;
            else if(parentOfDeleteNode->left == deleteNode) parentOfDeleteNode->left = deleteNode->left;
            else parentOfDeleteNode->right = deleteNode->left;
        }else{
            if(parentOfDeleteNode == nullptr) *T = deleteNode->right;
            else if(parentOfDeleteNode->left == deleteNode) parentOfDeleteNode->left = deleteNode->right;
            else parentOfDeleteNode->right = deleteNode->right;
        }
        delete deleteNode;
        return true;
    }
    updateHeight(stk);
    return true;
}

int height(Node* T){
    if(T==nullptr){
        return -1;
    }
    return 1 + max(height(T->left), height(T->right));
}

void inorderBST(Node* T){
    // left - root - right
    if(T == nullptr){
        return ;
    }
    inorderBST(T->left);
//    printf("{key: %d, height: %d}", T->key, T->height);
    cout << T->key << " ";
    inorderBST(T->right);
}

pair<Node*, Node*> maxNode(Node* T, stack<Node*>& stk){
    Node* q = nullptr; // parent node;
    Node* p = T; // 탐색 포인터
    while(p->right != nullptr){
        stk.push(p);
        q = p;
        p = p->right;
    }
    return {q, p};
}

pair<Node*, Node*> minNode(Node* T, stack<Node*>& stk){
    Node* q = nullptr;
    Node* p = T;
    while(p->left != nullptr){
        stk.push(p);
        q = p;
        p = p->left;
    }
    return {q, p};
}

pair<Node*, Node*> searchBST(Node* T, int searchKey, stack<Node*>& stk){
    Node* q = nullptr; // parent Node;
    Node* p = T; // search Node;
    while(p != nullptr){
        if(p->key == searchKey){
            break;
        }
        q = p;
        stk.push(q);
        if(searchKey < p->key) p = p->left;
        else p = p->right;
    }
    return {q, p};

}

/* subtree 의 노드의 갯수 */
int noNodes(Node* T){
    if(T == nullptr){
        return 0;
    }
    return noNodes(T->right) + noNodes(T->left) + 1;
}

void updateHeight(stack<Node*>& stk){
    Node* q;
    while(!stk.empty()){
        q = stk.top();
        stk.pop();
        q->height = 1 + max(height(q->left), height(q->right));
    }
}

