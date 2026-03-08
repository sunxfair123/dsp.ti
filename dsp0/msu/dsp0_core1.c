/******************************************************************************

  Copyright (C), 2001-2012, Texas Instrument.

 ******************************************************************************
  File Name     : dsp0_core1.c
  Version       : Initial Draft
  Author        : sxf
  Created       : 2016/01/08
  Last Modified :
  Description   : This file is the entrance of MSU                
  Function List :
              LL2_EDC_ISR
              Robust_Interrupts_Init
              Timer_ISR
  History       :
  1.Date        : 
    Author      : 
    Modification: 

******************************************************************************/
#include <stdio.h>
#include <c6x.h>
#include <tistdtypes.h>
#include <csl_cpintc.h>
#include "dsp0_common.h"
#include "CSL_msmc.h"
#include "CSL_msmcAux.h"
#include <cslr_chip.h>

#include "dsp0_core1.h"
#include "KeyStone_Packet_Descriptor.h"
//#include "../src/Robust_memory_transfer.h"
//#include "../src/Robust_MNav_Init.h"


/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/


/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
//[cpp] view plain copyprint?
#include <stdlib.h>  
//#include <stdio.h> 
#include <math.h>
#include <time.h>  




//#include <time.h>
//#include <stdio.h>
//#include <sys/types.h>
//#include <sys/timeb.h>
#include <string.h>

Uint32 uiaDescriptorBufferCore1[32];

/* for 319 project debug */
/* 构造数据用于验证 */
Uint32 packetBuffer_DDR_Check_core1[64]=
{
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0xbbbbbbbb, 0xcccccccc, 0xdddddddd,
0x12345678, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0xdddddddd,
0xffffffff, 0xaaaaaaaa, 0x55555555, 0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0x12345678, 0x89abcdef, 0xffffffff, 
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xbbbbbbbb, 
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0xbbbbbbbb, 0xcccccccc, 0xdddddddd,
0x12345678, 0x89abcdef, 0xffffffff, 0xaaaaaaaa, 0x55555555, 0xffffffff, 0xaaaaaaaa, 0x55555555
};

/* for 319 project debug */
/* 构造数据用于EDMA验证 */
Uint32 packetBuffer_DDR_EDMA_Check_core1[64];

#if 0
//临时调试修改
#define M1 480
#define N1 280
#define Maplistlength 100

#define M1_result 476
#define N1_result 276


#pragma DATA_SECTION(BW,"ArrayData.Core0_buffer_DDR")
#pragma DATA_SECTION(BW_restlt,"ArrayData.Core0_buffer_DDR")
#pragma DATA_SECTION(BW_average,"ArrayData.Core0_buffer_DDR")
#pragma DATA_SECTION(Node,"ArrayData.Core0_buffer_DDR")
#pragma DATA_SECTION(target,"ArrayData.Core0_buffer_DDR")






//unsigned short BW[N1][M1]; 
//unsigned short BW_restlt[N1_result][M1_result];
//short BW_average[N1_result][M1_result];
float filter[5][5];

unsigned short BW[N1][M1];
unsigned short BW_restlt[N1_result][M1_result];
short BW_average[N1_result][M1_result];
typedef struct {  
    unsigned short sc;  
    unsigned short ec;  
    unsigned short r;  
	unsigned short area;
    int label;  
}RunNode_t; 


typedef struct {  
    unsigned short row;  
    unsigned short column;  
    unsigned short area;  
	unsigned short label; 
}sttarget;

RunNode_t Node[2000];
sttarget  target[2000];



typedef struct MaplistHead{  
    unsigned int listlength;//node 个数
	unsigned int listIdxfind;//查找找到的node位置索引
	
	unsigned int listemptynum;//被删除的node数目
	unsigned int listemptycurrent;//需要新node时，开始位置	
	unsigned int listemptyoffset[8];//存放node位置索引listempty_offset
    //unsigned short reserve;	
     
	unsigned int* list1stptr;//第一个node位置指针
	unsigned int reserve;
}MaplistHead_t;
#define listempty_offset 8
typedef struct Maplist{  
    unsigned int labelnum;  //node中包含的label个数
    //unsigned short reserve;  
    unsigned int label[1023]; //label的索引   
}Maplist_t; 
#define labelnum_maplist 1023
  
typedef struct equivnode   
{  
    int LabelA;  
    int LabelB;  
    struct equivnode *next;  
}EquivNode;  
  
typedef struct equivlist   
{  
    EquivNode *head;  
    int length;  
}EquivList;  
  
typedef struct MapListNode_tag  
{  
    int idx;  
    struct MapListNode_tag* pNext;  
}MapListNode_t;  
  
typedef struct MapListList_tag  
{  
    MapListNode_t* pMapNodeList;  
    struct MapListList_tag* pNext;  
}MapListListNode_t;  
  
typedef struct    
{  
    int mode ;   
    int minarea ;   
    int minpixDiff;  
}SparseInfoExtractCondition_t; 





void DeleteEquivListHeadNode(EquivList *list)   
{  
    EquivNode *killNode;  
    if (list->head != NULL)   
    {  
        killNode = list->head;  
        list->head = killNode->next;  
        free(killNode);  
        list->length--;  
    }  
}  
  
void UpdateRunNode(int *pTable , RunNode_t* pNode, int NodeNum)  
{  
    int i;  
    for(i = 0 ; i < NodeNum ; i++)  
    {  
        pNode[i].label = pTable[pNode[i].label-1];  
    }  

//代码优化，直接在这里获取目标点相关信息，不再需要UpadateBWData
#if 0
	//获取目标点
    for(i = 0 ; i < NodeNum ; i++)  
    {  
        TargetArea[pNode[i].label] += pNode[i].area;  
    } 

	//删除虚假目标点	
	for(i = 0 ; i < labels ; i++)	
	{  
		if(TargetArea[i]<5) 
			TargetArea[i]=0;

		else
			TargetNum++;
	} 
#endif /* 0 */
}  

//重要处理流程  1  
void BWCreateRunNodeInfoExnoprint1(short* BW , int M ,int N ,  
                           SparseInfoExtractCondition_t* pCondtion,  
                           RunNode_t** ppRunNode, int *pRunNodeNum,  
                           int* plabels, EquivList** ppEquivList)  
{  
    //int *pr = NULL;  
    short *pr = NULL; 
    int numRuns = 0;  
    RunNode_t* pNode = NULL;  
    int offset ;  
    int i ,j ,k ,p;    
    int currentRow = -1;  
    int nextLabel = 1;  
    int firstRunOnPreviousRow = -1;  
    int lastRunOnPreviousRow = -1;  
    int firstRunOnThisRow = -1;  
    EquivList* pEquivList = (EquivList*)malloc(sizeof(EquivList));  
  
    int Mode = pCondtion->mode;  
    int MinArea = pCondtion->minarea;  
    int MinpixDiff = pCondtion->minpixDiff;  
  
    //(1)Computer numRuns  
    for (i = 0; i < N; i++)   //按行遍历  
    {  
        pr = &BW[i*M] ;    //第row行的头指针  
  
        if (pr[0] != 0)  
            numRuns++;        //本行的第一个元素就是一个Run的头 Run的数目+1  
  
        for (j = 1; j < M; j++) {  
            if ((pr[j] != 0) && (pr[j-1] == 0))   //01的模式就是Run  
                numRuns++;                        //Run的总数+1  
        }  
    }  
    *pRunNodeNum = numRuns;  
  
    //(2) 生成 RunNode信息  
    //pNode = (RunNode_t*)malloc(numRuns*sizeof(RunNode_t));  
    pNode = (RunNode_t* )Node;
    k = 0;  
    for (i = 0; i < N; i++)   
    {  
        pr = &BW[i*M];  //第row行的头指针  
  
        j = 0;  
        while (j < M)   
        {       
            //遍历该行  
            while (j < M && pr[j] == 0)  
                j++;                        //找到第一个Run所在的行号  
  
            if ((j < M) && (pr[j] != 0))   
            {  
                pNode[k].r = i ;       //  
                pNode[k].sc = j ;  
                while (j < M && pr[j] )   //走完这个Run  
                    j++;  
                pNode[k].ec = j-1;  //这个是结尾  
                pNode[k].area = pNode[k].ec-pNode[k].sc+1;
				pNode[k].label = 0;
                k++;          //处理下一个Run  
            }  
        }  
    }  
  
    //(3)扫描标记 ,并统计Label 关系的  
    //printf("\n\n");  
    //printf("(2)打印等价关系对\n");  
    pEquivList->head = NULL;  
    pEquivList->length = 0;  

	//八邻域为1  ,四邻域为0  
	if(Mode==1)
    	offset = 1;  //八邻域为1  ,四邻域为0  
    else
		offset = 0;

	
    for (k = 0; k < numRuns; k++)   
    {  
        if (pNode[k].r == currentRow + 1)   
        {     
            //第k个Run与第k-1个Run在相邻的两行上  
            firstRunOnPreviousRow = firstRunOnThisRow;   //firstRunOnPreviousColumn指向第k-1个Run所在的列的第一个（最上面的）Run  
            firstRunOnThisRow = k;  
            lastRunOnPreviousRow = k-1;  
            currentRow = pNode[k].r;  
        }  
        else if (pNode[k].r > (currentRow + 1))   
        {  
            /* 开始新列:前一列没有Run  直接进入下一轮循环*/  
            firstRunOnPreviousRow = -1;  
            lastRunOnPreviousRow = -1;  
            firstRunOnThisRow = k;  
            currentRow = pNode[k].r;  
        }  
  
        if (firstRunOnPreviousRow >= 0)   
        {  
            p = firstRunOnPreviousRow;  
            while (p <= lastRunOnPreviousRow && pNode[p].sc <= pNode[k].ec + offset)   
            {  
                if ((pNode[k].ec >= pNode[p].sc-offset) &&   
                    (pNode[k].sc <= pNode[p].ec+offset)) //有交叉  
                {  
                    if (pNode[k].label == 0)   
                    {  
                        pNode[k].label = pNode[p].label;  
                    }  
                    else   
                    {  
                        if (pNode[k].label != pNode[p].label)   
                        {  
                            EquivNode *newNode = (EquivNode *)malloc(sizeof(EquivNode));  
                            newNode->LabelA = pNode[k].label;  
                            newNode->LabelB = pNode[p].label;  
                            newNode->next = pEquivList->head;  
                            pEquivList->head = newNode;  
                            pEquivList->length++;  
                            //printf("%4d %4d\n" , newNode->LabelA , newNode->LabelB);  
                        }  
                    }  
                }  
                p++;  
            }  
        }  
  
        if (pNode[k].label == 0)   
        {  
            pNode[k].label = nextLabel;  
            nextLabel++;  
        }  
    }  
    *plabels =nextLabel-1;  
    *ppRunNode = pNode;  
    *ppEquivList = pEquivList;  
  
#if 0
    printf("\n\n(3) 打印初始RunNode信息\n");  
    printf("  sc   ec    r   label    area \n");  
    for(i=0;i<numRuns;i++)  
    {  
        printf("%4d ",pNode[i].sc);  
        printf("%4d ",pNode[i].ec);  
        printf("%4d ",pNode[i].r);  
        printf("%4d ",pNode[i].label); 
		printf("%4d ",pNode[i].area);
        printf("\n");  
    }  
#endif /* 0 */
}  



