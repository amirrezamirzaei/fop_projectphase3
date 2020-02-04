#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#define MAX 80
#define PORT 12345
#define SA struct sockaddr
typedef struct{
struct cAMIR *next;
char type[100];
char content[100];
}cAMIR;
cAMIR *pmembersid,*pmemberauthtoken,*pmemberactivechannel,*pmemberrefreshid;
char input[10000],authtoken[33];
void additemtoobject(cAMIR **head,char *a,char *b);
void printunformatted(cAMIR *x);
char* getitem(cAMIR *x,char *y);
cAMIR *deleteitem(cAMIR *head,char *y);
int server_socket, client_socket;
void sendjson(char type[],char content[]);
void listenserver();
void registeruser();
void authtokengenerator();
void loginuser();
void logout();
void createchannel();
void channelmembers();
void joinchannel();
void leavechannel();
void sendmessages();
void channelmembersecondversion(cAMIR *x,char *channelname);
void refresh();
void searchmember();
char* appendtojson(char *a,char *sender,char *message,char *filename);
int main()
{
    mkdir("user");
    mkdir("channel");
    pmembersid=NULL;
    pmemberauthtoken=NULL;
    pmemberactivechannel=NULL;
    pmemberrefreshid=NULL;
     while(1)
    {
        listenserver();
        memset(input, 0, sizeof(input));
        recv(client_socket, input, sizeof(input), 0);
        printf("FROM CLIENT:%s\n",input);
        char tmp1[100],tmp2[100];
    sscanf(input,"%s %s",tmp1,tmp2);
    if(strcmp(tmp1,"register")==0)
    {
        registeruser();
        continue;
    }
      if(strcmp(tmp1,"login")==0)
    {
        loginuser();
        continue;
    }
        if  (strcmp(tmp1,"create")==0)
    {
        createchannel();
        continue;
    }
    if(strcmp(tmp1,"logout")==0)
    {
        logout();
        continue;
    }
     if(strcmp(tmp1,"join")==0)
    {
        joinchannel();
        continue;
    }
     if(strcmp(tmp1,"leave")==0)
    {
        leavechannel();
        continue;
    }
      if(strcmp(tmp1,"send")==0)
    {
        sendmessages();
        continue;
    }
     if(strcmp(tmp1,"refresh")==0)
    {
        refresh();
        continue;
    }
      if(strcmp(tmp1,"channel")==0)
    {
        channelmembers();
        continue;
    }
     if(strcmp(tmp1,"searchmember")==0)
    {
        searchmember();
        continue;
    }
    }
    return 0;
}

