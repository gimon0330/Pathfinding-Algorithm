#include <stdio.h>
#include <time.h>   //time 함수
#include <stdlib.h>   //rand, srand 함수

#define TX 19   //맵 크기
#define TY 19

#define QueueSize 10000   //큐 크기

//-----------변수와 구조체------------
typedef struct {
    int max;
    int num;
    int front;
    int rear;
    int *que;
} IntQueue;   //큐 구조체
int ObsArr[TX+1][TY+1];   //맵
typedef struct {
    int Length;
    int MomLocationX;
    int MomLocationY;
} RouteArr;   //맵 Length 부모를 저장하는 구조체
RouteArr routearr[TX+1][TY+1];  
IntQueue Queue[4];   //열린 공간, 닫힌 공간 큐 생성
int StartX, StartY, TargetX, TargetY;   //시작 위치, 목표 위치
int PathX, PathY;   //길 위치
clock_t start, end;   //시간 측정 구조체
double res;   //걸린 시간을 저장할 변수
//-----------변수와 구조체------------

//-----------------큐-----------------
int MakingObs(int func, int n)   //장애물 생성 함수
{
    int i, j, ObsN=0, X, Y;   //ObsN: 장애물 개수, (X, Y): 장애물 위치
    
    switch(func) {   //0: 랜덤한 위치에 랜덤한 개수의 장애물을 설치. 1: 랜덤한 위치에 n개의 장애물을 설치. 2: 맵을 사용자에게 입력받음.
        case 0:
            ObsN=rand()%(TX*TY)/3 + (TX*TY)/3;   //(TX*TY)/3<=장애물 개수<=(TX*TY)*2/3
            break;
        case 1:
            ObsN=n;   //장애물 개수: n
            break;
        case 2:
            for(i=0; i<=TX; i++)
                for(j=0; j<=TY; j++)
                    scanf("%d", &ObsArr[i][j]);
            
            ObsN=0;
            for(i=0; i<TX; i++) {
                for(j=0; j<TY; j++) {
                    if(ObsArr[i][j]==1)
                        ObsN++;
                }
            }
            return ObsN;
    }
    
    for(i=0; i<ObsN;) {
        X=rand()%(TX+1);   //0<=X, Y<=TX+1, TY+1;
        Y=rand()%(TY+1);
        if(ObsArr[X][Y]==0) {
            ObsArr[X][Y]=1;
            i++;
        }
    }
    ObsArr[TargetX][TargetY]=2;   //목표 지점
    ObsArr[StartX][StartY]=0;
    
    return ObsN;   //장애물 개수 반환
}
int Initialize(IntQueue *q, int max)   //큐 생성 함수
{
    q->num=q->front=q->rear=0;
    if((q->que = (int *)calloc(max, sizeof(int))) == NULL) {
        q->max=0;
        return -1;
    }
    q->max=max;
    return 0;
}
int Enque(IntQueue *q, int x)   //값 대입 함수
{
    if(q->num>=q->max)
        return -1;
    else {
        q->num=(q->num+1)%QueueSize;
        q->que[q->rear++] = x;
        if(q->rear == q->max)
            q->rear=0;
        return 0;
    }
}
void Terminate(IntQueue *q)   //큐 삭제 함수
{
    if(q->que != NULL)
        free(q->que);
    q->max=q->num=q->front=q->rear=0;
}
void MakingQueue()   //열린 공간 닫힌 공간 생성 함수
{
    Initialize(&Queue[0], QueueSize);   //열린 공간 생성, x좌표
    Initialize(&Queue[1], QueueSize);   //열린 공간 생성, y좌표
    Initialize(&Queue[2], QueueSize);   //닫힌 공간 생성, x좌표
    Initialize(&Queue[3], QueueSize);   //닫힌 공간 생성, y좌표
}
//-----------------큐-----------------

