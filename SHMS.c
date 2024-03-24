#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define SHM_SIZE 100
#define SHM_NAME "my_shared_memory"

void ReceiverThread(LPVOID lpParam) 
{
    HANDLE hMapFile;
    LPCTSTR pBuf;
    char message[100];
    char mess[100]={};
    int flag=0;
    char chat_logs[100][100];  
    int j=0;  
    hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, SHM_NAME);

    if (hMapFile == NULL)
    {
        perror("OpenFileMapping");
        return;
    }

    pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, SHM_SIZE);

    if (pBuf == NULL) 
    {
        perror("MapViewOfFile");
        CloseHandle(hMapFile);
        return;
    }

   while (1) {
        strcpy(message,(char*)pBuf);
         if (strcmp(mess,message)==0)
            {
                flag=1;
                if (flag==1)
                {
                    //free((char*)pBuf);
                    strcpy(message,"");
                }
                else
                {
                    printf("No New message Received . . . . \n");
                    Sleep(3000);
                }
            }
        if (strncmp(pBuf, "exit", 4) == 0) 
        {
            break; 
        }
        else if((strncmp(message,"\n",2)==0 && strlen(message)==3) || (strncmp(message, "0000",4)==0 && strlen(message)==5 ))
        {
            printf("No New message Received . . . . \n");
            continue;
        }
        else if (strlen(message) > 0) 
        {
            printf("\nReceived message: %s\n", message);
            strcpy(chat_logs[j],message);
            j++;
            strcpy(mess,message);           
        }
        
        Sleep(500);
    }
    printf("\n\nThe Entered Message logs are:\n");
    for(int i=0;i<j;i++)
    {
        printf("line %d : %s\n",(i+1),chat_logs[i]);
    }


    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
}

int main()
{
    HANDLE hMapFile;
    LPCTSTR pBuf;
    char message[100];

    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHM_SIZE, SHM_NAME);
    if (hMapFile == NULL) {
        perror("CreateFileMapping");
        return 1;
    }

    pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHM_SIZE);

    if (pBuf == NULL) {
        perror("MapViewOfFile");
        CloseHandle(hMapFile);
        return 1;
    }

    HANDLE hReceiverThread = (HANDLE)_beginthread(ReceiverThread, 0, NULL);

    while (1) 
    {
        printf("\nEnter message to send (or type 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);
        strcpy((char *)pBuf, message);

        if (strncmp(message, "exit", 4) == 0) 
        {
            break;
        }
        Sleep(100); 
    }

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    WaitForSingleObject(hReceiverThread, INFINITE);
    CloseHandle(hReceiverThread);

    return 0;
}
