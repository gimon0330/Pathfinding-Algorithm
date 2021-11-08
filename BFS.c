#include<stdio.h>
#define SZ 101
int N, M;
 
int map[SZ][SZ];
int visit[SZ][SZ];
int outmap[SZ][SZ];
 
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
 
int main() {
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