void MapListListDeleteAllNode(MapListListNode_t* pMapListListNodeHead)  
{    
    MapListListNode_t* pListListNodeKill = NULL;  
    MapListNode_t* pMapNodeListMove = NULL; 
	MapListNode_t* pMapListNodeKill = NULL;
    MapListListNode_t* pMapListListMove = NULL;

	pListListNodeKill= pMapListListNodeHead;
	pMapListListMove = pMapListListNodeHead->pNext;
	free(pListListNodeKill);
	
		
    while(pMapListListMove)  
    {  
		pMapNodeListMove = pMapListListMove->pMapNodeList;
        while(pMapNodeListMove)   
        {                
  			pMapListNodeKill = pMapNodeListMove;
            pMapNodeListMove = pMapNodeListMove->pNext;  
			free(pMapListNodeKill);
        }

		//获取要free的listlist
		pListListNodeKill= pMapListListMove;
		//获取下一个listlist
		pMapListListMove = pMapListListMove->pNext;
		
		free(pListListNodeKill);		
    }    
}  

void MapListAddNode1(Maplist_t* pNode , int Idx)  
{  
	//int i;
	unsigned int labelnum;
	Maplist_t* ptrNode = NULL;
	
	ptrNode = pNode;
	//增加idx这个label
	if(ptrNode->labelnum<labelnum_maplist)
	{		
		labelnum = ptrNode->labelnum;
		ptrNode->label[labelnum] = Idx;
		ptrNode->labelnum+=1;

		if(ptrNode->labelnum>=labelnum_maplist)
			printf("labelnum full!!!\r\n");
	}
	else
		printf("MapListAddNode1 overflow!!!\r\n");		
}  


void MapListListAddNode1(MaplistHead_t* pListListNode ,int labela, int labelb)  
{  
	unsigned int listlength;
	unsigned int listemptycurrent;
	MaplistHead_t* ptrListHead = NULL;
	Maplist_t* ptrNode = NULL;

	
	ptrListHead = pListListNode;
	listemptycurrent = ptrListHead->listemptycurrent;
	//超过最大，不再增加这个节点
	if(ptrListHead->listlength>=Maplistlength)
	{
		printf("MapListListAddNode1 Maplistlength overflow!!!\r\n");
	}

	else
	{
		if(ptrListHead->listemptynum>listempty_offset)
		{
			printf("MapListListAddNode1 overflow!!!\r\n");
		}
		else
		{
			if(ptrListHead->listemptynum>0)
			{
				ptrNode = (Maplist_t*)(ptrListHead->list1stptr 
			    + ptrListHead->listemptyoffset[listemptycurrent]*(labelnum_maplist+1));

				//修改空白节点
				ptrListHead->listemptynum--;
				ptrListHead->listemptycurrent=((listemptycurrent+1)%listempty_offset);
					
				//增加节点
				ptrNode->label[0]=labela;
				ptrNode->label[1]=labelb;
				ptrNode->labelnum = 2;		

				ptrListHead->listlength++;
			}
			else
			{
				listlength = ptrListHead->listlength;
				ptrNode = (Maplist_t* )(ptrListHead->list1stptr + listlength*(labelnum_maplist+1));

				//增加节点
				ptrNode->label[0]=labela;
				ptrNode->label[1]=labelb;
				ptrNode->labelnum = 2;
				ptrListHead->listlength += 1;	
			}
		}
	}
}  

void MapListListDeleteNode1(MaplistHead_t* pListListNode ,Maplist_t* pListNode)  
{  
	//unsigned int listlength;
	unsigned int listemptycurrent;
	unsigned int listIdxfind;
	unsigned int ListNodeadd;
	MaplistHead_t* ptrListHead = NULL;
	Maplist_t* ptrNode = NULL;

	
	ptrListHead = pListListNode;
	ptrNode = pListNode;
	listIdxfind=ptrListHead->listIdxfind;//pListNode的offset
	listemptycurrent = ptrListHead->listemptycurrent;

	ptrNode->labelnum = 0;	
	//在最后一个节点索引位置找到
	if((listIdxfind+1)==(ptrListHead->listlength+ptrListHead->listemptynum))
	{
		ptrListHead->listlength--;
	}
	else
	{
		if(ptrListHead->listemptynum<listempty_offset)
		{
			ListNodeadd = ((listemptycurrent + ptrListHead->listemptynum)%listempty_offset);
			//修改空白节点
			ptrListHead->listemptynum++;
			ptrListHead->listemptyoffset[ListNodeadd] =	listIdxfind;

			ptrListHead->listlength--;
		}
		else
		{
			printf("MapListListDeleteNode1 overflow!!!\r\n");	
		}  
	}
}  


int MapListMinIdx1(Maplist_t* pNode)  
{  
	int labelnum,i;
	unsigned int min = 0x7FFFFFFF; 
	labelnum = pNode->labelnum;
    for(i=0;i<labelnum;i++) 
    {  
		if(pNode->label[i] < min)  
            min = pNode->label[i];          
    }  
    return min;  
}  

Maplist_t* MapListListFindIdx1(MaplistHead_t* pMaplistHead ,int Idx)  
{  
	unsigned int i,j,k;
	//Maplist_t* pMapListMove =NULL;
	Maplist_t* pMapListMove = (Maplist_t* )pMaplistHead->list1stptr;
	MaplistHead_t* ptrlistHead = pMaplistHead;
	k=0;
	for(i=0;i<ptrlistHead->listlength;i++)
	{
		while(!pMapListMove->labelnum)
		{ 
			pMapListMove = (pMapListMove+1);
			k++;
		}
		for(j=0;j<pMapListMove->labelnum;j++)
		{
			if(Idx == pMapListMove->label[j]) 
				{
					ptrlistHead->listIdxfind=(i+k);
					return pMapListMove; 
				}       
		}
		pMapListMove = (pMapListMove+1);
	}
    return NULL;  
} 

void MapListMerge1(Maplist_t* pMapListA , Maplist_t* pMapListB)  
{  
    unsigned int labelnumA;
	unsigned int labelnumB;
	unsigned int i;
	Maplist_t* pMapListFindA = NULL;  
    Maplist_t* pMapListFindB = NULL;
	
	i = 0;
	pMapListFindA = pMapListA;
	pMapListFindB = pMapListB;
	labelnumA = pMapListFindA->labelnum;
	labelnumB = pMapListFindB->labelnum;

	//发生下列情况，丢弃部分label
	if((labelnumA+labelnumB)>labelnum_maplist)
		printf("MapListMerge1 overflow!!!\r\n");
	else
	{		
		if(labelnumA<labelnum_maplist)
		{
			while(i<labelnumB)
			{
				pMapListFindA->label[labelnumA]= pMapListFindB->label[i];
				i++;
				labelnumA++;
			}				
		}

		pMapListFindA->labelnum = labelnumA;
		pMapListFindB->labelnum = 0;

		if(pMapListFindA->labelnum>labelnum_maplist)
				printf("labelnum overflow 001!!!\r\n");
	}
}  


