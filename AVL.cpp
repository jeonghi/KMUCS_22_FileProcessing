#include<iostream>
#include<fstream>
#include<cstdio>
#include<stack>
#include<algorithm>
using namespace std;
class Node{
public:
    int key; // 해당 노드가 가지는 키 값
    int height; // 해당 노드를 root로 하는 subtree의 높이
    int bf; // 해당 노드가 가지는 balance factor
    Node* left; // left subtree 의 root를 가리키는 pointer
    Node* right; // right subtree 의 root를 가리키는 pointer
    Node():left(nullptr), right(nullptr), bf(0){}
};
Node* getNode();
bool insertBST(Node** T, int newKey, stack<Node*>& stk);
bool deleteBST(Node** T, int deleteKey, stack<Node*>& stk);
void inorderBST(Node* T);
void updateHeightAndBalancingFactor(Node* q);
pair<Node*, Node*> searchBST(Node* T, int searchKey, stack<Node*>& stk);
int height(Node* T);
bool insertAVL(Node** T, int newKey);
bool deleteAVL(Node** T, int deleteKey);
void checkBalance(Node** T, int key, string& rotationType, Node** p, Node** q, stack<Node*>& stk);
void LL(Node **T, Node* p, Node* q);
void RR(Node **T, Node* p, Node* q);
void rotateTree(Node** T, const string& rotationType, Node* p, Node* q, stack<Node*>& stk);

