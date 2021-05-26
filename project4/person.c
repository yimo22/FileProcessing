#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "person.h"
//필요한 경우 헤더 파일과 함수를 추가할 수 있음

#define DEBUG 1
#define HEADER_AREA_totalrecord_SIZE 4
#define HEADER_AREA_offset_SIZE 4
#define HEADER_AREA_length_SIZE 4
#define HEADER_AREA_RECORDMETADATA_SIZE 8

void readPage(FILE *, char *, int);
void writePage(FILE *, const char *, int);
void pack(char *recordbuf, const Person *p);
void unpack(char *recordbuf, Person *p);
void add(FILE *fp, const Person *p);
void delete (FILE *fp, const char *id);
int Get_HeaderRecord(FILE *fp, int *totalPage, int *totalRecord, int *Deletd_page, int *Delted_record);
void Update_HeaderRecord(FILE *fp, int totalPage, int totalRecord, int Deletd_page, int Delted_record);
void Making_newPage(FILE *fp, char *databuff);

/* This is for RECORD FILE */
const int HEADER_RECORD_SIZE = 16;

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 위의 readPage() 함수를 호출하여 pagebuf에 저장한 후, 여기에 필요에 따라서 새로운 레코드를 저장하거나
// 삭제 레코드 관리를 위한 메타데이터를 저장합니다. 그리고 난 후 writePage() 함수를 호출하여 수정된 pagebuf를
// 레코드 파일에 저장합니다. 반드시 페이지 단위로 읽거나 써야 합니다.
//
// 주의: 데이터 페이지로부터 레코드(삭제 레코드 포함)를 읽거나 쓸 때 페이지 단위로 I/O를 처리해야 하지만,
// 헤더 레코드의 메타데이터를 저장하거나 수정하는 경우 페이지 단위로 처리하지 않고 직접 레코드 파일을 접근해서 처리한다.