//重要处理流程  2  :add by snoopy 20160601
//使用表的格式代替链表
void CreateMapTableTestarray_noprint(EquivList* pList ,int MapTableSize ,int* plabelmaps, int **ppMapTable )	
{  
  unsigned int i,j,k,length;
  int labela,labelb;
  MaplistHead_t MaplistHead;
  Maplist_t  Maplist[100];//对应#define Maplistlength 100
  
#if 1
  Maplist_t* pMaplist = NULL ;
  Maplist_t*  pMapListMove= NULL ;
  MaplistHead_t*  pMaplistHead = NULL ;
  MaplistHead_t*  pMapListHeadMove = NULL ;
  Maplist_t* pMapListFindA = NULL;	
  Maplist_t* pMapListFindB = NULL;
  int *pMapTableInit = NULL;  

  

  pMaplistHead = &MaplistHead;
  pMaplist = &Maplist[0];

  //使用memset效率更高
  memset(pMaplistHead,0,sizeof(MaplistHead_t));
  memset(pMaplist,0,Maplistlength*sizeof(Maplist_t));

  //初始化pMaplistHead
  pMaplistHead->listemptynum=0;
  
  pMaplistHead->listlength=0;
  pMaplistHead->list1stptr=(unsigned int*)pMaplist;  
#endif /* 0 */  
  
#if 1
  while(pList->length) 
  { 		 
	  labela = pList->head->LabelA;  
	  labelb = pList->head->LabelB;  
	  pMapListFindA = MapListListFindIdx1(pMaplistHead,labela);  
	  pMapListFindB = MapListListFindIdx1(pMaplistHead,labelb);

	  if(pMapListFindA)  
	  {  
		  if(pMapListFindB)  
		  {  
			  //在建立的list中，labela和labelb命中，把labelb加入同一个map表中，同时删除labelb的list
			  if (pMapListFindA!=pMapListFindB)  
			  { 			
				  //把整个MapList表加入
				  MapListMerge1(pMapListFindA,pMapListFindB);  
				  MapListListDeleteNode1(pMaplistHead,pMapListFindB);  
			  }  
			  //在建立的list中，labela和labelb命中，而且相等，说明已经存在于同一个list中
		  }  
		  else	
		  {  
			  //在建立的list中，labela命中，由于labela和labelb属于同一个list，加入同一个map表中
			  MapListAddNode1(pMapListFindA,labelb);  
		  }  
	  }  
	  else	
	  {  
		  if(pMapListFindB)  
		  {  
			  //在建立的list中，labelb命中，由于labela和labelb属于同一个list，加入同一个map表中
			  MapListAddNode1(pMapListFindB,labela);  
		  }  
		  else	
		  {  
			  //在建立的list中，labela和labelb都没有找到
			  MapListListAddNode1(pMaplistHead,labela,labelb);	
		  }  
	  }  
	  DeleteEquivListHeadNode(pList);  
  }
   
#endif /* 0 */

  //begin:调试使用，正式代码去掉
#if 1
  printf("\n\n(4)打印等价链表:\n");  
  pMapListHeadMove = pMaplistHead;	
  length = pMapListHeadMove->listlength;
  pMapListMove = (Maplist_t* )pMapListHeadMove->list1stptr;
  while (length)  
  {  
	  while(!pMapListMove->labelnum)
	  { 
		  pMapListMove = (pMapListMove+1);
	  }
	  for(i=0;i<pMapListMove->labelnum;i++)
	  {
		  printf("%8d",pMapListMove->label[i]); 			 
	  }
	  printf("\n"); 
	  length--;
	  pMapListMove = (pMapListMove+1);
  }  
#endif /* 0 */
  //end:调试使用，正式代码去掉

  
#if 1
  pMapTableInit = (int*)malloc(MapTableSize*sizeof(int));  
  //printf("\n\n打印初始关系表");  
  for(i = 1 ; i <= MapTableSize ; i++ )  
  {  
	  pMapListMove = MapListListFindIdx1(pMaplistHead,i);  
	  if(pMapListMove)	
	  {  
		  //找到节点的最小idx
		  pMapTableInit[i-1] = MapListMinIdx1(pMapListMove);  
		  //printf("%3d " ,pMapTableInit[i-1]);  
	  }  
	  else	
	  {  
		  pMapTableInit[i-1] = i;  
		  //printf("%3d " ,i);  
	  }  
  }  

  //printf("\n\n打印对应关系表");  
  *ppMapTable = (int*)malloc(MapTableSize*sizeof(int));  
  (*ppMapTable)[0] = 1;  
  //printf("%3d ",(*ppMapTable)[0]);	
  k = 2;  
  for(i = 1 ; i < MapTableSize ; i++ )	
  {  
	  j = 0;  

	  while(j<i)  
	  {  
		  if(pMapTableInit[j] == pMapTableInit[i])	
		  {  
			  (*ppMapTable)[i] = (*ppMapTable)[j];	
			  break;  
		  }  
		  j++;	
	  }  

	  if(j == i)  
		  (*ppMapTable)[i] = k++;  

	  //printf("%3d ",(*ppMapTable)[i]);	
  }  

  *plabelmaps=k-1;
  
  free(pMapTableInit);
#endif /* 0 */
  
}  


//重要处理流程  
void BWCreateRunNodeInfo1(short* BW , int M ,int N ,  
                         SparseInfoExtractCondition_t* pCondtion, int* plabels,  
                         RunNode_t** ppRunNode, int *pRunNodeNum)  
{  
    EquivList* pEquivList = NULL;  
    int *pMapTable = NULL;  
    int labels = 0; //合并之前label总数
	//合并之后label总数
	int labelmaps = 0;  //add by snoopy 20160516:用于记录最终映射的label数量

	EquivNode* pEquivNode= NULL;


	//广度优先遍历算法消除错误标签:使用数组代替list	
	BWCreateRunNodeInfoExnoprint1(BW ,M ,N ,pCondtion,ppRunNode, pRunNodeNum,&labels ,&pEquivList);
	if(pEquivList->length)	
	{  				
		CreateMapTableTestarray_noprint( pEquivList, labels, &labelmaps, &pMapTable);
		UpdateRunNode(pMapTable,*ppRunNode,*pRunNodeNum);  
		*plabels=labelmaps;
	}
	else
	{
		*plabels=labels;
	}
}  
  
void UpadateBWData(short* BW, int M ,RunNode_t* pNode , int NodeNum,int labels)  
{  
    int k = 0 , p = 0;  
	int i= 0 ,TargetNum= 0;
	int t;
    short *pr = NULL;  
	sttarget *pTargetArea;
	sttarget *pTempTargetArea;
	RunNode_t* pNodetemp;
    for (k = 0; k < NodeNum; k++)   
    {  
        pr = &BW[pNode[k].r*M + pNode[k].sc];  
        for (p = 0; p < pNode[k].ec - pNode[k].sc + 1; p++)  
            *pr++ = pNode[k].label;  
    }  

	//获取目标点
	//pTargetArea = (sttarget*)malloc(labels*sizeof(RunNode_t)); 
	pTargetArea = (sttarget*)target;
	pTempTargetArea = pTargetArea;

	//使用memset效率更高
	memset(pTempTargetArea,0,labels*sizeof(RunNode_t));
#if 0
	for(i = 0 ; i < labels ; i++)	
	{ 		
		*pTempTargetArea=0;		
		pTempTargetArea++;
	}
#endif /* 0 */
	
	pTempTargetArea = pTargetArea;
    for(i = 0 ; i < NodeNum ; i++)  
    {  
		t=pNode[i].label;
		//pTempTargetArea = pTargetArea+t-1;
        //*pTempTargetArea += pNode[i].area;  
        pTempTargetArea[t-1].area+= pNode[i].area;
		pTempTargetArea[t-1].label = t;
		pTempTargetArea[t-1].row = (pNode[i].r+pTempTargetArea[t-1].row)/2;
		pTempTargetArea[t-1].column = ((pNode[i].sc+pNode[i].ec)/2+pTempTargetArea[t-1].column)/2;
    } 

	
	
	//删除虚假目标点
#ifdef 0
	printf("\n(6) 获取目标点");
	printf("\n");
	printf("row   column   area  label\n"); 
#endif /* 0 */
	pTempTargetArea = pTargetArea;
	for(i = 0 ; i < labels ; i++)	
	{  
		if(pTempTargetArea[i].area<5) 
			pTempTargetArea[i].area=0;

		else
		{
			TargetNum++;	
//验证结果是否正确时打开			
#if 0
			printf("%4d ",pTempTargetArea[i].row); 
			printf("%8d ",pTempTargetArea[i].column);
			printf("%4d ",pTempTargetArea[i].area);
			printf("%4d ",pTempTargetArea[i].label);
    		printf("\n");		
#endif /* 0 */
		} 
	}
#if 0
	for(i = 0 ; i < labels ; i++)	
	{  
		if(*pTempTargetArea<5) 
			*pTempTargetArea=0;

		else
		{
			TargetNum++;
			t=i+1;
			
			printf("%4d           ",t); 
			printf("%4d ",*pTempTargetArea); 
    		printf("\n");		
		} 
		pTempTargetArea++;
	}
#endif /* 0 */
	//printf("目标个数:%4d ",TargetNum); 
    //printf("\n");		
}  