void listenserver() //copy shode az kelas kargah shabake
{
    closesocket(client_socket);
    closesocket(server_socket);
    struct sockaddr_in server, client;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d", err);
        return;
    }
    // Create and verify socket
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d", WSAGetLastError() );
    else
        printf("Socket successfully created.");

    // Assign IP and port
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);
    // Bind newly created socket to given IP and verify
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed.");
        exit(0);
    }
    else
        printf("Socket successfully bound.");

    // Now server is ready to listen and verify
    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed.");
        exit(0);
    }
    else
        printf("Server listening.");

    // Accept the data packet from client and verify
    int len = sizeof(client);
    client_socket = accept(server_socket, (SA *)&client, &len);
    if (client_socket < 0)
    {
        printf("Server accceptance failed.");
        exit(0);
    }
    else
        printf("Server acccepted the client.\n");
}
void sendjson(char type[],char content[])
{
    char output[500];
    strcpy(output,"{\"type\":\"");
    strcat(output,type);
    strcat(output,"\",\"content\",\"");
    strcat(output,content);
    strcat(output,"\"}");
    send(client_socket, output, strlen(output), 0);
    printf("\nTO CLIENT:%s",output);
}
void registeruser()
{
 char tmp1[100],tmp2[100],tmp3[110];
  sscanf(input,"%*s %s %s",tmp1,tmp2);
  tmp1[strlen(tmp1)-1]='\0';
  FILE *tmpuser;
  strcpy(tmp3,"user\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  tmpuser=fopen(tmp3,"r");
  if(tmpuser!=NULL)
  {
      sendjson("Error","");
      fclose(tmpuser);
      return;
  }
  if(tmpuser==NULL)
  {
  FILE *user;
  strcpy(tmp3,"user\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  user=fopen(tmp3,"w");
  fprintf(user,"%s\n%s",tmp1,tmp2);
  fclose(user);
  }
  sendjson("Successful","");
  additemtoobject(&pmembersid,tmp1,"0");

  fclose(tmpuser);
}
void authtokengenerator()
{
    for(int i=0;i<32;i++)
    {
        int a=rand()%3;
        int c;
        if(a==0)
        {
            c=rand()%26+65;
            authtoken[i]=c;
        }
         if(a==1)
        {
            c=rand()%26+97;
            authtoken[i]=c;
        }
        if(a==2)
        {
            c=rand()%10+48;
            authtoken[i]=c;
        }
    }
}
void loginuser()
{
  char tmp1[100],tmp2[100],tmp3[110];
  FILE *tmpuser;
  sscanf(input,"%*s %s %s",tmp1,tmp2);
  tmp1[strlen(tmp1)-1]='\0';
  strcpy(tmp3,"user\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  tmpuser=fopen(tmp3,"r+");
  if(tmpuser==NULL)
  {
      sendjson("Error","");
      fclose(tmpuser);
      return;
  }
  char password[100];
  fscanf(tmpuser,"%*s %s",password);
  if(strcmp(password,tmp2)!=0)
  {
      sendjson("Error","wrong password");
      fclose(tmpuser);
      return;
  }
  printf("%s",tmp1);
   char *status=getitem(pmembersid,tmp1);
   if(status==NULL)
   {
        additemtoobject(&pmembersid,tmp1,"1");
   }
   else
    if(strcmp(status,"1")==0)
   {
      sendjson("Error","user already signed in");
      return;
   }
     pmembersid=deleteitem(pmembersid,tmp1);
    additemtoobject(&pmembersid,tmp1,"1");
    authtokengenerator();
    authtoken[32]='\0';
    sendjson("AuthToken",authtoken);
   additemtoobject(&pmemberauthtoken,authtoken,tmp1);
   printunformatted(pmemberauthtoken);
}
void logout()
{
   char tmp1[32];
   int i;
  sscanf(input,"%*s %s",tmp1);
  char *username=getitem(pmemberauthtoken,tmp1);
  if(username==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
  printf("\n%s\n ",username);
  sendjson("Successful","");
  pmemberauthtoken=deleteitem(pmemberauthtoken,tmp1);
  pmembersid=deleteitem(pmembersid,username);
  additemtoobject(&pmembersid,username,"0");
  printunformatted(pmembersid);
  return;
}
void createchannel()
{
   char tmp1[100],tmp2[100],tmp3[110];
  sscanf(input,"%*s %*s %s %s",tmp1,tmp2);
 char *username=getitem(pmemberauthtoken,tmp2);
  if(username==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
  printf("\n%s\n ",username);
  tmp1[strlen(tmp1)-1]='\0';
  FILE *channel;
  strcpy(tmp3,"channel\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  channel=fopen(tmp3,"r");
  if(channel!=NULL)
  {
      sendjson("Error","channel with this name already exist");
      fclose(channel);
      return;
  }
  if(channel==NULL)
  {
  strcpy(tmp3,"channel\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  channel=fopen(tmp3,"w");
  char gar[100],gar1[100];
  strcpy(gar,username);
  strcat(gar," created channel");
    strcpy(gar1,username);
    strcat(gar1," joined channel");
    char tmppp[300];
    strcpy(tmppp,"{\"messages\":[{\"sender\":\"server\",\"content\":\"");
    strcat(tmppp,gar);
    strcat(tmppp,"\"},{\"sender\":\"server\",\"content\":\"");
    strcat(tmppp,gar1);
    strcat(tmppp,"\"}]}");
     printf("\n%s",tmppp);
     fprintf(channel,"%s",tmppp);
     fclose(channel);
     sendjson("Succesful","");
     pmemberactivechannel=deleteitem(pmemberactivechannel,username);
     additemtoobject(&pmemberactivechannel,username,tmp1);
     pmemberrefreshid=deleteitem(pmemberrefreshid,username);
     additemtoobject(&pmemberrefreshid,username,"0");
  }
}
void channelmembers()
{
   char tmp1[32];
  sscanf(input,"%*s %*s %s",tmp1);
 char *username=getitem(pmemberauthtoken,tmp1);
  if(username==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
  printf("\n%s\n ",username);
 char *channelname=getitem(pmemberactivechannel,username);
   if(channelname==NULL)
  {
    sendjson("Error","not joined in any channel");
    return;
  }
  printf("\n%s\n ",channelname);
  channelmembersecondversion(pmemberactivechannel,channelname);
}
void joinchannel()
{
    char tmp1[100],tmp2[100],tmp3[110];
  sscanf(input,"%*s %*s %s %s",tmp1,tmp2);
   char *username=getitem(pmemberauthtoken,tmp2);
  if(username==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
  printf("\n%s\n ",username);
  tmp1[strlen(tmp1)-1]='\0';
  FILE *channel;
  strcpy(tmp3,"channel\\");
  strcat(tmp3,tmp1);
  strcat(tmp3,".json");
  channel=fopen(tmp3,"r");
  if(channel==NULL)
  {
      sendjson("Error","channel with this name doesnt exist");
      fclose(channel);
      return;
  }
    channel=fopen(tmp3,"r");
    char linescan[4000],tmpp[200];
    strcpy(tmpp,username);
    strcat(tmpp," joined.");
    fgets(linescan,4000,channel);
    fclose(channel);
    appendtojson(linescan,"server",tmpp,tmp3);
     sendjson("Successful","");
     pmemberactivechannel=deleteitem(pmemberactivechannel,username);
     additemtoobject(&pmemberactivechannel,username,tmp1);
     pmemberrefreshid=deleteitem(pmemberrefreshid,username);
     additemtoobject(&pmemberrefreshid,username,"0");
}
void leavechannel()
{
    char tmp1[32],tmp3[100];
  sscanf(input,"%*s %s",tmp1);
   char *username=getitem(pmemberauthtoken,tmp1);
  if(username==NULL)
  {
      puts("wdjwifdjwi");
    sendjson("Error","auth token doesnt match");
    return;
  }
  printf("\n%s\n ",username);
  char *channelname=getitem(pmemberactivechannel,username);
   if(channelname==NULL)
  {
    sendjson("Error","not joined in any channel");
    return;
  }
  printf("\n%s\n ",channelname);
    strcpy(tmp3,"channel\\");
  strcat(tmp3,channelname);
  strcat(tmp3,".json");
    FILE *openchannel=fopen(tmp3,"r");
 char linescan[4000],tmpp[200];
    strcpy(tmpp,username);
    strcat(tmpp," left.");
    fgets(linescan,4000,openchannel);
    fclose(openchannel);
    appendtojson(linescan,"server",tmpp,tmp3);
    sendjson("Successful","");
    pmemberactivechannel=deleteitem(pmemberactivechannel,username);
    pmemberrefreshid=deleteitem(pmemberrefreshid,username);
}
void sendmessages()
{
    int i,j;
    char tmp3[100];
    char message[200],tmptoken[32];
    puts("h");
    for( i=5;;i++)
    {
        if(input[i]!=',')
        message[i-5]=input[i];
        else
        break;
    }
    message[i-5]='\0';
    for(j=0;j<32;j++)
    tmp3[j]=input[i+j+2];
    tmp3[32]='\0';
     char *username=getitem(pmemberauthtoken,tmp3);
  if(username==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
  printf("\n%s\n ",username);
   char *channelname=getitem(pmemberactivechannel,username);
   if(channelname==NULL)
  {
    sendjson("Error","not joined in any channel");
    return;
  }
  printf("\n%s\n ",channelname);
  strcpy(tmp3,"channel\\");
  strcat(tmp3,channelname);
  strcat(tmp3,".json");
    FILE *openchannel=fopen(tmp3,"r");
 char linescan[4000],tmpp[200];
    fgets(linescan,4000,openchannel);
    appendtojson(linescan,username,message,tmp3);
    fclose(openchannel);
    sendjson("Successful","");
}
void refresh()
{
  char tmp1[32],tmp3[100];
  sscanf(input,"%*s %s",tmp1);
   char *username=getitem(pmemberauthtoken,tmp1);
  if(username==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
  printf("\n%s\n ",username);
  char *channelname=getitem(pmemberactivechannel,username);
   if(channelname==NULL)
  {
    sendjson("Error","not joined in any channel");
    return;
  }
  printf("\n%s\n ",channelname);
    strcpy(tmp3,"channel\\");
  strcat(tmp3,channelname);
  strcat(tmp3,".json");
  int refreshcounter=0;
  char* t=getitem(pmemberrefreshid,username);
   printf("---%s----",t);
   for(int i=0;i<strlen(t);i++)
   {
       refreshcounter=refreshcounter*10+t[i]-'0';
   }
   printf("---%d----",refreshcounter);
    FILE *openchannel=fopen(tmp3,"r");
 char linescan[4000];
    fgets(linescan,4000,openchannel);


    char meseg[4000],x[4000];
    int i;
    for(i=12;i<strlen(linescan)-1;i++)
    {
        meseg[i-12]=linescan[i];
    }
    meseg[i-12]='\0';
    int counter1=0,counter2=0,flag=0;
    i=0;
    int arraysize=0;
    for(i=0;i<strlen(meseg);i++)
    {
        if(meseg[i]=='{')
        arraysize++;
    }
    for(i=1;i<strlen(meseg)-1;i++)
    {
        if((meseg[i]=='}')&&(flag==0))
            {
                counter1++;
                continue;
            }
        if(counter1==refreshcounter)
        {
            x[counter2]=meseg[i];
            counter2++;
            flag=1;
        }
    }
    x[counter2]='\0';
    if(x[0]==',')
    {
        int i;
        for(i=0;i<strlen(x)-1;i++)
            x[i]=x[i+1];
        x[i]='\0';
    }
    strcpy(meseg,"{\"type\":\"list\",\"content\":[");
    strcat(meseg,x);
    strcat(meseg,"]}");
    printf("------------------\n%s\n---------------------",meseg);
    send(client_socket,meseg, strlen(meseg), 0);
    fclose(openchannel);
    pmemberrefreshid=deleteitem(pmemberrefreshid,username);
    char xx[5];
    sprintf(xx,"%d",arraysize);
    additemtoobject(&pmemberrefreshid,username,xx);
}
void searchmember()
{
     char membertobesearched[1000],tmp2[100];
  sscanf(input,"%*s %s %s",membertobesearched,tmp2);
  char* username=getitem(pmemberauthtoken,tmp2);
  if(username==NULL)
  {
    sendjson("Error","auth token doesnt match");
    return;
  }
  membertobesearched[strlen(membertobesearched)-1]='\0';
  char *channelname=getitem(pmemberactivechannel,username);
  printf("\n%s\n ",channelname);

  char *a=getitem(pmemberactivechannel,membertobesearched);
  if(a==NULL)
  {
      send(client_socket,"FALSE",5,0);
      return;
  }
      if(strcmp(a,channelname)==0)
      {
         send(client_socket,"TRUE",4,0);
         return;
      }
   send(client_socket,"FALSE",5,0);
}
void channelmembersecondversion(cAMIR *x,char *channelname)
{
    char output[500];
    strcpy(output,"{\"type\":\"list\",\"content\":[");
     if(x==NULL)
        return;
     while(x->next!=NULL)
     {
         if(strcmp(x->content,channelname)==0)
         {
             strcat(output,"\"");
             strcat(output,x->type);
             strcat(output,"\",");
         }
        x=x->next;
     }
       if(strcmp(x->content,channelname)==0)
         {
             strcat(output,"\"");
             strcat(output,x->type);
             strcat(output,"\",");
         }
    if(output[strlen(output)-1]=',')
        output[strlen(output)-1]='\0';
    strcat(output,"]}");
    printf("%s\n",output);
    send(client_socket,output,strlen(output),0);
}
char* appendtojson(char *a,char *sender,char *message,char *filename)
{
    char output[6000];
    strcpy(output,a);
    output[strlen(a)-2]='\0';
    strcat(output,",{\"sender\":\"");
    strcat(output,sender);
    strcat(output,"\",\"content\":\"");
    strcat(output,message);
    strcat(output,"\"}]}");
    printf("\n%s\n",output);
    FILE *openchannel=fopen(filename,"w");
    fprintf(openchannel,"%s",output);
    fclose(openchannel);
}
void additemtoobject(cAMIR **head,char *a,char *b)
{
    cAMIR *newnode=(cAMIR*)malloc(sizeof(cAMIR));
    cAMIR *last=*head;
    strcpy(newnode->type,a);
    strcpy(newnode->content,b);
    newnode->next=NULL;
    if(*head==NULL)
    {
        *head=newnode;
        return;
    }
    while(last->next!=NULL)
    last=last->next;
    last->next=newnode;
}
void printunformatted(cAMIR *x)
{
    if(x==NULL)
        return;
     while(x->next!=NULL)
     {
        printf("type:%s content:%s\n",x->type,x->content);
        x=x->next;
     }
       printf("type:%s content:%s\n",x->type,x->content);
}
char* getitem(cAMIR *x,char *y)
{
    if(x==NULL)
        return(NULL);
while(x->next!=NULL)
     {
        if(strcmp(x->type,y)==0)
            return(x->content);
           x=x->next;
     }
        if(strcmp(x->type,y)==0)
            return(x->content);
        return(NULL);
}
cAMIR *deleteitem(cAMIR *head,char *y)
{
   if(head==NULL)
    return(NULL);

   if(strcmp(y,head->type)==0)
   {
    cAMIR *tmp;
    tmp=head->next;
    free(head);
    return(tmp);
   }
   head->next=deleteitem(head->next,y);
   return (head);
}
