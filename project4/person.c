#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "person.h"
//필요한 경우 헤더 파일과 함수를 추가할 수 있음

#define DEBUG 1
#define HEADER_AREA_totalrecord 4
#define HEADER_AREA_offset 4
#define HEADER_AREA_length 4

/* This is for RECORD FILE */
const int HEADER_RECORD_SIZE = 16;
int TOTAL_PAGE_NUM;
int TOTAL_RECORD_NUM;
int PAGE_NUM;
int RECORD_NUM;

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
	/* Get the page that indicated pagenum to pagebuf */
	

	if( fp == NULL){
		fprintf(stderr, "Error - Unexpeted File\n");
		exit(1);
	}
	/*
		Argument check from HEADER RECORD
		- whether indicated page is deleted
	*/
	if(fread(&TOTAL_PAGE_NUM,sizeof(int),1,fp) < 0){
		fprintf(stderr,"error - fread(TOTAL_PAGE_NUM\n");
		exit(1);
	}
	if(fread(&TOTAL_RECORD_NUM,sizeof(int),1,fp) < 0){
		fprintf(stderr,"error - fread(TOTAL_RECORD_NUM\n");
		exit(1);
	}
	if(fread(&PAGE_NUM,sizeof(int),1,fp) < 0){
		fprintf(stderr,"error - fread(PAGE_NUM\n");
		exit(1);
	}
	if(fread(&RECORD_NUM,sizeof(int),1,fp) < 0){
		fprintf(stderr,"error - fread(RECORD_NUM\n");
		exit(1);
	}
	if( TOTAL_PAGE_NUM < pagenum){
		fprintf(stderr,"error - pagenum(readPage)\n");
		exit(1);
	}
	// mv FILE seek point
	fseek(fp, HEADER_RECORD_SIZE + PAGE_SIZE * pagenum,SEEK_SET);
	// Read 
	if(fread(pagebuf,sizeof(int),PAGE_SIZE,fp) < 0){
		fprintf(stderr,"error - fread(pagebuf)\n");
		exit(1);
	}
	return ;
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다. 
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	// HEADER RECORD update
	fseek(fp,0,SEEK_SET);
	fwrite(&TOTAL_PAGE_NUM,sizeof(int),1,fp);
	fwrite(&TOTAL_RECORD_NUM,sizeof(int),1,fp);
	fwrite(&PAGE_NUM,sizeof(int),1,fp);
	fwrite(&RECORD_NUM,sizeof(int),1,fp);
	// Write Pagebuf
	fseek(fp,HEADER_RECORD_SIZE + PAGE_SIZE * pagenum , SEEK_SET);
	fwrite(pagebuf,sizeof(char),PAGE_SIZE,fp);
}

//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 
//
void pack(char *recordbuf, const Person *p)
{
	char Delimiter = '#';
	char *Record_form = (char *)malloc(sizeof(char) * MAX_RECORD_SIZE);
	memset(Record_form, MAX_RECORD_SIZE, 0);
	printf("<<pack>> %s \n", Record_form);
	if (p->id != NULL)
	{
		strcat(Record_form, p->id);
		strcat(Record_form, "#");
#ifdef DEBUG
		printf("<<pack>> %s \n", Record_form);
#endif
	}
	if (p->name != NULL)
	{
		strcat(Record_form, p->name);
		strcat(Record_form, "#");
#ifdef DEBUG
		printf("<<pack>> %s \n", Record_form);
#endif
	}
	if (p->age != NULL)
	{
		strcat(Record_form, p->age);
		strcat(Record_form, "#");
#ifdef DEBUG
		printf("<<pack>> %s \n", Record_form);
#endif
	}
	if (p->addr != NULL)
	{
		strcat(Record_form, p->addr);
		strcat(Record_form, "#");
#ifdef DEBUG
		printf("<<pack>> %s \n", Record_form);
#endif
	}
	if (p->phone != NULL)
	{
		strcat(Record_form, p->phone);
		strcat(Record_form, "#");
#ifdef DEBUG
		printf("<<pack>> %s \n", Record_form);
#endif
	}
	if (p->email != NULL)
	{
		strcat(Record_form, p->email);
		strcat(Record_form,"#");
#ifdef DEBUG
		printf("<<pack>> %s \n", Record_form);
#endif
	}
	strcpy(recordbuf,Record_form);
	free(Record_form);
}

