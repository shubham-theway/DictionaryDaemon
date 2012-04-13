#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define parent 1
#define child 0

#define For(i,n) for(i=0;i<n;i++)
#define max(a,b) a>b?a:b;
#define min(a,b) a<b?a:b;
#define MAX_LEN 50              //this is the maximum length our keyword may have

struct node
{
	char name;
	int number;
	int level;
	struct node* next[26];
	char *associatedNames;
};
struct node *newHash;
int uniqueNum=0;

struct msgText 
{
	long mType;
	char mText[200];
};


/*
 assumptions:
 small and caps dont matter


 /*
  preprocessing should include
  1. extractKeyword
  2. createNewHash

  runtime available methods:
  1. getHashValue
  */

 int getNextUniqueNum()
 {
	 return ++uniqueNum;
 }

 int absoluteCharVal(char charVal)
 {
	 char charNum;
	 if(charVal>96)
		 charNum=charVal-97;
	 else
		 charNum=charVal-65;
	 return charNum;
 }

 struct node* insertNewHashChar(struct node* tempNode, int charNum,int i)
 {
	 if(tempNode->next[charNum]==NULL)
	 {
		 tempNode->next[charNum]=(struct node*)malloc(sizeof(struct node));
	 }
	 tempNode->next[charNum]->name=65+charNum;
	 tempNode->next[charNum]->number=0;

	 tempNode->next[charNum]->level=i;
	 tempNode=tempNode->next[charNum];
	 return tempNode;
 }


 void extractKeyword(FILE *fp)
 {
	 struct node *tempNode=newHash;
	 char currentKeyword[MAX_LEN];
	 //char *currentKeyword=(char*)malloc(50);
	 int charNum;
	 int i,length;
	 int hashVal;
	 while(fscanf(fp,"%s",currentKeyword)>0)
	 {
		 fscanf(fp,"%d",&hashVal);
		 //hashVal=getNextUniqueNum();
		 tempNode=newHash;
		 length=strlen(currentKeyword);
		 // all except the last characters will be processed here
		 for(i=0;i<length-1;i++)
		 {
			 //extract below logic into this method createNewHash()
			 charNum=absoluteCharVal(currentKeyword[i]);
			 tempNode=insertNewHashChar(tempNode, charNum,i);
		 }
		 //last character
		 charNum=absoluteCharVal(currentKeyword[i]);
		 tempNode=insertNewHashChar(tempNode, charNum,i);
		 tempNode->number=hashVal;
	 }
 }

 // method also needs string length so that we should not get segmentation fault
 int find(struct node* dnode,char* str,int len)
 {
	 int i,num;
	 For(i,len)
	 {
		 num=absoluteCharVal(str[i]);
		 if(dnode==NULL || dnode->next[num]==NULL)
			 return 0;
		 if(dnode->next[num]->name!=num+'A')
			 return 0;
		 dnode=dnode->next[num];
	 }
	 return dnode->number;
 }

 // for testing purpose only
 void show(struct node* dnode)
 {
	 int i;
	 if(dnode==NULL)
		 return;
	 printf("%c %d\n",dnode->name,dnode->number);
	 For(i,26)
		 show(dnode->next[i]);
 }

 int main(int argc, char *argv[])
 {
	 char* tempArray,*query,*strHashVal,*notFound="Not Found";
	 int i,j,length,hashVal,tKey,msgLen,temp;
	 pid_t pid;
	 int waitStatus=1;
	 struct msgText msg;
	 struct msgText resp;
	 int serverMsgQId,clientMsgQId;
	 int rv;
	 char smsg[]="epsilon";
	 int slen=8;
	 struct node* tempNode;
	 FILE *fp;
	 key_t serverKey=1234,clientKey=4321;
	 if(argc==1)
	 {
		 printf("No argument supplied\n");
		 system("pause");
		 exit(1);
	 }

	 if ((fp = fopen(*++argv, "r")) == NULL)
	 {
		 printf("I see some error with the file %s\n", *argv);
		 return 1;
	 }
	 else
	 {
		 switch(pid = fork()) 
		 {
			 case child:
				 printf("Daemon: My PID is %d\n", getpid());
				 if ((serverKey = ftok("/tmp/10", 'B')) == -1) 
			 {  
				 perror("ftok");
				 exit(1);
			 }

				 if ((serverMsgQId = msgget(serverKey,0777 | IPC_CREAT)) == -1) 
			 {
				 printf("error code = %d\n",serverMsgQId);
				 perror("msgget");
				 exit(1);
			 }
				 newHash=(struct node*)malloc(sizeof(struct node));
				 extractKeyword(fp);
				 //show(newHash);
				 fclose(fp);
				 //printf("inf loop starts\n");
				 while(1)
			 	{
				For(i,100)
				msg.mText[i]='\0';
				 if (msgrcv(serverMsgQId, &msg, sizeof(struct msgText), 0, 0) == -1)
				 {
					 perror("msgrcv");
					 exit(1);
				 }

				 query=(char*)malloc(sizeof(char)*200);
				 tKey=j=0;
				 for(i=0;i<4;i++)
				 {
					 tKey+=msg.mText[i]-'0';
					 tKey*=10;
				 }
				 tKey/=10;
				 msgLen=strlen(msg.mText);
				 for(;i<msgLen;i++)
				 {
					 query[j++]=msg.mText[i];
				 }
				 query[j]='\0';

				 clientKey=tKey;
				 //printf("received client key = %d\n",clientKey);
				 if ((clientMsgQId = msgget(clientKey, 0777 )) == -1) 
				 {
					 printf("error code = %d\n",clientMsgQId);
					 perror("msgget");
					 exit(1);
				 }
				 //strcpy(query,msg.mText);
				 //printf(" query = %s\n",query);
				 hashVal=find(newHash,query,strlen(query));
				 if(hashVal)
				 {
					 strcpy(resp.mText,"found with key ");

					 
					 temp=hashVal;
					 tempArray=(char*)malloc(sizeof(char)*5);
					 i=0;
					 while(temp)
					 {
						 j=temp%10;
						 tempArray[i]=j+'0';
						 temp/=10;
						 i++;
					 }
					 strHashVal=(char*)malloc(sizeof(char)*i);
					 For(j,i)
						 strHashVal[j]='\0';
					 j=0;
					 while(--i>=0)
					 {
						 strHashVal[j++]=tempArray[i];
					 }
					 strHashVal[j]='\0';
					 strcat(resp.mText,strHashVal);
				 }
				 else
					 strcpy(resp.mText,notFound);
				 slen=strlen(resp.mText);
				 resp.mText[slen+1]='\0';
				 resp.mType=1;
				 if (resp.mText[slen-1] == '\n') resp.mText[slen-1] = '\0';
				//printf("server response %s ...\n",resp.mText);
				 if (msgsnd(clientMsgQId, &resp, strlen(resp.mText), 0) == -1)
				 {
					 perror("msgsnd");
					 printf("ERROR: %d\n", (errno));
				 }
			 }
				 
				 if (msgctl(serverMsgQId, IPC_RMID, NULL) == -1) 
			 {
				 perror("msgctl1");
				 exit(1);
			 }
				 if (msgctl(clientMsgQId, IPC_RMID, NULL) == -1) 
			 {
				 perror("msgctl2");
				 exit(1);
			 }
				 break;
			 case parent:
				 //printf("PARENT: This is the parent process!\n");
				 printf("PARENT: My PID is %d\n", getpid());
				 //wait(&waitStatus);
				 exit(0);
			 default:
				 perror("fork");  
				 exit(1);
		 }
		 newHash=(struct node*)malloc(sizeof(struct node));
		 extractKeyword(fp);
		 fclose(fp);
		 show(newHash);
		 query=(char*)malloc(sizeof(char)*200);
		 printf("enter the key you want to search\n");
		 scanf("%s",query);
		 hashVal=find(newHash,query,strlen(query));
		 if(hashVal)
			 printf("found with key %d\n",hashVal);
		 else
			 printf("key not found in hash table\n");
	 }
	 return 0;
 }


 /*
  DO NOT CHANGE THE MSGBUFFER STRUCTURE SIGNATURE
  had to spent one full day to debug the issue of first 4 byte loss, which happened bcos i changed it :(

invalid arg error occured bcos i didnt initialize resp.mType!!


be careful when converting from number to string
	while(--i>=0)
                                                                                                        */
 