inline void imfilter_mex(unsigned short* BW, short** result )
{
	unsigned int i , j ;
	unsigned int BW_temp[5];
	unsigned int k,flag; 
	float filter1=0.04;;//flag每行第一列判断标志
	float sum_average=0.0;
	float sum2_square=0.0;
	float aveGray,stdGray,threshold;
	unsigned short* pr[5];
	unsigned long long cycles64;
	unsigned short* pBW0;
	unsigned short* pBW1;
	unsigned short* pBW2;
	unsigned short* pBW3;
	unsigned short* pBW4;
	unsigned short* pBWResult;
	short* paverage = NULL;

	short* pr1 = NULL; 
    int numRuns = 0; 

	//获取模板系数
#ifdef 0
	for(i = 0;i<5;i++)
	{
		for(j = 0;j<5;j++)
		{
			filter[i][j]=0.04;
		}
	}	
#endif /* 0 */
	filter1 =0.04;

	//按行遍历  
	//cycles64=_itoll(TSCH, TSCL);	

	//pBW1 = (unsigned short*)&BW[0] ;    //第row行的头指针  
	//pBW2 = (unsigned short*)&BW[480] ;    //第row行的头指针 
	//pBW3 = (unsigned short*)&BW[960] ;    //第row行的头指针 
	//pBW4 = (unsigned short*)&BW[1440] ;    //第row行的头指针

	//pr[0] = &BW[i*M1] ;    //第row行的头指针  
	pr[1] = &BW[0] ;    //第row行的头指针 
	pr[2] = &BW[480] ;    //第row行的头指针 
	pr[3] = &BW[960] ;    //第row行的头指针 
	pr[4] = &BW[1440] ;    //第row行的头指针 
	for(i = 0;i<N1_result;i++)
	{
		flag=1;
		pBWResult= (unsigned short*)&BW_restlt[i] ;
		paverage = (short*)&BW_average[i] ;
		
		//pr[0] = &BW[i*M1] ;    //第row行的头指针  
		//pr[1] = &BW[(i+1)*M1] ;    //第row行的头指针 
		//pr[2] = &BW[(i+2)*M1] ;    //第row行的头指针 
		//pr[3] = &BW[(i+3)*M1] ;    //第row行的头指针 
		//pr[4] = &BW[(i+4)*M1] ;    //第row行的头指针 

		//pBW0 = pBW1;    //第row行的头指针  
		//pBW1 = pBW2 ;    //第row行的头指针 
		//pBW2 = pBW3 ;    //第row行的头指针 
		//pBW3 = pBW4 ;    //第row行的头指针 
		//pBW4 = (unsigned short*)&BW[(i+4)*M1] ;    //第row行的头指针 
		
		pr[0] = pr[1];    //第row行的头指针  
		pr[1] = pr[2] ;    //第row行的头指针 
		pr[2] = pr[3];    //第row行的头指针 
		pr[3] = pr[4];    //第row行的头指针 
		pr[4] = &BW[(i+4)*M1] ;    //第row行的头指针 
		
		
		for(j = 0;j<M1_result;j++)
		{
			if(flag==1)
			{				
				BW_temp[0]=(pr[0][0]+pr[1][0]+pr[2][0]+pr[3][0]+pr[4][0])*filter1;
				BW_temp[1]=(pr[0][1]+pr[1][1]+pr[2][1]+pr[3][1]+pr[4][1])*filter1;
				BW_temp[2]=(pr[0][2]+pr[1][2]+pr[2][2]+pr[3][2]+pr[4][2])*filter1;
				BW_temp[3]=(pr[0][3]+pr[1][3]+pr[2][3]+pr[3][3]+pr[4][3])*filter1;
				BW_temp[4]=(pr[0][4]+pr[1][4]+pr[2][4]+pr[3][4]+pr[4][4])*filter1;	
				
				//BW_temp[0]=(pBW0[0]+pBW1[0]+pBW2[0]+pBW3[0]+pBW4[0])*filter1;
				//BW_temp[1]=(pBW0[1]+pBW1[1]+pBW2[1]+pBW3[1]+pBW4[1])*filter1;
				//BW_temp[2]=(pBW0[2]+pBW1[2]+pBW2[2]+pBW3[2]+pBW4[2])*filter1;
				//BW_temp[3]=(pBW0[3]+pBW1[3]+pBW2[3]+pBW3[3]+pBW4[3])*filter1;
				//BW_temp[4]=(pBW0[4]+pBW1[4]+pBW2[4]+pBW3[4]+pBW4[4])*filter1;
				
				flag=0;
			}
			else
			{
				k=4+j;
				BW_temp[0]=BW_temp[1];
				BW_temp[1]=BW_temp[2];
				BW_temp[2]=BW_temp[3];
				BW_temp[3]=BW_temp[4];
				//BW_temp[4]=(pBW0[k]+pBW1[k]+pBW2[k]+pBW3[k]+pBW4[k])*filter1;
				BW_temp[4]=(pr[0][k]+pr[1][k]+pr[2][k]+pr[3][k]+pr[4][k])*filter1;	
			}
			pBWResult[j]=BW_temp[0]+BW_temp[1]+BW_temp[2]+BW_temp[3]+BW_temp[4];
			

			//获取减去背景后的残差图
			//if(pr[2][2+j]>BW_restlt[i][j])
				paverage[j]=(short )(pr[2][2+j]-pBWResult[j]);
				//paverage[j]=(short )(pBW2[2+j]-pBWResult[j]);
			//else
			{
				//BW_average[i][j]=0;
			}			
			sum_average += paverage[j];
			sum2_square += (paverage[j]*paverage[j]);
		}
	}
	//cycles64= TSC_count_cycle64_from(cycles64);
	//printf("113 consumes %lld cycles64\n", cycles64);
	
	aveGray = sum_average/(N1_result*M1_result);
	stdGray = (sum2_square/(N1_result*M1_result)-aveGray*aveGray);
	stdGray = sqrt(stdGray);
	threshold = stdGray*4;

	//获取BW图
#ifdef 0
	cycles64=_itoll(TSCH, TSCL);
	printf("111 consumes %lld cycles64\n", cycles64);
	for(i = 0;i<N1_result;i++)
	{
		for(j = 0;j<M1_result;j++)
		{
			if(BW_average[i][j]<threshold)
			{
				
				BW_average[i][j]=0;
			}
			else
			{
				BW_average[i][j]=1;
			}			
		}
	}
	cycles64=_itoll(TSCH, TSCL);
	printf("112 consumes %lld cycles64\n", cycles64);
#endif /* 0 */


	//cycles64=_itoll(TSCH, TSCL);
	//printf("111 consumes %lld cycles64\n", cycles64);
	for(i = 0;i<N1_result;i++)
	{
		paverage = (short*)&BW_average[i] ;
		for(j = 0;j<M1_result;j++)
		{
			if(paverage[j]>threshold)
			{				
				paverage[j]=1;
			}
			else
			{
				paverage[j]=0;
			}			
		}
	}
	//cycles64=_itoll(TSCH, TSCL);
	//printf("112 consumes %lld cycles64\n", cycles64);
	
	//(1)Computer numRuns  
#if 0
	cycles64=_itoll(TSCH, TSCL);
	printf("113 consumes %lld cycles64\n", cycles64);
	for (i = 0; i < N1_result; i++)   //按行遍历  
	{  
		pr1 = (short*)&BW_average[i] ;	//第row行的头指针  
  
		if (pr1[0] != 0)  
			numRuns++;		  //本行的第一个元素就是一个Run的头 Run的数目+1  
  
		for (j = 1; j < M1_result; j++) {  
			if ((pr1[j] != 0) && (pr1[j-1] == 0))	//01的模式就是Run  
				numRuns++;						  //Run的总数+1  
		}  
	}  
	cycles64=_itoll(TSCH, TSCL);
	printf("114 consumes %lld cycles64\n", cycles64);
#endif /* 0 */
	
	//return &BW_average[0][0];
	*result = &BW_average[0][0];
}

