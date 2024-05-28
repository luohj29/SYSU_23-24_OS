#ifndef LIST_H
#define LIST_H

struct ListItem
{
    ListItem *previous;
    ListItem *next;
};

// list ��һ����ͷ�ڵ��˫������
class List
{
public:
    ListItem head;

public:
    // ��ʼ��List
    List();
    // ��ʽ��ʼ��List
    void initialize();
    // ����ListԪ�ظ���
    int size();
    // ����List�Ƿ�Ϊ��
    bool empty();
    // ����ָ��List���һ��Ԫ�ص�ָ��
    // ��û�У��򷵻�nullptr
    ListItem *back();
    // ��һ��Ԫ�ؼ��뵽List�Ľ�β
    void push_back(ListItem *itemPtr);
    // ɾ��List���һ��Ԫ��
    void pop_back();
    // ����ָ��List��һ��Ԫ�ص�ָ��
    // ��û�У��򷵻�nullptr
    ListItem *front();
    // ��һ��Ԫ�ؼ��뵽List��ͷ��
    void push_front(ListItem *itemPtr);
    // ɾ��List��һ��Ԫ��
    void pop_front();
    // ��һ��Ԫ�ز��뵽pos��λ�ô�
    void insert(int pos, ListItem *itemPtr);
    // ɾ��posλ�ô���Ԫ��
    void erase(int pos);
    void erase(ListItem *itemPtr);
    // ����ָ��posλ�ô���Ԫ�ص�ָ��
    ListItem *at(int pos);
    // ���ظ���Ԫ����List�е����
    int find(ListItem *itemPtr);
};

#endif