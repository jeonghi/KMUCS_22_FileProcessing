#include <iostream>
#include <stack>
#include <fstream>
#include <cstdio>
#include <cstring>
using namespace std;
#define MAX_SIZE 5

class BTNode
{
public:
    int n; // 키의 갯수
    int K[MAX_SIZE]; // 키 값 배열
    BTNode *P[MAX_SIZE]; // 서브트리를 가리키는 포인터 배열
};

BTNode* getBTNode();
bool insertBT(BTNode **T, int m, int newKey);
pair<bool, stack<BTNode*>*> searchPath(BTNode **T, int m, int key, stack<BTNode*>* stk);
void insertKey(BTNode **T, int m, BTNode *x, BTNode *y, int newKey);
pair<int, BTNode*> splitNode(BTNode **T, int m, BTNode *x, BTNode *y, int newKey);

bool deleteBT(BTNode **T, int m, int oldKey);
int bestSibling(BTNode **T, int m, BTNode *x, BTNode *y);
void redistributeKeys(BTNode **T, int m, BTNode *x, BTNode *y, int bestSibling);
void mergeNode(BTNode **T, int m, BTNode *x, BTNode *y, int bestSib);

void inorderBT(BTNode **T, int m);

int main(){
    int M[] = {3, 4};
    for(int m : M){
        ifstream in("/Users/jeonghi/CLionProjects/KMUCS_22_FileProcessing/btree.txt");
        if(in.fail()){
            perror("open file : btree.txt");
            exit(1);
        }
        BTNode* root = nullptr;
        while(!in.eof()){
            char cmd;
            int key;
            in >> cmd >> key;
            switch(cmd){
                case 'i':
                    if(!insertBT(&root, m, key)){
                        printf("%c %d : The key already exists\n",cmd, key);
                    }
                    break;
                case 'd':
                    if(!deleteBT(&root, m, key)){
                        printf("%c %d : The key does not exist\n",cmd, key);
                    }
                    break;
                default:
                    perror("command not found");
                    exit(1);
            }
            inorderBT(&root, m);
            cout << endl;
        }
        in.close();
    }
    return 0;
}

BTNode* getBTNode()
{
    BTNode *newNode = new BTNode();
    return newNode;
}

/**
- key에 대한 검색을 수행
- key 발견 여부와 방문한 정점 경로를 담은 stack을 리턴
- 주어진 stack이 null이 아닌 스택이 주어지면, 해당 스택에 경로를 저장
 */
pair<bool, stack<BTNode*>*> searchPath(BTNode **T, int m, int key, stack<BTNode*>* stk)
{
    if(stk == nullptr){
        stk = new stack<BTNode*>();
    }

    BTNode*x;
    x = *T;

    int i;
    do
    {
        i = 0;
        while (i < x->n && key > x->K[i]) {
            i++;
        }
        stk->push(x);

        if (i < x->n && key == x->K[i]) {
            // 삽입할 키를 발견함. 삽입 불가.
            return {true, stk};
        }
        x = (x->P[i]);
    } while (x != nullptr);

    return {false, stk};
}

/**
* newKey를 노드 x의 적당한 위치에 삽입
* y는 x에서 newKey위치 다음 포인터가 가리킬 노드 (자식 노드에서 split되어 새로 만들어진 노드)
 */
void insertKey(BTNode **T, int m, BTNode *x, BTNode *y, int newKey)
{
    // newKey 보다 큰 키들을, 오른쪽으로 한 칸씩 이동
    int i = x->n - 1;
    while (i >= 0 && newKey < x->K[i])
    {
        x->K[i + 1] = x->K[i];
        x->P[i + 2] = x->P[i + 1];
        i--;
    }
    // newKey를 삽입
    x->K[i + 1] = newKey;
    x->P[i + 2] = y;
    x->n++;
} // 성공

/**
* newKey를 x에 삽입 후 split을 수행
* y는 x에서 newKey위치 다음 포인터가 가리킬 노드 (자식 노드에서 split되어 새로 만들어진 노드)
* split후 부모노드에 삽입되어야할 키 값과 분리된 새로운 노드를 반환
*/
pair<int, BTNode*> splitNode(BTNode **T, int m, BTNode *x, BTNode *y, int newKey)
{
    BTNode *tempNode = getBTNode(); // 오버플로우를 위한 임시 노드 (x와 newKey를 저장)
    memcpy(tempNode, x, sizeof(BTNode));
    insertKey(T, m, tempNode, y, newKey);

    int centerKey = tempNode->K[tempNode->n / 2]; // 분할 기준인 중앙값

    // centerKey 이전 값을 노드 x로 복사

    x->n = 0;
    int i = 0;

    while (tempNode->K[i] < centerKey)
    {
        x->K[i] = tempNode->K[i];
        x->P[i] = tempNode->P[i];
        i++;
        (x->n)++;
    }
    x->P[i] = tempNode->P[i];

    // centerKey 이후 값을 노드 newNode로 복사

    BTNode *newNode = getBTNode();
    newNode->n = 0;

    i++;

    while (i < tempNode->n)
    {
        newNode->K[newNode->n] = tempNode->K[i];
        newNode->P[newNode->n] = tempNode->P[i];
        i++;
        (newNode->n)++;
    }
    newNode->P[newNode->n] = tempNode->P[i];

    delete tempNode;

    return {centerKey, newNode};
}

