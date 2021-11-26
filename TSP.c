#include <stdio.h>
#include <math.h> //수학 연산 헤더파일 (sqrt를 사용함)
#include <limits.h> //최솟값과 최댓값을 정의해주는 헤더파일 (정수형 최댓값 INT_MAX를 사용함)

#define NUMBER 5 //도시의 수를 정의함 (5를 다른 숫자로 바꾸고 도시의 자료값을 정의하면 사용가능)

typedef struct City{
    char* name;
    int x;
    int y;
}City; //도시이름, 위치값 선언

City card[NUMBER]; //도시의 수

int top=-1;

void bye(City element){
    card[++top] = element;
}

City hi(){
    return card[top--];
}

void show(){
    int i;
    for(i=0;i<=top;i++)
    {
        printf("%s->",card[i].name);
    }
}

City city[]={{"A",10,10},{"B",15,20},{"C",20,20},{"D",20,30},{"E",30,30},{"F",35,20}};

int visited[NUMBER]={0};
int min = INT_MAX; //최댓값
int totalCount = 0;

int getDistance(City a,City b)
{
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
}

void TSP(int start, City city[], int number, int sum, int now)
{
    visited[start]=1;
    int i,count =0;
    for(i=0;i<number;i++)
    {
        if(visited[i]==0)
        {
            count++;
            visited[i]=1;
            bye(city[i]);
            TSP(start, city, number, sum+getDistance(city[now],city[i]),i);
            visited[i]=0;
            hi();
        }
    }
    if(count==0)
    {
        printf("%s->",city[start].name);
        sum = sum + getDistance(city[now],city[start]);
        show();
        printf("%s 합계: %d\n",city[start].name,sum);
        if(min>sum)
        {
            min=sum;
        }
        totalCount++;
    }
}

int main()
{
    TSP(0,city,NUMBER,0,0);
    printf("전체 경로의 개수: %d\n", totalCount);
    printf("최소 거리: %d",min);
    return 0;
}