// 
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다.
//
void unpack(const char *recordbuf, Person *p)
{
	char * ptr;
	ptr = strtok(recordbuf, "#");
	strcpy(p->id, ptr);

	ptr = strtok(NULL, "#");
	strcpy(p->name, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->age, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->addr, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->phone, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->email, ptr);

#ifdef DEBUG
		printf("<<unpack>>\n");
		printf(" >> id : %s\n",p->id);
		printf(" >> name : %s\n",p->name);
		printf(" >> age : %s\n",p->age);
		printf(" >> addr : %s\n",p->addr);
		printf(" >> phone : %s\n",p->phone);
		printf(" >> email : %s\n",p->email);
	#endif
	return ;
}

//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값들을 구조체에 저장한 후 아래 함수를 호출한다.
//
void add(FILE *fp, const Person *p)
{
	/*
	 if deleted record exist, save the data that space first
	 check the latest deleted record and right size(first fit). -> just check record len.
	 if there's no space to save, save with append 
	 if there's no space to save with append, allocate new page and save.
	 when you save in deleted space, do internal fragmentation.
	 -> new record length == deleted record size
	 renew header area/record, notice that header record's #records is (nomal record + delted record)
	 Examples of input file
	 a.out a person.dat "999999" "GD HONG" "23" "Seoul" "02-08"
	*/
	

	// Get RecordBuff from unpack()
	char * recrdbuff = (char *) malloc(sizeof(char) * MAX_RECORD_SIZE);
	unpack(recrdbuff,p);
	

	if( RECORD_NUM == -1 && PAGE_NUM == -1){
		// Deleted Record doesn't exist

	}
	else{
		// Deleted Record exist

		// Moved to deleted record that indicated in HEADER RECORD and Check the size. 
		if( strlen(recrdbuff) > Get_rightsize(fp,PAGE_NUM,RECORD_NUM)) {
			// Update Head_AREA (#records, record offset, record length)

			// Update TOTAL_RECORD_NUM in HeadRecord

			// Update PAGE_NUM && RECORD_NUM in HeadRecord



		}
		else{
			/* Find deleted record that fits in */
			int temp_pagenum = PAGE_NUM;
			int temp_recordnum = RECORD_NUM;
			while(strlen(recrdbuff) > Get_rightsize(fp,temp_pagenum,temp_recordnum)){
				MoveToRecord(fp,temp_pagenum,Get_record_offset(fp,temp_pagenum,temp_recordnum));
				fread(&temp_pagenum,sizeof(int),1,fp);
				fread(&temp_recordnum,sizeof(int),1,fp);


				if( temp_pagenum == -1 && temp_recordnum == -1){
					// Make New page and Save the data.




					break;
				} 
			}
		}

		// forwarding next deleted record
	}
}

//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void MoveToRecord(FILE * fp , int pagenum, int offset){
	fseek( fp, HEADER_RECORD_SIZE + PAGE_SIZE * pagenum + offset,SEEK_SET);
}
int Get_record_offset(FILE * fp, int pagenum, int recordnum){
	fseek(fp, HEADER_RECORD_SIZE + PAGE_SIZE * pagenum);
	fseek(fp,HEADER_AREA_totalrecord+(HEADER_AREA_offset+HEADER_AREA_length)*recordnum ,SEEK_CUR);
	int temp;
	fread(&temp,sizeof(int),1,fp);
	return temp;
}
int Get_rightsize(FILE *fp , int pagenum , int recordnum){
	fseek(fp, HEADER_RECORD_SIZE + PAGE_SIZE * pagenum);
	fseek(fp,HEADER_AREA_totalrecord+(HEADER_AREA_offset+HEADER_AREA_length)*recordnum + HEADER_AREA_offset,SEEK_CUR);

	int area_len = 0;
	fread(&area_len,sizeof(int),1,fp);

	return area_len;
}
void delete(FILE *fp, const char *id)
{
}

int main(int argc, char *argv[])
{
	FILE *fp;  // 레코드 파일의 파일 포인터
	
	Person p ={
		"20172612","yimo22","99","suwon","010","test@naver.com"
	};
	char * rdbuff = (char *) malloc (sizeof(char) * MAX_RECORD_SIZE);
	pack(rdbuff,&p);
	Person pq;
	unpack(rdbuff,&pq);

	return 1;
}