/*
 * oldKey를 x에서 제거 수행
 */
void deleteKey(BTNode **T, int m, BTNode *x, int oldKey)
{
    int i = 0;
    // oldKey의 위치 i를 탐색
    while (oldKey > x->K[i])
        i++;

    // oldKey 보다 큰 키들을 왼쪽으로 한 칸씩 이동
    while (i < x->n)
    {
        x->K[i] = x->K[i + 1];
        x->P[i + 1] = x->P[i + 2];
        i++;
    }
    (x->n)--;
}

/*
 * 노드x와 해당노드의 부모노드y가 주어졌을때, x의 best 형제노드를 반환
 */
int bestSibling(BTNode **T, int m, BTNode *x, BTNode *y)
{
    int i = 0;
    // y에서 x의 위치 i를 탐색
    while (y->P[i] != x)
        i++;

    // 바로 인접한 두 형제 중, 키의 개수가 많은 형제를 bestSibling으로 선택함.
    if (i == 0) return i + 1; // 왼쪽 형제가 없음
    else if (i == y->n) return i - 1; // 오른쪽 형제가 없음
    else if (y->P[i]->n >= y->P[i + 1]->n)
        return i - 1;
    return i + 1;
} // 완료


/*
 * 노드 x와 해당 노드의 부모 노드 y, 그리고 y에서의 best sibling의 인덱스
 * bestSibling이 주어졌을 때 x와 best sibling 노드간의 키 재분배 수행
 */
void redistributeKeys(BTNode **T, int m, BTNode *x, BTNode *y, int bestSibling)
{
    int i = 0;
    // y에서 x의 위치 i를 탐색
    while (y->P[i] != x){
        i++;
    }

    BTNode *bestNode = y->P[bestSibling];

    if (bestSibling < i) // bestSibling이 왼쪽 형제 노드
    {
        int lastKey = bestNode->K[bestNode->n - 1];
        insertKey(T, m, x, nullptr, y->K[i - 1]);
        x->P[1] = x->P[0];
        x->P[0] = bestNode->P[bestNode->n];
        bestNode->P[bestNode->n] = nullptr;
        deleteKey(T, m, bestNode, lastKey);
        y->K[i - 1] = lastKey;
    }
    else // bestSibling이 오른쪽 형제 노드
    {
        int firstKey = bestNode->K[0];
        insertKey(T, m, x, nullptr, y->K[i]);
        x->P[x->n] = bestNode->P[0];
        bestNode->P[0] = bestNode->P[1];
        deleteKey(T, m, bestNode, firstKey);
        y->K[i] = firstKey;
    }
}

/*
 * 노드 x와 해당 노드의 부모 노드 y, 그리고 y에서의 best sibling의 인덱스
 * bestSibling이 주어졌을 때 x와 best sibling 노드간의 합병 수행
 */
void mergeNode(BTNode **T, int m, BTNode *x, BTNode *y, int bestSib)
{
    int i = 0;
    // y에서 x의 위치 i를 탐색
    while (y->P[i] != x)
        i++;

    BTNode *bestNode = y->P[bestSib];
    // 왼쪽 형제 노드로의 병합만 고려할 수 있도록 swap
    if (bestSib > i)
    {
        int tmp = i;
        i = bestSib;
        bestSib = tmp;

        BTNode *tmpNode = x;
        x = bestNode;
        bestNode = tmpNode;
    }
    // 왼쪽 형제 노드와 병합
    bestNode->K[bestNode->n] = y->K[i - 1];
    bestNode->n++;
    int j = 0;
    while (j < x->n)
    {
        bestNode->K[bestNode->n] = x->K[j];
        bestNode->P[bestNode->n] = x->P[j];
        bestNode->n++;
        j++;
    }
    bestNode->P[bestNode->n] = x->P[x->n];
    deleteKey(T, m, y, y->K[i - 1]);
    delete x;
}

