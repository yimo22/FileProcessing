#include <stdio.h>
//필요하면 header file 추가 가능
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define RECORDSIZE 250
char buff[RECORDSIZE];
int main(int argc, char **argv)
{
	//
	// 사용자로부터 입력 받은 레코드 수만큼의 레코드를 레코드 파일에 저장하는 코드를 구현
	//
	int NumOfRecord = 0;
	FILE *fp;
	if ((fp = fopen(argv[2], "w")) == NULL)
	{
		perror("error");
		exit(1);
	}

	// Write Header Record with Binary Interger Mode
	// Header file must be 4 bytes.

	NumOfRecord = atoi(argv[1]);
	fwrite(&NumOfRecord, sizeof(int), 1, fp);

	srand((unsigned int)time(NULL));
	for (int i = 0; i < atoi(argv[1]); i++)
	{
		/* 'a' ~ 'z' 의 임의의 문자 */
		for (int j = 0; j < RECORDSIZE; j++)
		{
			buff[j] = 'a' + rand() % 26;
		}
		fwrite(buff, sizeof(char), sizeof(buff) / sizeof(char), fp);
	}

	return 0;
}
