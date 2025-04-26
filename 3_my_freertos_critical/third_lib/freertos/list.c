#include <stdlib.h>
#include "FreeRTOS.h"
#include "list.h"

/* 链表节点初始化 */
void vListInitialiseItem(ListItem_t *const pxItem)
{
    pxItem->pvContainer = NULL; // 表示节点还未插入任何链表
}

/* 链表根节点初始化 */
void vListInitialise(List_t *const pxList)
{
    /* 将链表索引指针指向尾节点 */
    pxList->pxIndex = (ListItem_t *)&pxList->xListEnd;
    
    /* 初始化链表尾节点 */
    pxList->xListEnd.xItemValue = portMAX_DELAY;    // 尾节点排序值设为最大，确保始终位于链表末端
    pxList->xListEnd.pxNext = (ListItem_t *)&pxList->xListEnd;      // 尾节点自指，形成空链表环
    pxList->xListEnd.pxPrevious = (ListItem_t *)&pxList->xListEnd;  // 尾节点自指，形成空链表环
    
    /* 初始化链表节点计数器的值为0，表示链表为空 */
    pxList->uxNumberOfItems = (UBaseType_t)0U;
}

/* 将节点插入到链表的尾部 */
void vListInsertEnd(List_t *const pxList, ListItem_t *const pxNewListItem)
{
    ListItem_t *const pxIndex = pxList->pxIndex;
    
    /* 将新节点插入pxIndex所指节点的前面 */
    pxNewListItem->pxNext = pxList->pxIndex;
    pxNewListItem->pxPrevious = pxList->pxIndex->pxPrevious;
    pxIndex->pxPrevious->pxNext = pxNewListItem;
    pxIndex->pxPrevious = pxNewListItem;
    
    /* 记录节点所在的链表 */
    pxNewListItem->pvContainer = (void *)pxList;
    
    /* 链表节点计数器++ */
    (pxList->uxNumberOfItems)++;
}

/* 将节点按照升序插入链表 */
void vListInsert(List_t *const pxList, ListItem_t *const pxNewListItem)
{
    ListItem_t *pxIterator;
    const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;
    
    /* 寻找插入位置 */
    if (xValueOfInsertion == portMAX_DELAY)
    {
        /* 如果val为最大值，直接插在尾部 */
        pxIterator = pxList->xListEnd.pxPrevious;
    }
    else
    {
        /* 否则遍历链表，直到下一项的val比新item大 */
        for (   pxIterator = (ListItem_t *)&pxList->xListEnd;
                pxIterator->pxNext->xItemValue <= xValueOfInsertion;
                pxIterator = pxIterator->pxNext)
        {
            /* 空循环，仅移动迭代器指针 */
        }
    }
    
    /* 将新节点插入 */
    pxNewListItem->pxNext = pxIterator->pxNext;
    pxNewListItem->pxNext->pxPrevious = pxNewListItem;
    pxNewListItem->pxPrevious = pxIterator;
    pxIterator->pxNext = pxNewListItem;
    
    /* 记录节点所在的链表 */
    pxNewListItem->pvContainer = (void *)pxList;
    
    /* 链表节点计数器++ */
    (pxList->uxNumberOfItems)++;
}

/* 从链表中移除指定节点并返回剩余节点数 */
UBaseType_t uxListRemove(ListItem_t *const pxItemToRemove)
{
    /* 获取目标节点所属的链表 */
    List_t *const pxList = pxItemToRemove->pvContainer;
    
    /* 移除指定节点 */
    pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
    pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;
    
    /* 调整链表节点索引指针 */
    if (pxList->pxIndex == pxItemToRemove)
    {
        pxList->pxIndex = pxItemToRemove->pxPrevious;
    }
    
    /* 清除节点所属链表标记 */
    pxItemToRemove->pvContainer = NULL;
    
    /* 更新链表节点计数器 */
    (pxList->uxNumberOfItems)--;
    
    /* 返回剩余节点数 */
    return pxList->uxNumberOfItems;
}
