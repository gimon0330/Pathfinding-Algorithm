#pragma warning (disable : 4996)
#include <stdio.h>
#include <time.h>   //time 함수
#include <stdlib.h>   //rand, srand 함수

#define TX 19   //맵 크기
#define TY 19

#define QueueSize 1000   //큐 크기

int N, M;

int map[TX][TY];
int visit[TX][TY];
int outmap[TX][TY];

//-----------변수와 구조체------------
typedef struct {
    int max;
    int num;
    int front;
    int rear;
    int* que;
} IntQueue;   //큐 구조체
int ObsArr[TX + 1][TY + 1];   //맵
typedef struct {
    int F;
    int G;
    int H;
    int MomLocationX;
    int MomLocationY;
} RouteArr;   //맵 F, G, H, 부모를 저장하는 구조체
RouteArr routearr[TX + 1][TY + 1];
IntQueue Queue[4];   //열린 공간, 닫힌 공간 큐 생성
int StartX, StartY, TargetX, TargetY;   //시작 위치, 목표 위치
int PathX, PathY;   //길 위치
//-----------변수와 구조체------------

//-----------------큐-----------------
int Initialize(IntQueue* q, int max)   //큐 생성 함수
{
    q->num = q->front = q->rear = 0;
    if ((q->que = (int *)calloc(max, sizeof(int))) == NULL) {
        q->max = 0;
        return -1;
    }
    q->max = max;
    return 0;
}
int Enque(IntQueue* q, int x)   //값 대입 함수
{
    if (q->num >= q->max)
        return -1;
    else {
        q->num = (q->num + 1) % QueueSize;
        q->que[q->rear++] = x;
        if (q->rear == q->max)
            q->rear = 0;
        return 0;
    }
}
void Terminate(IntQueue* q)   //큐 삭제 함수
{
    if (q->que != NULL)
        free(q->que);
    q->max = q->num = q->front = q->rear = 0;
}
void MakingQueue()   //열린 공간 닫힌 공간 생성 함수
{
    Initialize(&Queue[0], QueueSize);   //열린 공간 생성, x좌표
    Initialize(&Queue[1], QueueSize);   //열린 공간 생성, y좌표
    Initialize(&Queue[2], QueueSize);   //닫힌 공간 생성, x좌표
    Initialize(&Queue[3], QueueSize);   //닫힌 공간 생성, y좌표
}
//-----------------큐-----------------

