#ifndef STDLIB_H
#define STDLIB_H
#include"os_type.h"

/*
 * ��������������ֵ��
 */
template<typename T>
void swap(T &x, T &y);
/*
 * ��һ���Ǹ�����ת��Ϊָ�����Ʊ�ʾ���ַ�����
 * num: ��ת���ķǸ�������
 * mod: ���ơ�
 * numStr: ����ת������ַ��������У�numStr[0]�������num�ĸ�λ���֣��Դ����ơ�
 */

void itos(char *numStr, uint32 num, uint32 mod);
// ����memory��ʼ��length���ֽ�����Ϊvalue
void memset(void *memory, char value, int length);
#endif
