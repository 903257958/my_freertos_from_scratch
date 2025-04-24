#ifndef LIST_H
#define LIST_H

#include "FreeRTOS.h"

/* 链表节点 */
struct xLIST_ITEM
{
    TickType_t xItemValue;          // 用于排序
    struct xLIST_ITEM *pxNext;
    struct xLIST_ITEM *pxPrevious;
    void *pvOwner;                  // 指向拥有该节点的对象，通常是TCB
    void *pvContainer;              // 指向该节点所在的链表
};
typedef struct xLIST_ITEM ListItem_t;

/* 链表精简节点 */
struct xMINI_LIST_ITEM
{
    TickType_t xItemValue;          // 用于排序
    struct xLIST_ITEM *pxNext;
    struct xLIST_ITEM *pxPrevious;
};
typedef struct xMINI_LIST_ITEM MiniListItem_t;

/* 链表根节点 */
typedef struct xLIST
{
    UBaseType_t uxNumberOfItems;    // 节点计数器
    ListItem_t *pxIndex;            // 节点索引指针
    MiniListItem_t xListEnd;        // 尾节点
}List_t;

/* 初始化节点的拥有者 */
#define listSet_LIST_ITEM_OWNER(pxListItem, pxOwner) \
        ((pxListItem)->pvOwner = (void *)(pxOwner))
            
/* 获取节点的拥有者 */
#define listGet_LIST_ITEM_OWNER(pxListItem) \
        ((pxListItem)->pvOwner)

/* 初始化节点排序值 */
#define listSET_LIST_ITEM_VALUE(pxListItem, xValue) \
        ((pxListItem)->xItemValue = (xValue))

/* 获取节点排序值 */
#define listGET_LIST_ITEM_VALUE(pxListItem) \
        ((pxListItem)->xItemValue)

/* 获取链表中第一个节点的排序值 */
#define listGET_ITEM_VALUE_OF_HEAD_ENTRY(pxList) \
        (((pxList)->xListEnd).pxNext->xItemValue)

/* 获取链表入口节点 */
#define listGET_HEAD_ENTRY(pxList) \
        (((pxList)->xListEnd).pxNext)

/* 获取链表节点的下一个节点 */
#define listGET_NEXT(pxListItem) \
        ((pxListItem)->pxNext)

/* 获取链表的最后一个节点 */
#define listGET_END_MARKER(pxList) \
        ((ListItem_t const *)(&((pxList)->xListEnd)))

/* 判断链表是否为空 */
#define listLIST_IS_EMPTY(pxList) \
        ((BaseType_t)((pxList)->uxNumberOfItems == (UBaseType_t)0))

/* 获取链表节点数 */
#define listCURRENT_LIST_LENGTH(pxList) \
        ((pxList)->uxNumberOfItems)

/* 获取链表第一个节点的Owner，即TCB */
#define listGET_OWNER_OF_NEXT_ENTRY(pxTCB, pxList) { \
    List_t *const pxConstList = (pxList); \
    (pxConstList)->pxIndex = (pxConstList)->pxIndex->pxNext; \
    /* 检查是否到达链表末尾，若到达则跳过尾节点 */ \
    if ((void *)(pxConstList)->pxIndex == (void *)&((pxConstList)->xListEnd)) { \
        (pxConstList)->pxIndex = (pxConstList)->pxIndex->pxNext; \
    } \
    (pxTCB) = (pxConstList)->pxIndex->pvOwner; \
}

/* 函数声明 */
void vListInitialiseItem(ListItem_t *const pxItem);
void vListInitialise(List_t *const pxList);
void vListInsertEnd(List_t *const pxList, ListItem_t *const pxNewListItem);
void vListInsert(List_t *const pxList, ListItem_t *const pxNewListItem);
UBaseType_t uxListRemove(ListItem_t *const pxItemToRemove);

#endif