//使用pBW1代替pr[5]:使用一维数组代替二维数组
inline void imfilter_mex1(unsigned short* BW, short** result)
{
	unsigned int i , j ;
	unsigned int BW_temp[5];
	unsigned int k,flag; 
	float filter1=0.04;;//flag每行第一列判断标志
	float sum_average=0.0;
	float sum2_square=0.0;
	float aveGray,stdGray,threshold;
	unsigned short* pr[5];
	unsigned long long cycles64;
	unsigned short* pBW0;
	unsigned short* pBW1;
	unsigned short* pBW2;
	unsigned short* pBW3;
	unsigned short* pBW4;
	unsigned short* pBWResult;
	short* paverage = NULL;

	short* pr1 = NULL; 
    int numRuns = 0; 

	//获取模板系数
#ifdef 0
	for(i = 0;i<5;i++)
	{
		for(j = 0;j<5;j++)
		{
			filter[i][j]=0.04;
		}
	}	
#endif /* 0 */
	filter1 =0.04;

	//按行遍历  
	//cycles64=_itoll(TSCH, TSCL);
	

	pBW1 = (unsigned short*)&BW[0] ;    //第row行的头指针  
	pBW2 = (unsigned short*)&BW[480] ;    //第row行的头指针 
	pBW3 = (unsigned short*)&BW[960] ;    //第row行的头指针 
	pBW4 = (unsigned short*)&BW[1440] ;    //第row行的头指针 
	for(i = 0;i<N1_result;i++)
	{
		flag=1;
		pBWResult= (unsigned short*)&BW_restlt[i] ;
		paverage = (short*)&BW_average[i] ;
		
		//pr[0] = &BW[i*M1] ;    //第row行的头指针  
		//pr[1] = &BW[(i+1)*M1] ;    //第row行的头指针 
		//pr[2] = &BW[(i+2)*M1] ;    //第row行的头指针 
		//pr[3] = &BW[(i+3)*M1] ;    //第row行的头指针 
		//pr[4] = &BW[(i+4)*M1] ;    //第row行的头指针 

		pBW0 = pBW1;    //第row行的头指针  
		pBW1 = pBW2 ;    //第row行的头指针 
		pBW2 = pBW3 ;    //第row行的头指针 
		pBW3 = pBW4 ;    //第row行的头指针 
		pBW4 = (unsigned short*)&BW[(i+4)*M1] ;    //第row行的头指针 
		
		for(j = 0;j<M1_result;j++)
		{
			if(flag==1)
			{
				
				//BW_temp[0]=(pr[0][0]+pr[1][0]+pr[2][0]+pr[3][0]+pr[4][0])*filter1;
				//BW_temp[1]=(pr[0][1]+pr[1][1]+pr[2][1]+pr[3][1]+pr[4][1])*filter1;
				//BW_temp[2]=(pr[0][2]+pr[1][2]+pr[2][2]+pr[3][2]+pr[4][2])*filter1;
				//BW_temp[3]=(pr[0][3]+pr[1][3]+pr[2][3]+pr[3][3]+pr[4][3])*filter1;
				//BW_temp[4]=(pr[0][4]+pr[1][4]+pr[2][4]+pr[3][4]+pr[4][4])*filter1;	

				BW_temp[0]=(pBW0[0]+pBW1[0]+pBW2[0]+pBW3[0]+pBW4[0])*filter1;
				BW_temp[1]=(pBW0[1]+pBW1[1]+pBW2[1]+pBW3[1]+pBW4[1])*filter1;
				BW_temp[2]=(pBW0[2]+pBW1[2]+pBW2[2]+pBW3[2]+pBW4[2])*filter1;
				BW_temp[3]=(pBW0[3]+pBW1[3]+pBW2[3]+pBW3[3]+pBW4[3])*filter1;
				BW_temp[4]=(pBW0[4]+pBW1[4]+pBW2[4]+pBW3[4]+pBW4[4])*filter1;

				//BW_temp[0]=(pBW0[0]+pBW1[0]+pBW2[0]+pBW3[0]+pBW4[0]);
				//BW_temp[1]=(pBW0[1]+pBW1[1]+pBW2[1]+pBW3[1]+pBW4[1]);
				//BW_temp[2]=(pBW0[2]+pBW1[2]+pBW2[2]+pBW3[2]+pBW4[2]);
				//BW_temp[3]=(pBW0[3]+pBW1[3]+pBW2[3]+pBW3[3]+pBW4[3]);
				//BW_temp[4]=(pBW0[4]+pBW1[4]+pBW2[4]+pBW3[4]+pBW4[4]);
				
				flag=0;
			}
			else
			{
				k=4+j;					
				BW_temp[0]=BW_temp[1];
				BW_temp[1]=BW_temp[2];
				BW_temp[2]=BW_temp[3];
				BW_temp[3]=BW_temp[4];
				BW_temp[4]=(pBW0[k]+pBW1[k]+pBW2[k]+pBW3[k]+pBW4[k])*filter1;
				//BW_temp[4]=(pBW0[k]+pBW1[k]+pBW2[k]+pBW3[k]+pBW4[k]);
			}
			pBWResult[j]=(BW_temp[0]+BW_temp[1]+BW_temp[2]+BW_temp[3]+BW_temp[4]);

			
			//获取减去背景后的残差图
			//if(pr[2][2+j]>BW_restlt[i][j])
				//paverage[j]=(short )(pr[2][2+j]-pBWResult[j]);
				paverage[j]=(short)(pBW2[2+j]-pBWResult[j]);
			//else
			//{
				//BW_average[i][j]=0;
			//}			
			sum_average += (float)paverage[j];
			sum2_square += (float)(paverage[j]*paverage[j]);
		}
	}
	//cycles64= TSC_count_cycle64_from(cycles64);
	//printf("113 consumes %lld cycles64\n", cycles64);
	
	//aveGray = sum_average/(N1_result*M1_result);
	//stdGray = (sum2_square/(N1_result*M1_result)-aveGray*aveGray);
	aveGray = sum_average/131376;
	stdGray = (sum2_square/131376-aveGray*aveGray);
	stdGray = sqrt(stdGray);
	threshold = stdGray*4;

	//获取BW图
#ifdef 0
	cycles64=_itoll(TSCH, TSCL);
	printf("111 consumes %lld cycles64\n", cycles64);
	for(i = 0;i<N1_result;i++)
	{
		for(j = 0;j<M1_result;j++)
		{
			if(BW_average[i][j]<threshold)
			{
				
				BW_average[i][j]=0;
			}
			else
			{
				BW_average[i][j]=1;
			}			
		}
	}
	cycles64=_itoll(TSCH, TSCL);
	printf("112 consumes %lld cycles64\n", cycles64);
#endif /* 0 */


	//cycles64=_itoll(TSCH, TSCL);
	//printf("111 consumes %lld cycles64\n", cycles64);
	for(i = 0;i<N1_result;i++)
	{
		paverage = (short*)&BW_average[i] ;
		for(j = 0;j<M1_result;j++)
		{
			if(paverage[j]>threshold)
			{				
				paverage[j]=1;
			}
			else
			{
				paverage[j]=0;
			}			
		}
	}
	//cycles64= TSC_count_cycle64_from(cycles64);
	//printf("114 consumes %lld cycles64\n", cycles64);
	
	//(1)Computer numRuns  
#if 0
	cycles64=_itoll(TSCH, TSCL);
	printf("113 consumes %lld cycles64\n", cycles64);
	for (i = 0; i < N1_result; i++)   //按行遍历  
	{  
		pr1 = (short*)&BW_average[i] ;	//第row行的头指针  
  
		if (pr1[0] != 0)  
			numRuns++;		  //本行的第一个元素就是一个Run的头 Run的数目+1  
  
		for (j = 1; j < M1_result; j++) {  
			if ((pr1[j] != 0) && (pr1[j-1] == 0))	//01的模式就是Run  
				numRuns++;						  //Run的总数+1  
		}  
	}  
	cycles64=_itoll(TSCH, TSCL);
	printf("114 consumes %lld cycles64\n", cycles64);
#endif /* 0 */
	
	//return &BW_average[0][0];
	*result = &BW_average[0][0];

}

