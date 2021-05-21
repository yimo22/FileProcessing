#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "person.h"
//�ʿ��� ��� ��� ���ϰ� �Լ��� �߰��� �� ����

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

// ���� ������� �����ϴ� ����� ���� �ٸ� �� ������ �ణ�� ������ �Ӵϴ�.
// ���ڵ� ������ ������ ������ ���� �����Ǳ� ������ ����� ���α׷����� ���ڵ� ���Ϸκ��� �����͸� �а� �� ����
// ������ ������ ����մϴ�. ���� �Ʒ��� �� �Լ��� �ʿ��մϴ�.
// 1. readPage(): �־��� ������ ��ȣ�� ������ �����͸� ���α׷� ������ �о�ͼ� pagebuf�� �����Ѵ�
// 2. writePage(): ���α׷� ���� pagebuf�� �����͸� �־��� ������ ��ȣ�� �����Ѵ�
// ���ڵ� ���Ͽ��� ������ ���ڵ带 �аų� ���ο� ���ڵ带 ���ų� ���� ���ڵ带 ������ ����
// ���� readPage() �Լ��� ȣ���Ͽ� pagebuf�� ������ ��, ���⿡ �ʿ信 ���� ���ο� ���ڵ带 �����ϰų�
// ���� ���ڵ� ������ ���� ��Ÿ�����͸� �����մϴ�. �׸��� �� �� writePage() �Լ��� ȣ���Ͽ� ������ pagebuf��
// ���ڵ� ���Ͽ� �����մϴ�. �ݵ�� ������ ������ �аų� ��� �մϴ�.
//
// ����: ������ �������κ��� ���ڵ�(���� ���ڵ� ����)�� �аų� �� �� ������ ������ I/O�� ó���ؾ� ������,
// ��� ���ڵ��� ��Ÿ�����͸� �����ϰų� �����ϴ� ��� ������ ������ ó������ �ʰ� ���� ���ڵ� ������ �����ؼ� ó���Ѵ�.

//
// ������ ��ȣ�� �ش��ϴ� �������� �־��� ������ ���ۿ� �о �����Ѵ�. ������ ���۴� �ݵ�� ������ ũ��� ��ġ�ؾ� �Ѵ�.
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
// ������ ������ �����͸� �־��� ������ ��ȣ�� �ش��ϴ� ���ڵ� ������ ��ġ�� �����Ѵ�. 
// ������ ���۴� �ݵ�� ������ ũ��� ��ġ�ؾ� �Ѵ�.
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
// ���ο� ���ڵ带 ������ �� �͹̳ηκ��� �Է¹��� ������ Person ����ü�� ���� �����ϰ�, pack() �Լ��� ����Ͽ�
// ���ڵ� ���Ͽ� ������ ���ڵ� ���¸� recordbuf�� �����. 
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
// �Ʒ��� unpack() �Լ��� recordbuf�� ����Ǿ� �ִ� ���ڵ带 ����ü�� ��ȯ�� �� ����Ѵ�.
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
// ���ο� ���ڵ带 �����ϴ� ����� �����ϸ�, �͹̳ηκ��� �Է¹��� �ʵ尪���� ����ü�� ������ �� �Ʒ� �Լ��� ȣ���Ѵ�.
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
// �ֹι�ȣ�� ��ġ�ϴ� ���ڵ带 ã�Ƽ� �����ϴ� ����� �����Ѵ�.
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
	FILE *fp;  // ���ڵ� ������ ���� ������
	
	Person p ={
		"20172612","yimo22","99","suwon","010","test@naver.com"
	};
	char * rdbuff = (char *) malloc (sizeof(char) * MAX_RECORD_SIZE);
	pack(rdbuff,&p);
	Person pq;
	unpack(rdbuff,&pq);

	return 1;
}