// B트리의 삽입 알고리즘
bool insertBT(BTNode **T, int m, int newKey)
{
    // 루트 노드를 생성
    if (*T == nullptr)
    {
        *T = new BTNode;
        (*T)->K[0] = newKey;
        (*T)->n = 1;
        return true;
    }
    bool found; stack<BTNode*>* stk;

    // newKey를 삽입할 노드의 경로를 탐색하며, 스택에 경로 저장
    tie(found, stk) = searchPath(T, m, newKey, nullptr);

    // 이미 존재하는 키라면
    if (found)
    {
        // 삽입을 수행할 수 없으므로 삽입 알고리즘 실패
        return false;
    }

    // newKey가 없으므로, T에 삽입 (이제 x는 null)
    bool finished = false;
    BTNode *x; // 삽입을 수행할 노드를 담을 변수
    x = stk->top(); // 스택에서 pop해서 x에 할당
    stk->pop();
    BTNode *y = nullptr; // 새로 분할된 노드를 담을 변수

    do
    {
        if (x->n < m - 1) // Overflow 발생 여부 검사
        {
            // Overflow 발생 안함. newKey를 노드 x의 올바른 위치에 삽입
            insertKey(T, m, x, y, newKey);
            finished = true;
        }
        else
        { // Overflow 발생
            BTNode *newNode;
            // x를 newKey을 기준으로 분할, 분할된 노드 반환
            tie(newKey, newNode) = splitNode(T, m, x, y, newKey);
            y = newNode;
            if (!stk->empty())
            {
                x = stk->top();
                stk->pop();
            }
            else
            {
                // 트리의 레벨이 하나 증가
                *T = getBTNode();
                (*T)->n = 1;
                (*T)->K[0] = newKey;
                (*T)->P[0] = x;
                (*T)->P[1] = y;
                (*T)->n = 1;
                finished = true;
            }
        }
    } while (!finished);

    return true;
}

// B트리의 삭제 알고리즘
bool deleteBT(BTNode **T, int m, int oldKey)
{
    bool found; stack<BTNode*>* stk;

    // oldKey가 있는 노드의 경로를 탐색하며, 스택에 경로 저장
    tie(found, stk) = searchPath(T, m, oldKey, nullptr);

    if (!found) return false; // oldKey를 발견 못함. 삭제 불가.

    BTNode *x = stk->top();
    stk->pop();
    BTNode *y = nullptr;

    bool isLeaf = true; // oldKey가 내부 노드에 있는지 단말 노드에 있는 여부

    for (int i = 0; i < x->n; i++)
    {
        if (x->P[i] != nullptr)
        {
            isLeaf = false; // oldKey를 내부 노드에서 발견
            break;
        }
    }

    if (!isLeaf) // oldKey를 내부노드에서 발견했다면
    {
        BTNode *internalNode = x;
        int i; // index of oldKey in internalNode;

        for (i = 0; i < x->n; i++)
        {
            if (x->K[i] == oldKey)
                break;
        }

        stk->push(x);

        bool found2;

        // 후행키의 위치 탐색 (Ki와 일치하는 키는 찾지 못하나, 노드 경로는 검색)
        tie(found2, stk) = searchPath(&(x->P[i + 1]), m, x->K[i], stk);

        // 후행키와 oldKey를 교환함.
        x = stk->top(); // x는 후행키가 있는 단말노드
        stk->pop();
        int temp = internalNode->K[i];
        internalNode->K[i] = x->K[0];
        x->K[0] = temp; // 이제 x.K[0]이 oldKey임.
    }

    bool finished = false;
    deleteKey(T, m, x, oldKey); // 노드 x에서 oldKey를 삭제

    if (!stk->empty()){
        y = stk->top(); // 노드y는 x의부모노드
        stk->pop();
    }

    do
    {
        if (*T == x || x->n >= (m - 1) / 2) // underflow 발생하지 않음
        {
            finished = true;
        }
        else // underflow 발생
        {
            // 키 재분배 또는 노드 합병을 위한 형제 노드를 결정
            int bestSib = bestSibling(T, m, x, y);

            if (y->P[bestSib]->n > (m - 1) / 2) // bestSibling에서 재분배
            {
                redistributeKeys(T, m, x, y, bestSib);
                finished = true;
            }
            else // bestSibling과 노드 합병
            {
                mergeNode(T, m, x, y, bestSib);
                x = y;
                if (!stk->empty()){
                    y = stk->top();
                    stk->pop();
                }
                else{
                    finished = true;

                }
            }
        }
    } while (!finished);

    if (y != nullptr && y->n == 0) { // y에키가없음. (비어있음)
        (*T) = y->P[0];
    }

    return true;
}

// 출력을 위한 Btree의 순회 알고리즘
void inorderBT(BTNode **T, int m)
{
    BTNode* curr = *T;
    /* base case */
    if (curr == nullptr)
        return;

    /* recursive step */
    for (int i = 0; i < curr->n; i++)
    {
        inorderBT(&(curr->P[i]), m);
        cout << curr->K[i] << " ";
    }
    inorderBT((&curr->P[curr->n]), m);
}