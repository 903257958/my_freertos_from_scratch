#include "list.h"

List_t list_test;
ListItem_t list_item1, list_item2, list_item3;

int main()
{
    /* 链表初始化 */
    vListInitialise(&list_test);
    
    /* 链表项初始化 */
    vListInitialiseItem(&list_item1);
    list_item1.xItemValue = 1;
    vListInitialiseItem(&list_item2);
    list_item2.xItemValue = 2;
    vListInitialiseItem(&list_item3);
    list_item3.xItemValue = 3;
    
    /* 升序插入 */
    vListInsert(&list_test, &list_item2);
    vListInsert(&list_test, &list_item1);
    vListInsert(&list_test, &list_item3);
    
    for (;;)
    {
        
    }
}
