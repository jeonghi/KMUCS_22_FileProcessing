#include <iostream>
#include <stack>
#include <fstream>
#include <cstdio>
using namespace std;

#define MAX_SIZE 5
#define MAX_ST_SIZE 10000

class BTNode
{
public:
    int n;
    int K[MAX_SIZE];
    BTNode *P[MAX_SIZE];
};

BTNode* getBTNode(int newKey);

bool insertBT(BTNode **T, int m, int newKey);
bool searchPath(BTNode **T, int m, int key, BTNode *st[], int &sp);
void insertKey(BTNode **T, int m, BTNode **x, BTNode *y, int newKey);
BTNode *splitNode(BTNode **T, int m, BTNode **x, BTNode *y, int &newKey);
bool deleteBT(BTNode **T, int m, int oldKey);

void inorderBT(BTNode **T, int m);
int bestSibling(BTNode **T, int m, BTNode *x, BTNode *y);
void redistributeKeys(BTNode **T, int m, BTNode **x, BTNode **y, int bestSib);

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

/***
 *
 * @param newKey
 * @return
 */
BTNode* getBTNode(int newKey)
{
    BTNode *newNode = new BTNode;
    newNode->n = 1;
    newNode->K[0] = newKey;
    return newNode;
}

/***
 *
 * @param T
 * @param m
 * @param key
 * @param st
 * @param sp
 * @return
 */
bool searchPath(BTNode **T, int m, int key, BTNode *st[], int &sp)
{
    BTNode*x;
    x = *T;

    int i;
    do
    {
        i = 0;
        while (i < x->n && key > x->K[i])
            i++;

        //K[i-1] < key < K[i] 인 구간 찾기, 찾은 경로를 스택에 저장
        st[sp++] = x;

        //삽입 할 키를 발견함, 삽입 불가
        if (i < x->n && key == x->K[i])
            return true;
        x = (x->P[i]);
    } while (x != NULL);
    return false;
}

/**
 *
 * @param T
 * @param m
 * @param x
 * @param y
 * @param newKey
 */
void insertKey(BTNode **T, int m, BTNode **x, BTNode *y, int newKey)
{
    //newKey보다 큰 Key들을 오른쪽으로 한 칸씩 이동
    int i = (*x)->n - 1;
    while (i >= 0 && newKey < (*x)->K[i])
    {
        (*x)->K[i + 1] = (*x)->K[i];
        (*x)->P[i + 2] = (*x)->P[i + 1];
        i--;
    }

    //newKey 삽입
    (*x)->K[i + 1] = newKey;
    (*x)->P[i + 2] = y;
    (*x)->n = (*x)->n + 1;
}

/***
 *
 * @param T
 * @param m
 * @param x
 * @param y
 * @param newKey
 * @return
 */
BTNode *splitNode(BTNode **T, int m, BTNode **x, BTNode *y, int &newKey)
{
    //오버플로우를 위한 임시 노드, x노드의 메모리 복사
    BTNode *tempNode = new BTNode;
    memcpy(tempNode, *x, sizeof(BTNode));
    insertKey(T, m, &tempNode, y, newKey);

    int centerKey = tempNode->K[tempNode->n / 2]; //분할 기준인 중앙 값

    (*x)->n = 0;
    int i = 0;

    //centerKey보다 작은 값을 노드 x로 복사
    while (tempNode->K[i] < centerKey)
    {
        (*x)->K[i] = tempNode->K[i];
        (*x)->P[i] = tempNode->P[i];
        i++;
        (*x)->n = (*x)->n + 1;
    }
    (*x)->P[i] = tempNode->P[i];
    BTNode *newNode = new BTNode;
    newNode->n = 0;
    i++;

    //centerKey보다 큰 값을 노드 newNode로 복사
    while (i < tempNode->n)
    {
        newNode->K[newNode->n] = tempNode->K[i];
        newNode->P[newNode->n] = tempNode->P[i];
        i++;
        newNode->n = newNode->n + 1;
    }
    newNode->P[newNode->n] = tempNode->P[i];
    newKey = centerKey;
    delete tempNode;
    return newNode;
}

