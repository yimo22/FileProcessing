#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
//�ʿ��ϸ� header file �߰� ����
#include <sys/time.h> // gettimeofday()

#define RECORDSIZE 250
#define SUFFLE_NUM 10000 // �� ���� ������� ������ �� ����.
#define MICROSEC_PER_SEC 1000000
#define HEADERSIZE 4

char buff[RECORDSIZE];

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// �ʿ��� �Լ��� ������ �� �߰��� �� ����.

int main(int argc, char **argv)
{
	int *read_order_list;
	int num_of_records; // ���ڵ� ���Ͽ� ����Ǿ� �ִ� ��ü ���ڵ��� ��
	struct timeval start, end;
	int dur;

	FILE *fp = NULL;
	if ((fp = fopen(argv[1], "r")) == NULL)
	{
		perror("error");
		exit(1);
	}

	// Get num_of_records from header
	if (fread(&num_of_records, sizeof(char), 4, fp) != 4)
	{
		printf("error");
		exit(1);
	}
	read_order_list = (int *)malloc(sizeof(int) * num_of_records);
	printf("#records: %d", num_of_records);
	// Time start
	gettimeofday(&start, NULL);

	// �� �Լ��� �����ϸ� 'read_order_list' �迭���� �о�� �� ���ڵ� ��ȣ���� �����Ǿ� �����
	GenRecordSequence(read_order_list, num_of_records);

	//
	// 'read_order_list'�� �̿��Ͽ� random �ϰ� read �� �� �ɸ��� ��ü �ð��� �����ϴ� �ڵ� ����
	//
	for (int i = 0; i < num_of_records; i++)
	{
		int record_num = *(read_order_list + i);
		fseek(fp, record_num * RECORDSIZE + HEADERSIZE, SEEK_SET);
		fread(buff, sizeof(char), 250, fp);
	}

	// Time end
	gettimeofday(&end, NULL);
	dur = (end.tv_sec - start.tv_sec) * MICROSEC_PER_SEC + ((end.tv_usec - start.tv_usec));
	printf("\telapsed_time: %d us\n", dur);

	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for (i = 0; i < n; i++)
	{
		list[i] = i;
	}

	for (i = 0; i < SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}
