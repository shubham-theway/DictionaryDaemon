#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define For(i,n) for(i=0;i<n;i++)
#define max(a,b) a>b?a:b;
#define min(a,b) a<b?a:b;

struct msgbuf {
    long mtype;
    char mtext[200];
};



int main(int argc, char *argv[])
{
    struct msgbuf *buf,res;
    int len,i=1,temp,j,digit,l;
int serverMsgQId,clientMsgQId;
    key_t serverKey,clientKey;
	time_t seconds;
	char str[5],ch;
	seconds=time(NULL);

    if ((serverKey = ftok("/tmp/10", 'B')) == -1) {
        perror("ftok");
        exit(1);
    }
	buf=(struct msgbuf*)malloc (sizeof(struct msgbuf));

	temp=seconds%10000;
	while(temp<=999)
		temp=temp*101%10000;
	clientKey=temp;
	j=3;
	while(temp)
	{
		digit=temp%10;
		buf->mtext[j--]=digit+'0';
		temp/=10;
	}
	buf->mtext[4]='\0';
	
    /*if ((clientKey = ftok("~/20", 'B')) == -1) {
        perror("ftok");
        exit(1);
    }*/

    if ((serverMsgQId = msgget(serverKey, 0777 )) == -1) {
        perror("msgget1");
        exit(1);
    }

    if ((clientMsgQId = msgget(clientKey, 0777 | IPC_CREAT)) == -1) {
        perror("msgget2");
        exit(1);
    }
    
	if(argc<=1)
	{
		printf("no argument supplied\n");
		return 1;
	}
while(i<argc)
{
	
    buf->mtype = 1;
	strcat(buf->mtext,argv[i]);
	//scanf("%s",buf.mtext);
        len = strlen(buf->mtext);
        if (buf->mtext[len-1] == '\n') buf->mtext[len-1] = '\0';
        if (msgsnd(serverMsgQId, buf, sizeof(struct msgbuf), 0) == -1) 
            perror("msgsnd");
	//printf("sent\n");
	For(i,100)
	res.mtext[i]='\0';
	if (msgrcv(clientMsgQId, &res,200, 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
       printf("response: \"%s\" \n", res.mtext);
free(buf);
buf=(struct msgbuf*)malloc (sizeof(struct msgbuf));
	for(l=4;l<200;l++)
		buf->mtext[l]=res.mtext[l]='\0';
//printf("msg sent, enter another msg \n");
	i++;
	sleep(1);
 }

    if (msgctl(clientMsgQId, IPC_RMID, NULL) == -1) 
	{
        perror("msgctl");
        exit(1);
    }

    return 0;
}