/***
 *
 * @param T
 * @param m
 * @param x
 * @param oldKey
 */
void deleteKey(BTNode **T, int m, BTNode **x, int oldKey)
{
    //oldKey의 위치 i를 탐색
    int i = 0;
    while (oldKey > (*x)->K[i])
        i++;

    //oldKey보다 큰 키들을 왼쪽으로 한 칸씩 이동
    while (i < (*x)->n)
    {
        (*x)->K[i] = (*x)->K[i + 1];
        (*x)->P[i + 1] = (*x)->P[i + 2];
        i++;
    }
    (*x)->n--;
}

/***
 *
 * @param T
 * @param m
 * @param x
 * @param y
 * @return
 */
int bestSibling(BTNode **T, int m, BTNode *x, BTNode *y)
{
    //y에서 x의 위치 i를 탐색
    int i = 0;
    while (y->P[i] != x)
        i++;

    //바로 인접한 두 형제 중, 키의 개수가 많은 형제를 bestSibling으로 선택
    if (i == 0) return i + 1;           //왼쪽 형제 x
    else if (i == y->n) return i - 1;   //오른쪽 형제 x
    else if (y->P[i]->n >= y->P[i + 1]->n)
        return i - 1;
    return i + 1;
}


/***
 *
 * @param T
 * @param m
 * @param x
 * @param y
 * @param bestSib
 */
void redistributeKeys(BTNode **T, int m, BTNode **x, BTNode **y, int bestSib)
{
    //y에서 x의 위치 i를 탐색
    int i = 0;
    while ((*y)->P[i] != (*x))
        i++;

    BTNode *bestNode = (*y)->P[bestSib];

    //bestSibling이 왼쪽 형제 노드
    if (bestSib < i)
    {
        int lastKey = bestNode->K[bestNode->n - 1];
        insertKey(T, m, x, NULL, (*y)->K[i - 1]);
        (*x)->P[1] = (*x)->P[0];
        (*x)->P[0] = bestNode->P[bestNode->n];
        bestNode->P[bestNode->n] = NULL;
        deleteKey(T, m, &bestNode, lastKey);
        (*y)->K[i - 1] = lastKey;
    }
        //bestSibling이 오른쪽 형제 노드
    else
    {
        int firstKey = bestNode->K[0];
        insertKey(T, m, x, NULL, (*y)->K[i]);
        (*x)->P[(*x)->n] = bestNode->P[0];
        bestNode->P[0] = bestNode->P[1];
        deleteKey(T, m, &bestNode, firstKey);
        (*y)->K[i] = firstKey;
    }
}

/***
 *
 * @param T
 * @param m
 * @param x
 * @param y
 * @param bestSib
 */
void mergeNode(BTNode **T, int m, BTNode **x, BTNode **y, int bestSib)
{
    //y에서 x의 위치 i를 탐색
    int i = 0;
    while ((*y)->P[i] != *x)
        i++;

    BTNode *bestNode = (*y)->P[bestSib];
    //왼쪽 형제 노드로의 병합만 고려하도록 swap
    if (bestSib > i)
    {
        int tmp = i;
        i = bestSib;
        bestSib = tmp;

        BTNode *tmpNode = *x;
        (*x) = bestNode;
        bestNode = tmpNode;
    }
    //왼쪽 형제 노드와 병합
    bestNode->K[bestNode->n] = (*y)->K[i - 1];
    bestNode->n++;
    int j = 0;
    while (j < (*x)->n)
    {
        bestNode->K[bestNode->n] = (*x)->K[j];
        bestNode->P[bestNode->n] = (*x)->P[j];
        bestNode->n++;
        j++;
    }
    bestNode->P[bestNode->n] = (*x)->P[(*x)->n];
    deleteKey(T, m, y, (*y)->K[i - 1]);
}