//-------------다익스트라 알고리즘-------------
void PrintMap()   //맵 출력 함수
{
    int i, j;
    
    for(i=0; i<=TX; i++, puts("")) {  //맵 출력 □■○
        for(j=0; j<=TY; j++) {
            switch(ObsArr[i][j]) {
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
int IsInOpen(int x, int y)   //열린 공간에 이미 있는지 판단하는 함수. 있으면 1, 없으면 0을 반환
{
    int i;
    
    for(i=0; i<Queue[0].num; i++) {
        if(Queue[0].que[i]==x && Queue[1].que[i]==y)
            return 1;
    }
    return 0;
}
int IsInClose(int x, int y)   //닫히 공간에 이미 있는지 판단하는 함수. 있으면 1, 없으면 0을 반환
{
    int i;
    
    for(i=0; i<Queue[2].num; i++) {
        if(Queue[2].que[i]==x && Queue[3].que[i]==y)
            return 1;
    }
    return 0;
}
int IsBlocked(int x, int y)   //탐색 하려는 곳이 장애물이 있는 곳인지 판단하는 함수. 있으면 1, 없으면 0을 반환
{
    if(ObsArr[x][y]==1)   //있으면 1을 반환
        return 1;
    else
        return 0;   //없으면 0을 반환
}
int Lengthnum(int i, int j, int x, int y)   //Length값 계산에서 반환하는 함수
{
    if(i*j==0)
        return routearr[x][y].Length+10;   //상하좌우는 10반환
    else
        return routearr[x][y].Length+14;   //대각선은 14반환
}
void SearchArea(int x, int y)   //가능한 위치 탐색 및 입력 함수
{
    int i, j;
    
    for(i=-1; i<2; i++) {   //한 칸 주위의 칸 모두 탐색
        for(j=-1; j<2; j++) {
            if((i!=0 || j!=0) && x+i>=0 && x+i<TX+1 && y+j>=0 && y+j<TY+1) {
                if(IsBlocked(x+i, y+j))
                    continue;
                else if(IsInClose(x+i, y+j))   //닫힌 공간에 이미 있는가?
                    continue;
                else if(IsInOpen(x+i, y+j)) {   //열린 공간에 이미 있는가?
                    int CpL = Lengthnum(i, j, x, y);   //비교 Length값
                    if(routearr[x+i][y+j].Length>CpL) {   //비교 Length값이 원래 Length값보다 더 작으면 수정
                        routearr[x+i][y+j].Length=CpL;   //Length값 저장
                        routearr[x+i][y+j].MomLocationX = x;   //부모 x값 저장
                        routearr[x+i][y+j].MomLocationY = y;   //부모 y값 저장
                    }
                }
                else {   //없는가?
                    Enque(&Queue[0], x+i);   //없으면 열린 공간에 추가
                    Enque(&Queue[1], y+j);
                    routearr[x+i][y+j].Length=Lengthnum(i, j, x, y);   //Length값 저장
                    routearr[x+i][y+j].MomLocationX = x;   //부모 x값 저장
                    routearr[x+i][y+j].MomLocationY = y;   //부모 y값 저장
                }
            }
        }
    }
}
int DikstraAlgorithm()
{
    int i;
    
    for(i=0; i<=Queue[0].num; i++) {
        if(i==Queue[0].num)   //탐색할 위치가 없으면 -1반환
            return -1;
        if(Queue[0].que[i]!=-1) {
            if(Queue[0].que[i]==TargetX && Queue[1].que[i]==TargetY) {   //목표 위치에 도달했을 경우 종료
                PathX=TargetX;   //길 저장
                PathY=TargetY;
                return 1;
            }
            SearchArea(Queue[0].que[i], Queue[1].que[i]);   //열린 공간에 있는 것 탐색
            Enque(&Queue[2], Queue[0].que[i]);   //닫힌 공간에 추가
            Enque(&Queue[3], Queue[1].que[i]);
            Queue[0].que[i]=-1;   //열린 공간에서 삭제
            Queue[0].que[i]=-1;
        }
    }
    return 0;
}
void StartDikstraAlgorithm()
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
    if(MakingObsFunc==1)
        scanf("%d", &ObsNum);
   
    start = clock();   //시작!
    ObsNum = MakingObs(MakingObsFunc, ObsNum);   //장애물 생성 및 개수 저장
    MakingQueue();   //열린 공간 닫힌 공간 생성
    Enque(&Queue[0], StartX);   //열린 공간에 시작 지점을 추가
    Enque(&Queue[1], StartY);
    
    IsBlock = DikstraAlgorithm();   //다익스트라 알고리즘 시작!
    
    if(IsBlock==-1)   //막혔으면 막혔다고 출력
        puts("Is Blocked");
    else {
        while(1) {
            PX=PathX, PY=PathY;   //임시 대입
            if(PathX==StartX && PathY==StartY)   //출발 위치로 되도라 오면 끝
                break;
            ObsArr[PathX][PathY]=3;   //길 표시
            PathX=routearr[PX][PY].MomLocationX;   //부모 위치로 다시 갱신
            PathY=routearr[PX][PY].MomLocationY;
        }
        ObsArr[StartX][StartY]=4;   //출발 지점은 4, 목표 지점은 5로 표시
        ObsArr[TargetX][TargetY]=5;
    }
    
    printf("%d\n", ObsNum);
    PrintMap();   //맵 출력
    
    for(i=0; i<4; i++)   //큐 동적 배열 해제
        Terminate(&Queue[i]);
    
    end = clock();   //종료!
    res = (double)(end-start)/CLOCKS_PER_SEC;   //CLOCKS_PER_SEC는 시간 단위로 상수
    printf("\n%lf초\n", res);
}
//-------------다익스트라 알고리즘-------------

int main()
{
    srand((int)time(NULL));   //난수 설정
    StartDikstraAlgorithm();   //다익스트라 알고리즘
    
    
    return 0;
}



