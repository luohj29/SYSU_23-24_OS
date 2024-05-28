#include "stdio.h"
#include "os_type.h"

template<typename T>
// ��������������ֵ
void swap(T &x, T &y) {
    T z = x;
    x = y;
    y = z;
}

void itos(char *numStr, uint32 num, uint32 mod) {
    // ֻ��ת��2~26���Ƶ�����
    if (mod < 2 || mod > 26 || num < 0) {
        return;
    }

    uint32 length, temp;

    // ����ת��
    length = 0;
    while(num) {
        temp = num % mod;
        num /= mod;
        // 0~9��Ӧ'0'~'9'��10~25��Ӧ'A'~'Z'
        numStr[length] = temp > 9 ? temp - 10 + 'A' : temp + '0';
        ++length;
    }

    // �ر���num=0�����
    if(!length) {
        numStr[0] = '0';
        ++length;
    }

    // ���ַ�����ת��ʹ��numStr[0]�������num�ĸ�λ����
    for(int i = 0, j = length - 1; i < j; ++i, --j) {
        swap(numStr[i], numStr[j]);
    }
    
    numStr[length] = '\0';
}


void memset(void *memory, char value, int length) // �ڴ��ʼ��
{
    for (int i = 0; i < length; ++i)
    {
        ((char *)memory)[i] = value;  // ��memory��ÿ���ֽڶ���ʼ��Ϊvalue
    }
}
