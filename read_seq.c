#include <stdio.h>
//필요하면 header file 추가 가능
#include <sys/time.h> // gettimeofday()
#include <stdlib.h>
#include <string.h>


#define MICROSEC_PER_SEC 1000000
#define RECORDSIZE 250
char buff[RECORDSIZE];

/* 4byte char 형 배열을 받아 int형 반환 */
int HexToInt(char* arr){

}
int main(int argc, char **argv)
{
	//
	// 레코드 파일로부터 전체 레코들을 순차적으로 read할 때
	// 걸리는 전체 시간을 측정하는 코드 구현
	//
	FILE* fp = NULL;
		int num_of_records; // 레코드 파일에 저장되어 있는 전체 레코드의 수
	if( (fp = fopen(argv[1],"r"))== NULL){
		perror("error");
		exit(1);
	}

	// Reading Header
	if(fread(&num_of_records,sizeof(char),4,fp) == -1){
		printf("error");
		exit(1);
	} 
	printf("#records: %d",num_of_records);
	struct timeval start,end;
	int dur;

	// Time start
	gettimeofday(&start,NULL);

	// Read all record in sequence
	for (int i = 0; i < num_of_records; i++)
	{
		/* Reading Record */
		fread(buff, sizeof(char), 250, fp);
	}

	// Time end
	gettimeofday(&end,NULL);


	// Get Running time
	dur = ( end.tv_sec - start.tv_sec ) * MICROSEC_PER_SEC  + (( end.tv_usec - start.tv_usec ) );
	printf("\telapsed_time: %d us\n",dur);

	return 0;
}
