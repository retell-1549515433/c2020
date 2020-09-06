#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

struct node{
    int value;
    struct  node* next;
};
struct node *head=NULL;
void RList(struct node *now,struct node *before);
void showList();
struct node* findEnd();
int FNext(int target);
void IList(int target);

int main() 
{
	int x;
    int num;
    scanf("%d",&num);
	for(int i;i<=num;i++){
        scanf("%d",&x);
        IList(x);
    }
    SList();

    printf("1:find in \n");
    printf("%d",FNext(5));
    
    printf("2:find in\n");
    printf("%d\n",FNext(5));


    RList(head,NULL);
    printf("\nthe list after reversing is:\n");
    SList();
    return 0;
}
void reverseList(struct node *now,struct node *before){
    if(now->next==NULL){
        head=now;
        return;
    }
    reverseList(now->next,now);
    now->next->next=now;
    now->next=before;
}
void insertList(int value){
    struct node* now=findEnd(head);
    if(now==NULL){
        head=(struct node*)malloc(sizeof(struct node));
        head->value=value;
        head->next=NULL;
        return;
    }
    now->next=(struct node*)malloc(sizeof(struct node));
    now->next->value=value;
    now->next->next=NULL;
}
struct node* findEnd(){
    struct node * now=head;
    if(now==NULL)return NULL;
    while(now->next)now=now->next;
    return now;
}
void showList(){
    struct node* now =head;
    while(now){
        printf("%d ",now->value);
        now=now->next;
    }
}
int findNext(int target){
    if(head==NULL)return -1;
    static int ptr=0;
    struct node* now=head;
    for(int i=1;i<=ptr+1;i++)now=now->next;
    while(now){
        ptr++;
        if(now->value==target){
            return ptr;
        }
        now=now->next;
    }
    return -1;
}
