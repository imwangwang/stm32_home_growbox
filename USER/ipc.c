#include <stdio.h>

/* simple IPC communication structure */
#define MAX_SIZE 255
#define TRUE 1
#define FALSE 0

int buffer[MAX_SIZE]={0};


int CAS(int *pointer, int a, int b)
{
	int ret;
	__disable_irq();
	ret = ((*pointer == b) ? (*pointer = a), 1 : 0);
	__enable_irq();
	return ret;
}


int Insert(int data)
{
    int i=0;
    for(i=0;i<MAX_SIZE;i++)
    {
        if(CAS(&buffer[i], data, 0))
            return i;
    }
    return -1;
}
int InsertAt(int data,int index)
{
    if(CAS(&buffer[index], data, 0))
        return TRUE;
    return FALSE;
}
int Delete(int *data)
{
    int i=0;
    for(i=0;i<MAX_SIZE;i++)
    {
        *data=buffer[i];
        if(*data!=0)
        {
            if(CAS(&buffer[i],0, *data))
                return i;
        }
    }
    return -1;
}
//Find free slot at max “range?away from “startIndex ?and return the offset.
int FindFreeSlot(int startIndex,int range)
{
    int i;
    //find free slot onwards from start index and return the relative offset if found
    for(i=startIndex+1;i<MAX_SIZE&&i<startIndex+range;i++) {
        if(buffer[i]==0) return i-startIndex;
    }
    if(startIndex+range>=MAX_SIZE) {
        //find free slot from 0 index onward but before startIndex and in range
        for(i=0;i<startIndex&&i<=(startIndex+range)%MAX_SIZE;i++) {
            if(buffer[i]==0) return i+(MAX_SIZE-startIndex);
        }
    }
    return -1;
}
//Find first free slot
int FindFreeSlotAll()
{
    int i;
    for(i=0;i<MAX_SIZE;i++) {
        if(buffer[i]==0) return i;
    }
    return -1;
}


int send_a_cmd(int cmd)
{
	int insert_at = -1;

	insert_at = FindFreeSlotAll();
	if (insert_at == -1)
		return -1;

//	printf("get a valid slot at : %d\n", insert_at);
	
	if (!InsertAt(cmd, insert_at))
		return -1;

//	printf("cmd inserted: %d\n", cmd);
	
	return 0;
}

int get_a_cmd(int *cmd)
{
	int ret = Delete(cmd);
	
//	if (ret != -1)
//		printf("get a valid cmd:%d\n", *cmd);
	
	return ret;
}