//-------------A*알고리즘-------------
int myabs(int n)   //절댓값 반환 함수
{
    return n > 0 ? n : -n;
}
int MakingObs(int func, int n)   //장애물 생성 함수
{
    int i, j, ObsN, X, Y;   //ObsN: 장애물 개수, (X, Y): 장애물 위치

    switch (func) {   //0: 랜덤한 위치에 랜덤한 개수의 장애물을 설치. 1: 랜덤한 위치에 n개의 장애물을 설치. 2: 맵을 사용자에게 입력받음.
    case 0:
        ObsN = rand() % (TX * TY) / 3 + (TX * TY) / 3;   //(TX*TY)/3<=장애물 개수<=(TX*TY)*2/3
        break;
    case 1:
        ObsN = n;   //장애물 개수: n
        break;
    case 2:
        for (i = 0; i <= TX; i++)
            for (j = 0; j <= TY; j++)
                scanf("%d", &ObsArr[i][j]);

        ObsN = 0;
        for (i = 0; i < TX; i++) {
            for (j = 0; j < TY; j++) {
                if (ObsArr[i][j] == 1)
                    ObsN++;
            }
        }
        return ObsN;
    }

    for (i = 0; i < ObsN;) {
        X = rand() % (TX + 1);   //0<=X, Y<=TX+1, TY+1;
        Y = rand() % (TY + 1);
        if (ObsArr[X][Y] == 0) {
            ObsArr[X][Y] = 1;
            i++;
        }
    }
    ObsArr[TargetX][TargetY] = 2;   //목표 지점
    ObsArr[StartX][StartY] = 0;

    return ObsN;   //장애물 개수 반환
}
int IsInOpen(int x, int y)   //열린 공간에 이미 있는지 판단하는 함수. 있으면 1, 없으면 0을 반환
{
    int i;

    for (i = 0; i < Queue[0].num; i++) {
        if (Queue[0].que[i] == x && Queue[1].que[i] == y)
            return 1;
    }
    return 0;
}
int IsInClose(int x, int y)   //닫히 공간에 이미 있는지 판단하는 함수. 있으면 1, 없으면 0을 반환
{
    int i;

    for (i = 0; i < Queue[2].num; i++) {
        if (Queue[2].que[i] == x && Queue[3].que[i] == y)
            return 1;
    }
    return 0;
}
int IsBlocked(int x, int y)   //탐색 하려는 곳이 장애물이 있는 곳인지 판단하는 함수. 있으면 1, 없으면 0을 반환
{
    if (ObsArr[x][y] == 1)   //있으면 1을 반환
        return 1;
    else
        return 0;   //없으면 0을 반환
}
int Gnum(int i, int j, int x, int y)   //G값 계산에서 반환하는 함수
{
    if (i * j == 0)
        return routearr[x][y].G + 10;   //상하좌우는 10반환
    else
        return routearr[x][y].G + 14;   //대각선은 14반환
}
int Hnum(int x, int y)   //H값 계산에서 반환하는 함수
{
    int xlength, ylength;

    xlength = myabs(TargetX - x);   //x좌표계에서의 예상 길이
    ylength = myabs(TargetY - y);   //y좌표계에서의 예상 길이

    return (xlength + ylength) * 10;   //대각선은 무시하고 상하좌우로만 움직인다고 가정해서 10곲한 값 반환
}
void SearchArea(int x, int y)   //가능한 위치 탐색 및 입력 함수
{
    int i, j;

    for (i = -1; i < 2; i++) {   //한 칸 주위의 칸 모두 탐색
        for (j = -1; j < 2; j++) {
            if ((i != 0 || j != 0) && x + i >= 0 && x + i < TX + 1 && y + j >= 0 && y + j < TY + 1) {
                if (IsBlocked(x + i, y + j))
                    continue;
                else if (IsInClose(x + i, y + j))   //닫힌 공간에 이미 있는가?
                    continue;
                else if (IsInOpen(x + i, y + j)) {   //열린 공간에 이미 있는가?
                    int CpG = Gnum(i, j, x, y);   //비교 G값
                    if (routearr[x + i][y + j].G > CpG) {   //비교 G값이 원래 G값보다 더 작으면 수정
                        routearr[x + i][y + j].G = CpG;   //G값 저장
                        routearr[x + i][y + j].F = routearr[x + i][y + j].G + routearr[x + i][y + j].H;   //F값 저장
                        routearr[x + i][y + j].MomLocationX = x;   //부모 x값 저장
                        routearr[x + i][y + j].MomLocationY = y;   //부모 y값 저장
                    }
                }
                else {   //없는가?
                    Enque(&Queue[0], x + i);   //없으면 열린 공간에 추가
                    Enque(&Queue[1], y + j);
                    routearr[x + i][y + j].G = Gnum(i, j, x, y);   //G값 저장
                    routearr[x + i][y + j].H = Hnum(x + i, y + j);   //H값 저장
                    routearr[x + i][y + j].F = routearr[x + i][y + j].G + routearr[x + i][y + j].H;   //F값 저장
                    routearr[x + i][y + j].MomLocationX = x;   //부모 x값 저장
                    routearr[x + i][y + j].MomLocationY = y;   //부모 y값 저장
                }
            }
        }
    }
}
int PrioritySearchId()   //우선으로 탐색하는 위치 반환. 탐색할 위치가 없으면 -1반환
{
    int i, min = 99999, minid = -1;

    for (i = 0; i < Queue[0].num; i++) {
        if (Queue[0].que[i] == StartX && Queue[1].que[i] == StartY)   //시작 위치를 탐색할 경우 다시 시작
            continue;
        if (routearr[Queue[0].que[i]][Queue[1].que[i]].F < min && routearr[Queue[0].que[i]][Queue[1].que[i]].F != 0) {   //탐색하려는 곳의 예상 비용이 더 작을 경우
            min = routearr[Queue[0].que[i]][Queue[1].que[i]].F;   //최솟값과 index저장.
            minid = i;
        }
    }

    return minid;   //H값이 가장 작고 부모노드가 같은 위치를 반환
}
void PrintMap()   //맵 출력 함수
{
    int i, j;

    for (i = 0; i <= TX; i++, puts("")) {  //맵 출력 □■○
        for (j = 0; j <= TY; j++) {
            switch (ObsArr[i][j]) {
            case 0:
                printf("□ ");   //빈 공간
                break;
            case 1:
                printf("+ ");   //장애물
                break;
            case 2:
                printf("G ");   //목표 지점
                break;
            case 3:
                printf("■ ");   //길
                break;
            case 4:
                printf("# ");   //시작 지점
                break;
            case 5:
                printf("& ");   //목표 지점
                break;
            }
        }
    }
}
int AstarAlgorithm(int x, int y)   //A* 알고리즘
{
    int SearchId, flag, FindX, FindY;   //우선으로 탐색하는 위치 큐 id 저장, 목표 위치에 도달하면 1을 저장, 탐색할 좌표(X, Y)

    if (x == TargetX && y == TargetY) {   //목표 위치에 도달했을 경우 종료
        ObsArr[x][y] = 3;   //길은 3으로 표시
        PathX = TargetX;   //길 저장
        PathY = TargetY;
        return 1;
    }
    while (1) {
        flag = 0;   //flag 초기화
        SearchArea(x, y);   //열린 공간에 가능한 위치 추가

        SearchId = PrioritySearchId();   //우선으로 탐색할 큐 id 저장
        if (SearchId == -1)   //길이 없는가?
            return -1;

        FindX = Queue[0].que[SearchId];   //탐색 할 좌표 저장
        FindY = Queue[1].que[SearchId];
        Enque(&Queue[2], FindX);   //닫힌 공간에 추가
        Enque(&Queue[3], FindY);
        Queue[0].que[SearchId] = 0;   //열린 공간에서 삭제
        Queue[1].que[SearchId] = 0;

        flag = AstarAlgorithm(FindX, FindY);   //우선으로 탐색할 위치
        if (flag == 1)   //목표 위치에 도달했다면 종료
            return 1;
    }
}
void StartAstarAlgorithm()   //A* 알고리즘 시작!
{
    int i, IsBlock, ObsNum=0, PX, PY, MakingObsFunc;   //막혀있으면 -1, 길이 있으면 1이 저장됨, 장애물 개수, 임시 길 위치 저장
    /*
        장애물 설치 유형
        0: 랜덤한 개수로 랜덤하게 장애물 설치
        1: 주어진 개수로 랜덤하게 장애물 설치
        2: 사용자가 직접 입력
    */

    scanf("%d %d %d %d", &StartX, &StartY, &TargetX, &TargetY);   //시작위치와 목표 위치
    scanf("%d", &MakingObsFunc);
    if (MakingObsFunc == 1)
        scanf("%d", &ObsNum);

    ObsNum = MakingObs(MakingObsFunc, ObsNum);   //장애물 생성 및 개수 저장
    MakingQueue();   //열린 공간 닫힌 공간 생성
    Enque(&Queue[2], StartX);   //닫힌 공간에 시작 지점을 추가
    Enque(&Queue[3], StartY);

    IsBlock = AstarAlgorithm(StartX, StartY);   //A*알고리즘 시작!

    if (IsBlock == -1)  //막혔으면 막혔다고 출력
        puts("Is Blocked");
    else {
        while (1) {
            PX = PathX, PY = PathY;   //임시 대입
            if (PathX == StartX && PathY == StartY)   //출발 위치로 되도라 오면 끝
                break;
            ObsArr[PathX][PathY] = 3;   //길 표시
            PathX = routearr[PX][PY].MomLocationX;   //부모 위치로 다시 갱신
            PathY = routearr[PX][PY].MomLocationY;
        }
        ObsArr[StartX][StartY] = 4;   //출발 지점은 4, 목표 지점은 5로 표시
        ObsArr[TargetX][TargetY] = 5;
    }

    printf("%d\n", ObsNum);
    PrintMap();   //맵 출력

    for (i = 0; i < 4; i++)   //큐 동적 배열 해제
        Terminate(&Queue[i]);
}
//-------------A*알고리즘-------------