//使用pBW1代替pr[5]:使用一维数组代替二维数组
inline void imfilter_mex1_1(unsigned short* BW, short** result)
{
	unsigned int i , j ;
	unsigned int BW_temp[5];
	unsigned int k,flag; 
	float filter1=0.04;;//flag每行第一列判断标志
	float sum_average=0.0;
	float sum2_square=0.0;
	float aveGray,stdGray,threshold;
	unsigned short* pr[5];
	unsigned long long cycles64;
	unsigned short* pBW0;
	unsigned short* pBW1;
	unsigned short* pBW2;
	unsigned short* pBW3;
	unsigned short* pBW4;
	unsigned short* pBWResult;
	short* paverage = NULL;

	short* pr1 = NULL; 
    int numRuns = 0; 

	//获取模板系数
#ifdef 0
	for(i = 0;i<5;i++)
	{
		for(j = 0;j<5;j++)
		{
			filter[i][j]=0.04;
		}
	}	
#endif /* 0 */
	filter1 =0.04;

	//按行遍历  
	//cycles64=_itoll(TSCH, TSCL);
	

	pBW1 = (unsigned short*)&BW[0] ;    //第row行的头指针  
	pBW2 = (unsigned short*)&BW[480] ;    //第row行的头指针 
	pBW3 = (unsigned short*)&BW[960] ;    //第row行的头指针 
	pBW4 = (unsigned short*)&BW[1440] ;    //第row行的头指针 
	for(i = 0;i<N1_result;i++)
	{
		flag=1;
		pBWResult= (unsigned short*)&BW_restlt[i] ;
		paverage = (short*)&BW_average[i] ;
		
		//pr[0] = &BW[i*M1] ;    //第row行的头指针  
		//pr[1] = &BW[(i+1)*M1] ;    //第row行的头指针 
		//pr[2] = &BW[(i+2)*M1] ;    //第row行的头指针 
		//pr[3] = &BW[(i+3)*M1] ;    //第row行的头指针 
		//pr[4] = &BW[(i+4)*M1] ;    //第row行的头指针 

		pBW0 = pBW1;    //第row行的头指针  
		pBW1 = pBW2 ;    //第row行的头指针 
		pBW2 = pBW3 ;    //第row行的头指针 
		pBW3 = pBW4 ;    //第row行的头指针 
		pBW4 = (unsigned short*)&BW[(i+4)*M1] ;    //第row行的头指针 
		
		for(j = 0;j<M1_result;j++)
		{
			if(flag==1)
			{
				
				//BW_temp[0]=(pr[0][0]+pr[1][0]+pr[2][0]+pr[3][0]+pr[4][0])*filter1;
				//BW_temp[1]=(pr[0][1]+pr[1][1]+pr[2][1]+pr[3][1]+pr[4][1])*filter1;
				//BW_temp[2]=(pr[0][2]+pr[1][2]+pr[2][2]+pr[3][2]+pr[4][2])*filter1;
				//BW_temp[3]=(pr[0][3]+pr[1][3]+pr[2][3]+pr[3][3]+pr[4][3])*filter1;
				//BW_temp[4]=(pr[0][4]+pr[1][4]+pr[2][4]+pr[3][4]+pr[4][4])*filter1;	

				BW_temp[0]=(pBW0[0]+pBW1[0]+pBW2[0]+pBW3[0]+pBW4[0])*filter1;
				BW_temp[1]=(pBW0[1]+pBW1[1]+pBW2[1]+pBW3[1]+pBW4[1])*filter1;
				BW_temp[2]=(pBW0[2]+pBW1[2]+pBW2[2]+pBW3[2]+pBW4[2])*filter1;
				BW_temp[3]=(pBW0[3]+pBW1[3]+pBW2[3]+pBW3[3]+pBW4[3])*filter1;
				BW_temp[4]=(pBW0[4]+pBW1[4]+pBW2[4]+pBW3[4]+pBW4[4])*filter1;

				//BW_temp[0]=(pBW0[0]+pBW1[0]+pBW2[0]+pBW3[0]+pBW4[0]);
				//BW_temp[1]=(pBW0[1]+pBW1[1]+pBW2[1]+pBW3[1]+pBW4[1]);
				//BW_temp[2]=(pBW0[2]+pBW1[2]+pBW2[2]+pBW3[2]+pBW4[2]);
				//BW_temp[3]=(pBW0[3]+pBW1[3]+pBW2[3]+pBW3[3]+pBW4[3]);
				//BW_temp[4]=(pBW0[4]+pBW1[4]+pBW2[4]+pBW3[4]+pBW4[4]);
				
				flag=0;
			}
			else
			{
				k=4+j;					
				BW_temp[0]=BW_temp[1];
				BW_temp[1]=BW_temp[2];
				BW_temp[2]=BW_temp[3];
				BW_temp[3]=BW_temp[4];
				BW_temp[4]=(pBW0[k]+pBW1[k]+pBW2[k]+pBW3[k]+pBW4[k])*filter1;
				//BW_temp[4]=(pBW0[k]+pBW1[k]+pBW2[k]+pBW3[k]+pBW4[k]);
			}
			pBWResult[j]=(BW_temp[0]+BW_temp[1]+BW_temp[2]+BW_temp[3]+BW_temp[4]);

			
			//获取减去背景后的残差图
			//if(pr[2][2+j]>BW_restlt[i][j])
				//paverage[j]=(short )(pr[2][2+j]-pBWResult[j]);
				paverage[j]=(short)(pBW2[2+j]-pBWResult[j]);
			//else
			//{
				//BW_average[i][j]=0;
			//}			
			sum_average += (float)paverage[j];
			sum2_square += (float)(paverage[j]*paverage[j]);
		}
	}
	//cycles64= TSC_count_cycle64_from(cycles64);
	//printf("113 consumes %lld cycles64\n", cycles64);
	
	//aveGray = sum_average/(N1_result*M1_result);
	//stdGray = (sum2_square/(N1_result*M1_result)-aveGray*aveGray);
	aveGray = sum_average/131376;
	stdGray = (sum2_square/131376-aveGray*aveGray);
	stdGray = sqrt(stdGray);
	threshold = stdGray*4;

	//获取BW图
#ifdef 0
	cycles64=_itoll(TSCH, TSCL);
	printf("111 consumes %lld cycles64\n", cycles64);
	for(i = 0;i<N1_result;i++)
	{
		for(j = 0;j<M1_result;j++)
		{
			if(BW_average[i][j]<threshold)
			{
				
				BW_average[i][j]=0;
			}
			else
			{
				BW_average[i][j]=1;
			}			
		}
	}
	cycles64=_itoll(TSCH, TSCL);
	printf("112 consumes %lld cycles64\n", cycles64);
#endif /* 0 */


	//cycles64=_itoll(TSCH, TSCL);
	//printf("111 consumes %lld cycles64\n", cycles64);
	for(i = 0;i<N1_result;i++)
	{
		paverage = (short*)&BW_average[i] ;
		for(j = 0;j<M1_result;j++)
		{
			if(paverage[j]>threshold)
			{				
				paverage[j]=1;
			}
			else
			{
				paverage[j]=0;
			}			
		}
	}
	//cycles64= TSC_count_cycle64_from(cycles64);
	//printf("114 consumes %lld cycles64\n", cycles64);
	
	//(1)Computer numRuns  
#if 0
	cycles64=_itoll(TSCH, TSCL);
	printf("113 consumes %lld cycles64\n", cycles64);
	for (i = 0; i < N1_result; i++)   //按行遍历  
	{  
		pr1 = (short*)&BW_average[i] ;	//第row行的头指针  
  
		if (pr1[0] != 0)  
			numRuns++;		  //本行的第一个元素就是一个Run的头 Run的数目+1  
  
		for (j = 1; j < M1_result; j++) {  
			if ((pr1[j] != 0) && (pr1[j-1] == 0))	//01的模式就是Run  
				numRuns++;						  //Run的总数+1  
		}  
	}  
	cycles64=_itoll(TSCH, TSCL);
	printf("114 consumes %lld cycles64\n", cycles64);
#endif /* 0 */
	
	//return &BW_average[0][0];
	*result = &BW_average[0][0];

}