//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp, HEADER_RECORD_SIZE + PAGE_SIZE * pagenum, SEEK_SET);
	if (fread(pagebuf, sizeof(char), PAGE_SIZE, fp) < 0)
	{
		fprintf(stderr, "error - fread(pagebuf)\n");
		exit(1);
	}
	return;
}
//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다.
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp, pagenum * PAGE_SIZE + HEADER_RECORD_SIZE, SEEK_SET);
	if (fwrite(pagebuf, sizeof(char), PAGE_SIZE, fp) < 0)
	{
		fprintf(stderr, "error - fwrite(writePage)");
		exit(1);
	}
	return;
}
//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다.
//
void pack(char *recordbuf, const Person *p)
{
	sprintf(recordbuf, "%s#%s#%s#%s#%s#%s#", p->id, p->name, p->age, p->addr, p->phone, p->email);
	return;
}
//
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다.
//
void unpack(char *recordbuf, Person *p)
{
	char *ptr;
	ptr = strtok(recordbuf, "#");
	strcpy(p->id, ptr);
	strtok(NULL, " ");
	strcpy(p->name, ptr);
	strtok(NULL, " ");
	strcpy(p->age, ptr);
	strtok(NULL, " ");
	strcpy(p->addr, ptr);
	strtok(NULL, " ");
	strcpy(p->phone, ptr);
	strtok(NULL, " ");
	strcpy(p->email, ptr);
	return;
}
void Update_HeaderRecord(FILE *fp, int totalPage, int totalRecord, int Deletd_page, int Delted_record)
{
	fseek(fp, 0, SEEK_SET);
	fwrite(&totalPage, sizeof(int), 1, fp);
	fwrite(&totalRecord, sizeof(int), 1, fp);
	fwrite(&Deletd_page, sizeof(int), 1, fp);
	fwrite(&Delted_record, sizeof(int), 1, fp);
	return;
}
int Get_HeaderRecord(FILE *fp, int *totalPage, int *totalRecord, int *Deletd_page, int *Delted_record)
{
	fseek(fp, 0, SEEK_SET);
	if (fread(totalPage, sizeof(int), 1, fp) < 0)
		return -1;
	if (fread(totalRecord, sizeof(int), 1, fp) < 0)
		return -1;
	if (fread(Deletd_page, sizeof(int), 1, fp) < 0)
		return -1;
	if (fread(Delted_record, sizeof(int), 1, fp) < 0)
		return -1;
	return 1;
}
void WriteInPage_Or_NewPage(FILE *fp, char *recordbuff)
{
	int HEAD_RECORD_Allpage = 0;
	int HEAD_RECORD_Allrecrd = 0;
	int HEAD_RECORD_Del_page = -1;
	int HEAD_RECORD_Del_recrd = -1;
	Get_HeaderRecord(fp, &HEAD_RECORD_Allpage, &HEAD_RECORD_Allrecrd, &HEAD_RECORD_Del_page, &HEAD_RECORD_Del_recrd);
	int LIMIT_RECORD_NUM_IN_PAGE = (HEADER_AREA_SIZE - 4) / 8;

	char *pagebuff = (char *)malloc(sizeof(char) * PAGE_SIZE);

	int cur_page = HEAD_RECORD_Allpage - 1;
	readPage(fp, pagebuff, cur_page);
	/* Save the data at the end of Data page */

	// Update Header Area
	int total_record_num = 0;
	int cur_record_offset = -1;
	int cur_record_len = -1;
	memcpy(&total_record_num, pagebuff, 4);

	int pre_record_offset = -1;
	int pre_record_len = -1;
	// Move to pre record
	memcpy(&pre_record_offset, pagebuff + HEADER_AREA_totalrecord_SIZE + (HEADER_AREA_offset_SIZE + HEADER_AREA_length_SIZE) * (total_record_num - 1), 4);
	memcpy(&pre_record_len, pagebuff + HEADER_AREA_totalrecord_SIZE + (HEADER_AREA_offset_SIZE + HEADER_AREA_length_SIZE) * (total_record_num - 1) + HEADER_AREA_offset_SIZE, 4);
	int Available_Data_size = DATA_AREA_SIZE - (pre_record_offset + pre_record_len);

	if (total_record_num < LIMIT_RECORD_NUM_IN_PAGE && strlen(recordbuff) < Available_Data_size)
	{
		// Case - availalbe in page
		// Write Data in this page.
		int cur_record_num = total_record_num;
		total_record_num++;
		cur_record_offset = pre_record_len + pre_record_offset;
		cur_record_len = strlen(recordbuff);

		// Update HeaderArea
		memcpy(pagebuff, &total_record_num, 4);
		memcpy(pagebuff + HEADER_AREA_totalrecord_SIZE + (HEADER_AREA_offset_SIZE + HEADER_AREA_length_SIZE) * cur_record_num, &cur_record_offset, 4);
		memcpy(pagebuff + HEADER_AREA_totalrecord_SIZE + (HEADER_AREA_offset_SIZE + HEADER_AREA_length_SIZE) * cur_record_num + HEADER_AREA_offset_SIZE, &cur_record_len, 4);

		// Update HeaderRecord
		HEAD_RECORD_Allrecrd++;
		Update_HeaderRecord(fp, HEAD_RECORD_Allpage, HEAD_RECORD_Allrecrd, HEAD_RECORD_Del_page, HEAD_RECORD_Del_recrd);

		// Write Data
		strcpy(pagebuff + HEADER_AREA_SIZE + cur_record_offset, recordbuff);
		writePage(fp, pagebuff, cur_page);
	}
	else
	{
		// Case - Make new page
		Making_newPage(fp, recordbuff);
	}
}
//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값들을 구조체에 저장한 후 아래 함수를 호출한다.
//
void add(FILE *fp, const Person *p)
{
	int HEAD_RECORD_Allpage = 0;
	int HEAD_RECORD_Allrecrd = 0;
	int HEAD_RECORD_Del_page = -1;
	int HEAD_RECORD_Del_recrd = -1;
	Get_HeaderRecord(fp, &HEAD_RECORD_Allpage, &HEAD_RECORD_Allrecrd, &HEAD_RECORD_Del_page, &HEAD_RECORD_Del_recrd);
	if (HEAD_RECORD_Allrecrd == 0 && HEAD_RECORD_Allpage == 0)
	{
		// Init
		fseek(fp, 0, SEEK_SET);
		HEAD_RECORD_Allpage = 1;
		if (fwrite(&HEAD_RECORD_Allpage, sizeof(int), 1, fp) < 0)
		{
			perror("Init fwrite");
			exit(1);
		}
		if (fwrite(&HEAD_RECORD_Allrecrd, sizeof(int), 1, fp) < 0)
		{
			perror("Init fwrite");
			exit(1);
		}
		if (fwrite(&HEAD_RECORD_Del_page, sizeof(int), 1, fp) < 0)
		{
			perror("Init fwrite");
			exit(1);
		}
		if (fwrite(&HEAD_RECORD_Del_recrd, sizeof(int), 1, fp) < 0)
		{
			perror("Init fwrite");
			exit(1);
		}
	}
	// Get RecordBuff from unpack()
	int LIMIT_RECORD_NUM_IN_PAGE = (HEADER_AREA_SIZE - 4) / 8;
	char *recrdbuff = (char *)malloc(sizeof(char) * (MAX_RECORD_SIZE + 8));
	char *pagebuff = (char *)malloc(sizeof(char) * PAGE_SIZE);
	pack(recrdbuff, (Person *)p);

	if (HEAD_RECORD_Del_page == -1 && HEAD_RECORD_Del_recrd == -1){
		WriteInPage_Or_NewPage(fp,recrdbuff);
	}
	else{
		// Deleted Record exist
		int data_len = strlen(recrdbuff);
		int next_page = -1;
		int next_record = -1;
		int cur_page = HEAD_RECORD_Del_page;
		int cur_record = HEAD_RECORD_Del_recrd;
		int offset;
		int len;
		while (1)
		{
			int pre_page;
			int pre_record;
			readPage(fp, pagebuff, cur_page);
			if (cur_page == -1 && cur_record == -1){
				/* Move to Last Page and Check */
				WriteInPage_Or_NewPage(fp, recrdbuff);
				return;
			}
			// Get information of pre/next deleted Node
			memcpy(&offset, pagebuff + HEADER_AREA_totalrecord_SIZE + (HEADER_AREA_offset_SIZE + HEADER_AREA_length_SIZE) * cur_record, 4);
			memcpy(&len, pagebuff + HEADER_AREA_totalrecord_SIZE + (HEADER_AREA_offset_SIZE + HEADER_AREA_length_SIZE) * cur_record + HEADER_AREA_offset_SIZE, 4);
			memcpy(&pre_page, pagebuff + HEADER_AREA_SIZE + offset + 1, 4);
			memcpy(&pre_record, pagebuff + HEADER_AREA_SIZE + offset + 5, 4);
			if (len >= data_len){
				// Update Data
				strncpy(pagebuff + HEADER_AREA_SIZE + offset, recrdbuff, data_len);

				// Update Metadata in Header Record Del
				if (next_page == -1 && next_record == -1){
					HEAD_RECORD_Del_page = pre_page;
					HEAD_RECORD_Del_recrd = pre_record;
					Update_HeaderRecord(fp, HEAD_RECORD_Allpage, HEAD_RECORD_Allrecrd, HEAD_RECORD_Del_page, HEAD_RECORD_Del_recrd);
				}
				else{
					char *temp_page_buff = (char *)malloc(sizeof(char) * PAGE_SIZE);
					readPage(fp, temp_page_buff, next_page);
					int next_offset;
					memcpy(&next_offset, temp_page_buff + HEADER_AREA_totalrecord_SIZE + (HEADER_AREA_offset_SIZE + HEADER_AREA_length_SIZE) * next_record, 4);
					memset(temp_page_buff + HEADER_AREA_SIZE + next_offset, '*', 1);
					memset(temp_page_buff + HEADER_AREA_SIZE + next_offset + 1, pre_page, 4);
					memset(temp_page_buff + HEADER_AREA_SIZE + next_offset + 5, pre_record, 4);
					writePage(fp, temp_page_buff, next_page);
				}
				writePage(fp, pagebuff, cur_page);
				return;
			}
			// Update cur/next page & record
			next_page = cur_page;
			next_record = cur_record;
			cur_page = pre_page;
			cur_record = pre_record;
		}
	}
}
void Making_newPage(FILE *fp, char *databuff){
	int HEAD_RECORD_Allpage = -1;
	int HEAD_RECORD_Allrecrd = -1;
	int HEAD_RECORD_Del_page = -1;
	int HEAD_RECORD_Del_recrd = -1;
	Get_HeaderRecord(fp, &HEAD_RECORD_Allpage, &HEAD_RECORD_Allrecrd, &HEAD_RECORD_Del_page, &HEAD_RECORD_Del_recrd);

	char *pagebuff = (char *)malloc(sizeof(char) * PAGE_SIZE);
	int record_num;
	int cur_record_offset;
	int cur_record_len;

	record_num = 1;
	cur_record_offset = 0;
	cur_record_len = strlen(databuff);

	// Update Record Area
	memcpy(pagebuff, &record_num, 4);
	memcpy(pagebuff + HEADER_AREA_totalrecord_SIZE, &cur_record_offset, 4);
	memcpy(pagebuff + HEADER_AREA_totalrecord_SIZE + HEADER_AREA_offset_SIZE, &cur_record_len, 4);

	// Update Header Record
	int cur_page = HEAD_RECORD_Allpage;
	HEAD_RECORD_Allpage++;
	HEAD_RECORD_Allrecrd++;
	Update_HeaderRecord(fp, HEAD_RECORD_Allpage, HEAD_RECORD_Allrecrd, HEAD_RECORD_Del_page, HEAD_RECORD_Del_recrd);
	// Write Data
	strcpy(pagebuff + HEADER_AREA_SIZE + cur_record_offset, databuff);
	writePage(fp, pagebuff, cur_page);
}
//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void delete (FILE *fp, const char *id)
{
	int HEAD_RECORD_Allpage = -1;
	int HEAD_RECORD_Allrecrd = -1;
	int HEAD_RECORD_Del_page = -1;
	int HEAD_RECORD_Del_recrd = -1;
	Get_HeaderRecord(fp, &HEAD_RECORD_Allpage, &HEAD_RECORD_Allrecrd, &HEAD_RECORD_Del_page, &HEAD_RECORD_Del_recrd);

	char *pagebuff = (char *)malloc(sizeof(char) * PAGE_SIZE);
	int page_Allrecord = 0;
	int page_offset = 0;
	int page_len = 0;
	for (int i = 0; i < HEAD_RECORD_Allpage; i++){
		readPage(fp, pagebuff, i);
		memcpy(&page_Allrecord, pagebuff, 4);
		for (int j = 0; j < page_Allrecord; j++){
			Person *temp = (Person *)malloc(sizeof(Person));
			char data_id[14];
			char *temp_data = (char *)malloc(sizeof(char) * (MAX_RECORD_SIZE + 8));
			memcpy(&page_offset, pagebuff + 4 + 8 * j, 4);
			memcpy(&page_len, pagebuff + 8 * (j + 1), 4);
			memcpy(temp_data, pagebuff + HEADER_AREA_SIZE + page_offset, page_len);
			unpack(temp_data, temp);
			strcpy(data_id, temp->id);
			if (data_id[0] == '*')
				continue;
			if (strcmp(data_id, id) == 0){
				memset(pagebuff + HEADER_AREA_SIZE + page_offset, '*', 1);
				memcpy(pagebuff + HEADER_AREA_SIZE + page_offset + 1, &HEAD_RECORD_Del_page, 4);
				memcpy(pagebuff + HEADER_AREA_SIZE + page_offset + 5, &HEAD_RECORD_Del_recrd, 4);
				// Update HEAD record
				HEAD_RECORD_Del_page = i;
				HEAD_RECORD_Del_recrd = j;
				Update_HeaderRecord(fp, HEAD_RECORD_Allpage, HEAD_RECORD_Allrecrd, HEAD_RECORD_Del_page, HEAD_RECORD_Del_recrd);

				free(temp);
				free(temp_data);
				writePage(fp, pagebuff, i);
				return;
			}
		}
	}
	fprintf(stderr, "Not Detected\n");
}
void PrintAllData(FILE *fp)
{
	int HEAD_RECORD_Allpage = 0;
	int HEAD_RECORD_Allrecrd = 0;
	int HEAD_RECORD_Del_page = -1;
	int HEAD_RECORD_Del_recrd = -1;
	Get_HeaderRecord(fp, &HEAD_RECORD_Allpage, &HEAD_RECORD_Allrecrd, &HEAD_RECORD_Del_page, &HEAD_RECORD_Del_recrd);

	int RECORD_NUM;
	printf("HEAD_RECORD_ALLPAGE : %d\n", HEAD_RECORD_Allpage);
	printf("HEAD_RECORD_Allrecrd : %d\n", HEAD_RECORD_Allrecrd);
	printf("HEAD_RECORD_Del_page : %d\n", HEAD_RECORD_Del_page);
	printf("HEAD_RECORD_Del_recrd : %d\n", HEAD_RECORD_Del_recrd);
	char *pagebuff = (char *)malloc(sizeof(char) * PAGE_SIZE);
	for (int i = 0; i < HEAD_RECORD_Allpage; i++)
	{
		readPage(fp, pagebuff, i);
		memcpy(&RECORD_NUM, pagebuff, 4);
		printf("[%d page]\n", i);
		printf("[%d's RecordNum] : %d\n", i, RECORD_NUM);
		for (int j = 0; j < RECORD_NUM; j++)
		{
			int len, offset;
			memcpy(&offset, pagebuff + 4 + 8 * j, 4);
			memcpy(&len, pagebuff + 4 + 8 * j + 4, 4);
			char *recordbuff = (char *)malloc(sizeof(char) * 9);
			memcpy(recordbuff, pagebuff + HEADER_AREA_SIZE + offset, 9);
			printf("> %d record's offset : %d\n", j, offset);
			printf("> %d record's len : %d\n", j, len);
			if (recordbuff[0] != '*')
				printf("> %d record : %s\n", j, recordbuff);
			else
			{
				int temp_page, temp_record;
				memcpy(&temp_page, pagebuff + HEADER_AREA_SIZE + offset + 1, 4);
				memcpy(&temp_record, pagebuff + HEADER_AREA_SIZE + offset + 5, 4);
				printf("> %d record : %d,%d\n", j, temp_page, temp_record);
			}
		}
	}

	return;
}
int main(int argc, char *argv[])
{
	FILE *fp; // 레코드 파일의 파일 포인터
	fp = fopen(argv[2], "r+");
	if (fp == NULL)
	{
		fp = fopen(argv[2], "w+");
	}
	Person p;
	switch (**(argv + 1))
	{
	case 'd':
		delete (fp, argv[3]);
		break;

	case 'a':
		strcpy(p.id, argv[3]);
		strcpy(p.name, argv[4]);
		strcpy(p.age, argv[5]);
		strcpy(p.addr, argv[6]);
		strcpy(p.phone, argv[7]);
		strcpy(p.email, argv[8]);
		add(fp, &p);
		break;

	default:
		printf("Error-Invalid Input\n");
		break;
	}

	PrintAllData(fp);
	return 0;
}