//-------------BFS Queue----------------
struct node {
    int x;
    int y;
};

struct node queue[10001];
int tail = 0;
int head = 0;
int max = 0;

int vectX[4] = { 0, 0, 1, -1 };
int vectY[4] = { 1, -1, 0, 0 };

struct node deque() {
    return queue[head++];
}

void enque(int x_, int y_) {
    struct node temp;
    temp.x = x_;
    temp.y = y_;
    queue[tail++] = temp;
}


//----------------BFS Algorithm-------------
void BFS() {
    int nextX, nextY;
    while (head != tail) { //큐가 비었을때까지 황인한다.

        // 다음 방문할 노드 : [nextX][nextY]
        struct node k = deque();

        for (int i = 0; i < 4; i++) {
            nextX = k.x + vectX[i];
            nextY = k.y + vectY[i];

            //방향값 확인
            if (nextX >= 1 && nextX <= M && nextY >= 1 && nextY <= N) {

                // [nextX][nextY]까지 가는 길이 있고 한번도 방문하지 않은 노드인 경우 => queue에 넣는다.
                if (map[nextX][nextY] == 0 && visit[nextX][nextY] == 0) {
                    //[x][y] 에서 [nextX][nextY]까지 가는데 걸리는 노드 개수
                    visit[nextX][nextY] = visit[k.x][k.y] + 1;
                    enque(nextX, nextY);
                }
            }
        }
    }
}


//-------------- Main ----------------
int main() {
    int algorithmMode = 0;
    printf("1: A* Algorithm\n2: BFS Algorithm\n입력 ==> ");
    scanf("%d", &algorithmMode);

    if (algorithmMode == 1) {
        srand(time(NULL));   //난수 설정
        StartAstarAlgorithm();   //A*알고리즘
    }

    else if (algorithmMode == 2) {
        scanf("%d %d", &M, &N);

        for (int i = 1; i <= M; i++) {
            for (int j = 1; j <= N; j++) {
                scanf("%d", &map[i][j]);
            }
        }

        visit[1][1] = 1;
        enque(1, 1);
        BFS();

        for (int i = 1; i <= M; i++) {
            for (int j = 1; j <= N; j++) {
                printf("%3d", visit[i][j]);
            }
            printf("\n");
        }
    }

    return 0;
}