inline void imfilter_mex_1(unsigned short* BW, short** result )
{
	unsigned int i , j ;
	unsigned int BW_temp[5];
	unsigned int k,flag; 
	float filter1=0.04;;//flag每行第一列判断标志
	float sum_average=0.0;
	float sum2_square=0.0;
	float aveGray,stdGray,threshold;
	unsigned short* pr[5];
	unsigned long long cycles64;
	unsigned short* pBW0;
	unsigned short* pBW1;
	unsigned short* pBW2;
	unsigned short* pBW3;
	unsigned short* pBW4;
	unsigned short* pBWResult;
	short* paverage = NULL;

	short* pr1 = NULL; 
    int numRuns = 0; 

	//获取模板系数
#ifdef 0
	for(i = 0;i<5;i++)
	{
		for(j = 0;j<5;j++)
		{
			filter[i][j]=0.04;
		}
	}	
#endif /* 0 */
	filter1 =0.04;

	//按行遍历  
	//cycles64=_itoll(TSCH, TSCL);	

	//pBW1 = (unsigned short*)&BW[0] ;    //第row行的头指针  
	//pBW2 = (unsigned short*)&BW[480] ;    //第row行的头指针 
	//pBW3 = (unsigned short*)&BW[960] ;    //第row行的头指针 
	//pBW4 = (unsigned short*)&BW[1440] ;    //第row行的头指针

	//pr[0] = &BW[i*M1] ;    //第row行的头指针  
	pr[1] = &BW[0] ;    //第row行的头指针 
	pr[2] = &BW[480] ;    //第row行的头指针 
	pr[3] = &BW[960] ;    //第row行的头指针 
	pr[4] = &BW[1440] ;    //第row行的头指针 
	for(i = 0;i<N1_result;i++)
	{
		flag=1;
		pBWResult= (unsigned short*)&BW_restlt[i] ;
		paverage = (short*)&BW_average[i] ;
		
		//pr[0] = &BW[i*M1] ;    //第row行的头指针  
		//pr[1] = &BW[(i+1)*M1] ;    //第row行的头指针 
		//pr[2] = &BW[(i+2)*M1] ;    //第row行的头指针 
		//pr[3] = &BW[(i+3)*M1] ;    //第row行的头指针 
		//pr[4] = &BW[(i+4)*M1] ;    //第row行的头指针 

		//pBW0 = pBW1;    //第row行的头指针  
		//pBW1 = pBW2 ;    //第row行的头指针 
		//pBW2 = pBW3 ;    //第row行的头指针 
		//pBW3 = pBW4 ;    //第row行的头指针 
		//pBW4 = (unsigned short*)&BW[(i+4)*M1] ;    //第row行的头指针 
		
		pr[0] = pr[1];    //第row行的头指针  
		pr[1] = pr[2] ;    //第row行的头指针 
		pr[2] = pr[3];    //第row行的头指针 
		pr[3] = pr[4];    //第row行的头指针 
		pr[4] = &BW[(i+4)*M1] ;    //第row行的头指针 
		
		
		for(j = 0;j<M1_result;j++)
		{
			if(flag==1)
			{				
				BW_temp[0]=(pr[0][0]+pr[1][0]+pr[2][0]+pr[3][0]+pr[4][0])*filter1;
				BW_temp[1]=(pr[0][1]+pr[1][1]+pr[2][1]+pr[3][1]+pr[4][1])*filter1;
				BW_temp[2]=(pr[0][2]+pr[1][2]+pr[2][2]+pr[3][2]+pr[4][2])*filter1;
				BW_temp[3]=(pr[0][3]+pr[1][3]+pr[2][3]+pr[3][3]+pr[4][3])*filter1;
				BW_temp[4]=(pr[0][4]+pr[1][4]+pr[2][4]+pr[3][4]+pr[4][4])*filter1;	
				
				//BW_temp[0]=(pBW0[0]+pBW1[0]+pBW2[0]+pBW3[0]+pBW4[0])*filter1;
				//BW_temp[1]=(pBW0[1]+pBW1[1]+pBW2[1]+pBW3[1]+pBW4[1])*filter1;
				//BW_temp[2]=(pBW0[2]+pBW1[2]+pBW2[2]+pBW3[2]+pBW4[2])*filter1;
				//BW_temp[3]=(pBW0[3]+pBW1[3]+pBW2[3]+pBW3[3]+pBW4[3])*filter1;
				//BW_temp[4]=(pBW0[4]+pBW1[4]+pBW2[4]+pBW3[4]+pBW4[4])*filter1;
				
				flag=0;
			}
			else
			{
				k=4+j;
				BW_temp[0]=BW_temp[1];
				BW_temp[1]=BW_temp[2];
				BW_temp[2]=BW_temp[3];
				BW_temp[3]=BW_temp[4];
				//BW_temp[4]=(pBW0[k]+pBW1[k]+pBW2[k]+pBW3[k]+pBW4[k])*filter1;
				BW_temp[4]=(pr[0][k]+pr[1][k]+pr[2][k]+pr[3][k]+pr[4][k])*filter1;	
			}
			pBWResult[j]=BW_temp[0]+BW_temp[1]+BW_temp[2]+BW_temp[3]+BW_temp[4];
			

			//获取减去背景后的残差图
			//if(pr[2][2+j]>BW_restlt[i][j])
				paverage[j]=(short )(pr[2][2+j]-pBWResult[j]);
				//paverage[j]=(short )(pBW2[2+j]-pBWResult[j]);
			//else
			{
				//BW_average[i][j]=0;
			}			
			sum_average += paverage[j];
			sum2_square += (paverage[j]*paverage[j]);
		}
	}
	//cycles64= TSC_count_cycle64_from(cycles64);
	//printf("113 consumes %lld cycles64\n", cycles64);
	
	aveGray = sum_average/(N1_result*M1_result);
	stdGray = (sum2_square/(N1_result*M1_result)-aveGray*aveGray);
	stdGray = sqrt(stdGray);
	threshold = stdGray*4;

	//获取BW图
#ifdef 0
	cycles64=_itoll(TSCH, TSCL);
	printf("111 consumes %lld cycles64\n", cycles64);
	for(i = 0;i<N1_result;i++)
	{
		for(j = 0;j<M1_result;j++)
		{
			if(BW_average[i][j]<threshold)
			{
				
				BW_average[i][j]=0;
			}
			else
			{
				BW_average[i][j]=1;
			}			
		}
	}
	cycles64=_itoll(TSCH, TSCL);
	printf("112 consumes %lld cycles64\n", cycles64);
#endif /* 0 */


	//cycles64=_itoll(TSCH, TSCL);
	//printf("111 consumes %lld cycles64\n", cycles64);
	for(i = 0;i<N1_result;i++)
	{
		paverage = (short*)&BW_average[i] ;
		for(j = 0;j<M1_result;j++)
		{
			if(paverage[j]>threshold)
			{				
				paverage[j]=1;
			}
			else
			{
				paverage[j]=0;
			}			
		}
	}
	//cycles64=_itoll(TSCH, TSCL);
	//printf("112 consumes %lld cycles64\n", cycles64);
	
	//(1)Computer numRuns  
#if 0
	cycles64=_itoll(TSCH, TSCL);
	printf("113 consumes %lld cycles64\n", cycles64);
	for (i = 0; i < N1_result; i++)   //按行遍历  
	{  
		pr1 = (short*)&BW_average[i] ;	//第row行的头指针  
  
		if (pr1[0] != 0)  
			numRuns++;		  //本行的第一个元素就是一个Run的头 Run的数目+1  
  
		for (j = 1; j < M1_result; j++) {  
			if ((pr1[j] != 0) && (pr1[j-1] == 0))	//01的模式就是Run  
				numRuns++;						  //Run的总数+1  
		}  
	}  
	cycles64=_itoll(TSCH, TSCL);
	printf("114 consumes %lld cycles64\n", cycles64);
#endif /* 0 */
	
	//return &BW_average[0][0];
	*result = &BW_average[0][0];
}

int main_imagetarget() 
{  
    Uint32 ret=0;
	Uint32 i, j;  
    RunNode_t* pRunNode = NULL;  
	FILE*       fp;
	FILE*       fp1;
	
    int RunNum = 0;  
    SparseInfoExtractCondition_t ExtractCondition;  
	short* result;
	int labels = 0;

	/* for 319 project debug */
	Uint32 cycles;
	unsigned long long cycles64;
	
	time_t rawtime;
	struct tm * timeinfo;

	/***************************************
	* 文件读写操作
	
	***************************************/
#if 0
	printf("获取文件image数据\r\n"); 
	fp = fopen("6283.ADAT", "rb"); 
	if (fp == NULL)    
	{        
		printf("Error in opening input file!!!\r\n");        
		return ret;    
	}
	//跳过文件头960个字节
	fseek(fp,960,0);
	fread(BW, 2, 480*280,  fp);	
#endif /* 0 */

	//验证文件写入是否正确
#if 0
	fp1 = fopen("2.ADAT", "wb+");    
	if (fp1 == NULL)    
	{        
		printf("Error in opening input file!!!\r\n");        
		return;    
	}
	fwrite(&BW[0][0], 2, 480*280, fp1);	
#endif /* 0 */

	/*对数据进行滤波处理*/
	//Filter the image
    //matlab计算方法:plhs[0] = filter.evaluate();

	/* for 319 project debug */
	//测试程序运行时间
	//cycles= TSCL;	
	//TSCL=0;TSCH=0;
	cycles64=_itoll(TSCH, TSCL);
	printf("001 consumes %lld cycles64\n", cycles64);

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );	
	printf ( "%d:%d\n",timeinfo->tm_min,timeinfo->tm_sec);
	for(i=0;i<1000;i++)
	{
		//cycles64=_itoll(TSCH, TSCL);	
		
		imfilter_mex((unsigned short*)BW,&result);
		//cycles64= TSC_count_cycle64_from(cycles64);
		//printf("111 consumes %lld cycles64\n", cycles64);

		/*对数据进行目标提取*/
		ExtractCondition.minarea = 10;  
	    ExtractCondition.minpixDiff = 10;  
	    ExtractCondition.mode = 1; 
		//cycles64=_itoll(TSCH, TSCL);
	    BWCreateRunNodeInfo1((short*)result ,M1_result ,N1_result ,&ExtractCondition, &labels ,&pRunNode, &RunNum);  
	  	//cycles64= TSC_count_cycle64_from(cycles64);
		//printf("112 consumes %lld cycles64\n", cycles64);
#if 0
	    printf("\n\n");  
	    printf("(5) 打印最后RunNode信息\n");  
	    printf("  sc  ec   r   label   area\n");  
		for(i=0;i<RunNum;i++)  
	    {  
	        printf("%4d ",pRunNode[i].sc);  
	        printf("%4d ",pRunNode[i].ec);  
	        printf("%4d ",pRunNode[i].r);  
	        printf("%4d ",pRunNode[i].label);  
			printf("%4d ",pRunNode[i].area);
	        printf("\n");  
	    }
#endif /* 0 */
		//cycles64=_itoll(TSCH, TSCL);
		UpadateBWData((short*)result,M1_result,pRunNode,RunNum,labels); 
		//cycles= TSC_count_cycle_from(cycles);		
		//printf("consumes %5d cycles\n", cycles);

		//cycles64= TSC_count_cycle64_from(cycles64);
		//printf("end consumes %lld cycles64\n", cycles64);
	}
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );	
	printf ( "%d:%d\n",timeinfo->tm_min,timeinfo->tm_sec);
	
	cycles64= TSC_count_cycle64_from(cycles64);
	printf("end consumes %lld cycles64\n", cycles64);
	ret=0;
	return ret;
}
#endif /* 0 */