/***
 * T를 루트로하는 분기율 m의 B트리에 newKey를 삽입하는 연산 수행
 * @param T
 * @param m
 * @param newKey
 * @return
 */
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

    BTNode *st[MAX_ST_SIZE];
    int sp = 0;

    //newKey를 삽입할 노드의 경로를 탐색하며, 스택에 경로 저장
    bool found = searchPath(T, m, newKey, st, sp);
    if (found)
    {
        //이미 존재하는 키, 삽입 불가
        return false;
    }

    //newKey가 없으므로, T에 삽입
    bool finished = false;
    BTNode *x = st[--sp];
    BTNode *y = NULL; //새로 분할된 노드를 담을 변수;

    do
    {
        if (x->n < m - 1)     //Overflow 발생 여부 검사
        {
            //Overflow 발생 안함. newKey를 노드 x의 올바른 위치에 삽입
            insertKey(T, m, &x, y, newKey);
            finished = true;
        }
        else     //Overflow 발생
        {
            //x를 newKey를 기준으로 분할, 분할된 노드 반환
            BTNode *newNode = splitNode(T, m, &x, y, newKey);
            y = newNode;
            if (sp > 0)
            {
                x = st[--sp];
            }
            else   //트리의 레벨이 하나 증가
            {
                *T = getBTNode(newKey);
                (*T)->P[0] = x;
                (*T)->P[1] = y;
                (*T)->n = 1;
                finished = true;
            }
        }
    } while (!finished);

    return true;
}

/***
 *
 * @param T
 * @param m
 * @param oldKey
 * @return
 */
bool deleteBT(BTNode **T, int m, int oldKey)
{
    BTNode *st[MAX_ST_SIZE];
    int sp = 0;

    //oldKey가 있던 노드의 경로를 탐색하며, 스택에 경로 저장
    bool found = searchPath(T, m, oldKey, st, sp);
    if (!found)
    {
        //oldKey를 발견 못함, 삭제 불가
        return false;
    }

    //oldKey가 내부 노드에 존재하는지 여부 검사
    BTNode *x = st[--sp];
    BTNode *y = nullptr;
    bool isTerminalNode = true;
    for (int i = 0; i < x->n; i++)
    {
        if (x->P[i] != nullptr)
        {
            isTerminalNode = false;
            break;
        }
    }

    //oldKey가 내부 노드에서 발견
    if (!isTerminalNode)
    {
        BTNode *internalNode = x;
        int i;
        for (i = 0; i < x->n; i++)
        {
            if (x->K[i] == oldKey)
                break;
        }

        st[sp++] = x;

        //후행키의 위치 탐색 (K[i]와 일치하는 키는 찾지 못하나, 노드의 경로는 탐색)
        searchPath(&(x->P[i + 1]), m, x->K[i], st, sp);

        //후행키와 oldKey를 교환
        x = st[--sp];  //x는 후행키가 있는 단말 노드
        int temp = internalNode->K[i];
        internalNode->K[i] = x->K[0];
        x->K[0] = temp;   //x->K[0]이 oldKey
    }

    bool finished = false;

    //노드 x에서 oldKey를 삭제
    deleteKey(T, m, &x, oldKey);

    if (sp > 0)
        y = st[--sp]; //노드 y는 x의 부모 노드

    do
    {
        if (*T == x || x->n >= (m - 1) / 2)
        {
            //underflow 발생 x
            finished = true;
        }
        else
        {
            //underflow 발생
            //키 재분배 또는 노드 합병을 위한 형제 노드를 결정
            int bestSib = bestSibling(T, m, x, y);

            if (y->P[bestSib]->n > (m - 1) / 2)
            {
                //bestSibling에서 재분배
                redistributeKeys(T, m, &x, &y, bestSib);
                finished = true;
            }
            else
            {
                //bestSibling과 노드 합병
                mergeNode(T, m, &x, &y, bestSib);
                x = y;
                if (sp > 0)
                    y = st[--sp];
                else
                    finished = true;
            }
        }
    } while (!finished);

    //y에 키가 없음(비어 있음)
    if (y != nullptr && y->n == 0)
        (*T) = y->P[0];   //old root를 삭제, 트리 높이가 하나 줄어든다

    return true;
}

/***
 *
 * @param T 순회하려는 현재 노드
 * @param m 분기율
 */
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