int main(){
    Node* root = nullptr;
    ifstream in("./avl.txt");
    if(in.fail()){
        perror("open file : avl.txt");
        exit(1);
    }
    while(!in.eof()){
        char cmd;
        int key;
        in >> cmd >> key;
        switch(cmd){
            case 'i':
                if(insertAVL(&root, key)){
                    inorderBST(root);
                }else{
                    printf("%c %d : The key already exists",cmd, key);
                }
                break;
            case 'd':
                if(deleteAVL(&root, key)){
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
bool insertBST(Node** T, int newKey, stack<Node*>& stk){
    Node* p = *T;
    Node* q = nullptr;
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
    return true;
}


bool deleteBST(Node** T, int deleteKey, stack<Node*>& stk){
    Node* p; // 삭제할 노드
    Node* q; // 삭제할 노드의 부모 노드
    tie(q, p) = searchBST(*T, deleteKey, stk);

    // 삭제할 키가 없다면, 그냥 리턴한다.
    if(p == nullptr) return false;

    // 차수가 2인경우 차수가 1 또는 0인 경우로 만든다.
    if( p->left != nullptr && p->right != nullptr){
        stk.push(p);
        Node* tmp = p;
        if(p->left->height <= p->right->height){
            p = p->right;
            while(p->left != nullptr){
                stk.push(p);
                p = p ->left;
            }
        }else{
            p = p->left;
            while(p->right != nullptr){
                stk.push(p);
                p = p ->right;
            }
        }
        tmp->key = p->key;
        q = stk.top();
    }
    // p 의 차수가 0 또는 1인 경우
    // 트리에서 p를 지운다.
    if(p->left == nullptr && p->right == nullptr){
        if(q == nullptr) *T = nullptr;
        else if(q->left == p) q->left = nullptr;
        else q->right = nullptr;
    }else{
        if(p->left != nullptr){
            if(q == nullptr) *T = (*T)->left;
            else if(q->left == p) q->left = p->left;
            else q->right = p->left;
        }else{
            if(q == nullptr) *T = (*T)->right;
            else if(q->left == p) q->left = p->right;
            else q->right = p->right;
        }
    }

    delete p;

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
    printf("(%d, %d) ",T->key, T->bf);
    inorderBST(T->right);
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


void updateHeightAndBalancingFactor(Node* q){
    q->height = 1 + max(height(q->left), height(q->right));
    q->bf = height(q->left) - height(q->right);
}

bool insertAVL(Node** T, int newKey){
    stack<Node*> stk;
    /* 0) 루트가 null 일때
     * 루트 노드 생성한후, 삽입 알고리즘 종료.
     */
    if(*T == nullptr){
        Node* tmp = getNode();
        tmp->key = newKey;
        tmp->bf = 0;
        tmp->height = 1;
        *T = tmp;
        cout << "NO ";
        return true;
    }

    /* 1) 노드 삽입
     * insertBST를 적용하여 노드를 삽입함.
     */
    if(!insertBST(T, newKey, stk))
        return false;
    /* 2) 균형 검사
     * 삽입된 노드의 부모에서 루트까지의 균형 인수 계산 (balance factor)
     * 균형 검사
     */
    Node* p = nullptr;
    Node* q = nullptr;
    string rotationType;
    checkBalance(T, newKey, rotationType, &p, &q, stk);
    /* 3) 재균형 ( rebalancing )
     * 불균형 노드를 발견하면, 재균형 알고리즘(회전) 적용함.
     * 균형 인수 변경이 필요한 노드의 균형 인수를 다시 계산함.
     */
    rotateTree(T, rotationType, p, q, stk);
    /* insertAVL 알고리즘 종료*/
    // 높이와 균형인수 재계산
    while (!stk.empty())
    {
        p = stk.top();
        stk.pop();
        updateHeightAndBalancingFactor(p);
    }
    return true;
}
bool deleteAVL(Node** T, int deleteKey){
    stack<Node*> stk;
    Node* f = nullptr; // 균형이 깨진 노드의 부모 노드
    Node* x = nullptr; // 균형이 깨진 노드

    string rotationType;

    if(!deleteBST(T, deleteKey, stk)){
        return false;
    }
    checkBalance(T, deleteKey, rotationType, &x, &f, stk);
    rotateTree(T, rotationType, x, f, stk);
    // 높이와 균형인수 재계산
    while (!stk.empty())
    {
        x = stk.top();
        stk.pop();
        updateHeightAndBalancingFactor(x);
    }

    return true;
}
void checkBalance(Node** T, int key, string& rotationType, Node** p, Node** q, stack<Node*>& stk){
    Node* tmp;
    rotationType = "NO";
    while(!stk.empty()){
       tmp = stk.top();
       stk.pop();
       updateHeightAndBalancingFactor(tmp);
       if(abs(tmp->bf) > 1){
           *p = tmp;
           *q = stk.empty() ? nullptr : stk.top();
           break;
       }
    }
    if(*p == nullptr) return ;

    if((*p)->bf > 1){
        if((*p)->left->bf < 0) rotationType = "LR";
        else rotationType = "LL";
    }else{
        if((*p)->right->bf > 0) rotationType = "RL";
        else rotationType = "RR";
    }
}

void rotateTree(Node** T, const string& rotationType, Node* p, Node* q, stack<Node*>& stk) {
    if (rotationType == "LL") {
        stk.push(p->left);
        stk.push(p);
        LL(T, p, q);
    } else if (rotationType == "LR") {
        stk.push(p->left->right);
        stk.push(p->left);
        stk.push(p);
        RR(T, p->left, p);
        LL(T, p, q);
    } else if (rotationType == "RR") {
        stk.push(p->right);
        stk.push(p);
        RR(T, p, q);
    } else if (rotationType == "RL") {
        stk.push(p->right->left);
        stk.push(p->right);
        stk.push(p);
        LL(T, p->right, p);
        RR(T, p, q);
    }
    cout << rotationType << " ";
}

void LL(Node **T, Node* p, Node* q){
    // p : 균형이 깨진 노드
    // q : p 의 부모노드
    Node* left_child = p->left; // 균형이 깨진 노드의 왼쪽 자식 노드
    p->left = left_child->right;
    left_child->right = p;

    if(q == nullptr){
        *T = left_child;
    }else if(q->left == p){
        q->left = left_child;
    }else{
        q->right = left_child;
    }
}

void RR(Node **T, Node* p, Node* q){
    // p : 균형이 깨진 노드
    // q : p 의 부모노드
    Node* right_child = p->right; // 균형이 깨진 노드의 오른쪽 자식 노드
    p->right = right_child->left;
    right_child->left = p;

    if(q== nullptr){
        *T = right_child;
    }else if(q->left == p){
        q->left = right_child;
    }else{
        q->right = right_child;
    }
}