/* for 319 project debug */
#if 0
int core1_test0()
{
	int ret=0;

	/*移植imagetarget工程到C6678上,进行测试*/
	ret=main_imagetarget();

	return ret;
}
#endif
/*****************************************************************************
 Prototype    : MSU_Core1_Image
 Description  : MSU image module
 Input        : None
 Output       : None
 Return Value : None
 
  History        :
  1.Date         : 2016/01/08
    Author       : sxf
    Modification : Created function
 Ps:
WritebackCache 限制uiByteCount最大256k
MSU_Image 最后在core的中断函数中处理   
测试函数运行效率，使用cycles(max 4s) and cycles64
*****************************************************************************/
unsigned int MSU_Core1_Image()
{
	Uint32 eventFlag;
	Uint32 uicircles;
	Uint32 uiStartIdx;
	Uint32 i;
	Uint32 j;
	int ret;

	Uint32 uiEntryCount;
	Uint32 uiTotalFailCount=0;
	Uint32 uiByteCount;
	HostPacketDescriptor * hostDescriptor;
	Uint32 * uipDstBufPtr;
	DMA_MEM_Tran_Config   DMA_transfer_cfg;

	/* for 319 project debug */
	unsigned long long cycles64;
	//处理函数

	//测试函数，项目调试完后去掉
	/* for 319 project debug */
	ret=0;

	//while(1)
	//ret=core1_test0();
	
	/*poll the packet descriptor in destination queue*/
	{
		/* for 319 project debug */
		cycles64=_itoll(TSCH, TSCL);


		
#if 0	
		uiEntryCount= gpQueueStatusConfigRegs[CORE1_PKT_DMA_DST_Q0].REG_A_EntryCount;

		/* for 319 project debug SL2-LL2*/
		//uiEntryCount= gpQueueStatusConfigRegs[LL2_PKT_DMA_DST_Q0].REG_A_EntryCount;
		
		/*received packet number should equal to channel number 
		since each channel transfer on packet*/
		//uiEntryCount= KeyStone_GetQueueEntryCount(CORE1_PKT_DMA_DST_Q0);
		if(uiEntryCount>0)
		{
			/*check recieved data and recycle descriptors and buffers*/
			hostDescriptor= (HostPacketDescriptor *)KeyStone_queuePop(CORE1_PKT_DMA_DST_Q0);

			/* for 319 project debug SL2-LL2*/
			//hostDescriptor= (HostPacketDescriptor *)KeyStone_queuePop(LL2_PKT_DMA_DST_Q0);

			/*invalid cache before read descriptor RAM*/
			InvalidCache((void *)hostDescriptor, 64);

			/*get the destination buffer*/
			if(Cppi_DescType_HOST==hostDescriptor->type_id)
			{
				uipDstBufPtr= (Uint32 *)hostDescriptor->buffer_ptr;
				uiaDescriptorBufferCore1[0]= (Uint32)hostDescriptor;
				uiByteCount = hostDescriptor->packet_length;
			}
			else
			{
				/*drop the packet*/
				/*descriptor Reclamation*/
				//gpQueueManageVBUSM[RECLAMATION_QUEUE].REG_D_Descriptor= uiaDescriptorBuffer[j];
				gpQueueManageVBUSM[(hostDescriptor->pkt_return_qmgr<<12)|
					hostDescriptor->pkt_return_qnum].REG_D_Descriptor= 
					uiaDescriptorBufferCore1[0];
				printf("MSU_Image ERROR code 000!\r\n");
				
				return;
			}
			/* for 319 project debug */
			printf("MSU_Image get from 0x%8x\r\n", (Uint32)uipDstBufPtr);

			/*invalid old contents of destination buffer in cache before read*/
			InvalidCache((void *)uipDstBufPtr, uiByteCount);

			/* edma copy uipDstBufPtr,进行算法处理*/
			//Core1_Image();

			
			/* for 319 project debug */
			/*compare the destination data against source data*/
			for(i=0; i<uiByteCount/4; i++)
			{
				if(packetBuffer_DDR_Check_core1[i]!= uipDstBufPtr[i])
				{
					//printf("consumes %5d cycles\n", cycles);
					printf("destination data 0x%8x does not match source data 0x%8x at word %d of packet (0x%08x)\n",
						uipDstBufPtr[i], packetBuffer_DDR_Check_core1[i], i,(Uint32)uipDstBufPtr);
					/*drop the packet*/
					/*descriptor Reclamation*/
					//gpQueueManageVBUSM[RECLAMATION_QUEUE].REG_D_Descriptor= uiaDescriptorBuffer[j];
					gpQueueManageVBUSM[(hostDescriptor->pkt_return_qmgr<<12)|
						hostDescriptor->pkt_return_qnum].REG_D_Descriptor= 
						uiaDescriptorBufferCore1[0];

					return;
				}
			}
#endif			


			/*获得pbuf数据传递一个16*4的数组*/
			DMA_transfer_cfg.TC_channel_index=TC_channel_core1_index0;
			DMA_transfer_cfg.uiByteCount=4;
			DMA_transfer_cfg.A_B_CNT=(64<<16)|((DMA_transfer_cfg.uiByteCount)&0xFFFF);//每个像元4个字节byteCount=4;
			DMA_transfer_cfg.SRC_DST_BIDX=(4<<16)|(4);//每个像元4个字节
			DMA_transfer_cfg.SRC_DST_CIDX=(256<<16)|(256);//每个像元4个字节
			DMA_transfer_cfg.CCNT=1;
			uiTotalFailCount=EDMA_MEM_Transfer(uipDstBufPtr, (Uint32 *)(packetBuffer_DDR_EDMA_Check_core1), &DMA_transfer_cfg);
			if (uiTotalFailCount)
	    	{
	    		//printf("!!!Failed Memory Fill Test at %d Units with pattern 0x%16llx with EDMA CC%d TC%d\n",uiTotalFailCount, ulDataPatternTable[j], ((Uint32)TC_channel)>>16, ((Uint32)TC_channel)&0xFFFF);
	    		printf("!!!Failed Memory Fill Test at %d Units\n,uiTotalFailCount");
	    	}
		
			/*进行图像处理*/
			//core1_deal_test1();
			
			/* for 319 project debug ，最终结果选用KeyStone_ReturnQueue(debug 时候不能单步跟踪，不方便查找问题)*/
			/*descriptor Reclamation*/
			//gpQueueManageVBUSM[RECLAMATION_QUEUE].REG_D_Descriptor= uiaDescriptorBuffer[j];
#if 0
			gpQueueManageVBUSM[(hostDescriptor->pkt_return_qmgr<<12)|
				hostDescriptor->pkt_return_qnum].REG_D_Descriptor= 
				uiaDescriptorBufferCore1[0];
#endif

			//KeyStone_ReturnQueue(uiaDescriptorBufferCore1[0]);
		cycles64= TSC_count_cycle64_from(cycles64);
		printf("consumes %lld cycles\n", cycles64);
	}

	/*for 319 project debug:获得pbuf数据传递一个16*4的数组*/
#if 0
	DMA_transfer_cfg.TC_channel_index=TC_channel_core1_index0;
	DMA_transfer_cfg.uiByteCount=4;
	DMA_transfer_cfg.A_B_CNT=(16<<16)|((DMA_transfer_cfg.uiByteCount)&0xFFFF);//每个像元4个字节byteCount=4;
	DMA_transfer_cfg.SRC_DST_BIDX=(4<<16)|(4);//每个像元4个字节
	DMA_transfer_cfg.SRC_DST_CIDX=(64<<16)|(64);//每个像元4个字节
	DMA_transfer_cfg.CCNT=4;
	uiTotalFailCount=EDMA_MEM_Transfer((Uint32 *)packetBuffer_DDR_Check_core1, (Uint32 *)(packetBuffer_DDR_EDMA_Check_core1), &DMA_transfer_cfg);
	if (uiTotalFailCount)
	{
	    //printf("!!!Failed Memory Fill Test at %d Units with pattern 0x%16llx with EDMA CC%d TC%d\n",uiTotalFailCount, ulDataPatternTable[j], ((Uint32)TC_channel)>>16, ((Uint32)TC_channel)&0xFFFF);
	    printf("!!!Failed Memory Fill Test at %d Units\n,uiTotalFailCount");
	}
#endif

	return;

}

