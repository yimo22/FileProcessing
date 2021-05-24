#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "person.h"
//�ʿ��� ��� ��� ���ϰ� �Լ��� �߰��� �� ����

#define DEBUG 1
#define HEADER_AREA_totalrecord_SIZE 4
#define HEADER_AREA_offset_SIZE 4
#define HEADER_AREA_length_SIZE 4

void Making_newPage(FILE *fp , char * databuff);

/* This is for RECORD FILE */
const int HEADER_RECORD_SIZE = 16;

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

	/*
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

	*/
	fseek(fp, HEADER_RECORD_SIZE + PAGE_SIZE * pagenum,SEEK_SET);
	// Read 
	if(fread(pagebuf,sizeof(char),PAGE_SIZE,fp) < 0){
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
	// Write Pagebuf
	#ifdef DEBUG
		printf("\tMoving to pagenum : %d\n",pagenum);
		fseek(fp,0,SEEK_END);
		int b_filesize = ftell(fp);
		printf("\tbefore file size : %d\n",b_filesize);
	#endif
	fseek(fp,pagenum * PAGE_SIZE + HEADER_RECORD_SIZE,SEEK_SET);
	fwrite(pagebuf,sizeof(char),PAGE_SIZE,fp);
	#ifdef DEBUG
		fseek(fp,0,SEEK_END);
		int af_filesize = ftell(fp);
		printf("\tafter file size : %d\n",af_filesize);
	#endif
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
void unpack(char *recordbuf, Person *p)
{
	sprintf(recordbuf,"%s#%s#%s#%s#%s#%s#",p->id,p->name,p->age,p->addr,p->phone,p->email);
	return ;
}


int Get_Datapage_FROM_HEADRECORD(FILE * fp){
	fseek(fp,0,SEEK_SET);
	int temp;
	fread(&temp,sizeof(int),1,fp);
	return temp;
}
int Get_Allrecord_FROM_HEADRECORD(FILE * fp){
	fseek(fp,4,SEEK_SET);
	int temp;
	fread(&temp,sizeof(int),1,fp);
	return temp;
}
int Get_Deleted_page_FROM_HEADRECORD(FILE * fp){
	fseek(fp,8,SEEK_SET);
	int temp;
	fread(&temp,sizeof(int),1,fp);
	return temp;
}
int Get_Deleted_record_FROM_HEADRECORD(FILE * fp){
	fseek(fp,12,SEEK_SET);
	int temp;
	fread(&temp,sizeof(int),1,fp);
	return temp;
}

void Update_HeaderRecord(FILE * fp, int totalPage, int totalRecord, int Deletd_page, int Delted_record){
	fseek(fp,0,SEEK_SET);
	fwrite(&totalPage,sizeof(int),1,fp);
	fwrite(&totalRecord,sizeof(int),1,fp);
	fwrite(&Deletd_page,sizeof(int),1,fp);
	fwrite(&Delted_record,sizeof(int),1,fp);

	return ;
}
int Get_HeaderRecord(FILE * fp, int* totalPage, int* totalRecord, int *Deletd_page, int *Delted_record){
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
//
// ���ο� ���ڵ带 �����ϴ� ����� �����ϸ�, �͹̳ηκ��� �Է¹��� �ʵ尪���� ����ü�� ������ �� �Ʒ� �Լ��� ȣ���Ѵ�.
//
void add(FILE *fp, const Person *p)
{
	/*
	 Examples of input file
	 a.out a person.dat "999999" "GD HONG" "23" "Seoul" "02-08"
	*/
	int HEAD_RECORD_Allpage = 0;
	int HEAD_RECORD_Allrecrd = 0;
	int HEAD_RECORD_Del_page = -1;
	int HEAD_RECORD_Del_recrd = -1;
	Get_HeaderRecord(fp, &HEAD_RECORD_Allpage, &HEAD_RECORD_Allrecrd, &HEAD_RECORD_Del_page, &HEAD_RECORD_Del_recrd);
	if (HEAD_RECORD_Allrecrd == 0 && HEAD_RECORD_Allpage == 0)
	{
		// Init
		fseek(fp, 0, SEEK_SET);
	//	HEAD_RECORD_Allpage = ;
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
	unpack(recrdbuff, (Person *)p);

	if (HEAD_RECORD_Del_page == -1 && HEAD_RECORD_Del_recrd == -1)
	{
		#ifdef DEBUG
		printf("> Deleted Record doesn't exist\n");
		#endif
		
		// Deleted Record doesn't exist
		
		int cur_page = HEAD_RECORD_Allpage;
		printf("cur_page : %d\n",cur_page);
		readPage(fp,pagebuff,cur_page);
		/* Save the data at the end of Data page */

		// Update Header Area
		int total_record_num = 0;
		int cur_record_offset = -1;
		int cur_record_len = -1;
		memcpy(&total_record_num, pagebuff, 4);

		int pre_record_offset = -1;
		int pre_record_len = -1;
		// Move to pre record
		memcpy(&pre_record_offset, pagebuff + HEADER_AREA_totalrecord_SIZE + (HEADER_AREA_offset_SIZE+HEADER_AREA_length_SIZE)*(total_record_num-1), 4);
		memcpy(&pre_record_len, pagebuff + HEADER_AREA_totalrecord_SIZE + (HEADER_AREA_offset_SIZE+HEADER_AREA_length_SIZE)*(total_record_num-1) + HEADER_AREA_offset_SIZE, 4);
		int Available_Data_size = DATA_AREA_SIZE - (pre_record_offset + pre_record_len);
#ifdef DEBUG
		printf("\ttotal_record_num : %d\n", total_record_num);
		printf("\tLIMIT_RECORD_NUM : %d\n", LIMIT_RECORD_NUM_IN_PAGE);
		printf("\trecrdbuff : %s\n", recrdbuff);
		printf("\tstrlen(recrdbuff) : %ld\n", strlen(recrdbuff));
		printf("\tAvailable_data_size : %d\n", Available_Data_size);
#endif
		/*
			checking whether it is possible to insert new record in data area
		*/
		if (total_record_num < LIMIT_RECORD_NUM_IN_PAGE && strlen(recrdbuff) < Available_Data_size)
		{

			// Case - availalbe in page
			// Write Data in this page.
			int cur_record_num = total_record_num;
			total_record_num++;
			cur_record_offset = pre_record_len + pre_record_offset;
			cur_record_len = strlen(recrdbuff);
			
			// Update HeaderArea
			memcpy(pagebuff,&total_record_num,4);
			memcpy(pagebuff+HEADER_AREA_totalrecord_SIZE+(HEADER_AREA_offset_SIZE+HEADER_AREA_length_SIZE)*cur_record_num,&cur_record_offset,4);
			memcpy(pagebuff+HEADER_AREA_totalrecord_SIZE+(HEADER_AREA_offset_SIZE+HEADER_AREA_length_SIZE)*cur_record_num + HEADER_AREA_offset_SIZE,&cur_record_len,4);

			
#ifdef DEBUG
			printf("\t\tcase - available in page\n");
			printf("\t\tcur_record_offset : %d\n",cur_record_offset);
			printf("\t\tcur_record_len : %d\n",cur_record_len);
			printf("\t\tpre_record_offset : %d\n",pre_record_offset);
			printf("\t\tpre_record_len : %d\n",pre_record_offset);


#endif
			// Update HeaderRecord
			HEAD_RECORD_Allrecrd++;
			Update_HeaderRecord(fp, HEAD_RECORD_Allpage, HEAD_RECORD_Allrecrd, HEAD_RECORD_Del_page, HEAD_RECORD_Del_recrd);

			// Write Data
			strcpy(pagebuff + HEADER_AREA_SIZE + cur_record_offset, recrdbuff);
			writePage(fp,pagebuff,cur_page);
		}
		else
		{
		#ifdef DEBUG
		printf("\t\tcase - Making new page\n");
		#endif
			// Case - Make new page
			Making_newPage(fp,recrdbuff);
		}
	}
	else
	{
			#ifdef DEBUG
		printf("> Deleted Record does exist\n");
		printf("\t\tcase - Making new page\n");
		#endif
		// Deleted Record exist
		int data_len = strlen(recrdbuff);
		int next_page = -1;
		int next_record = -1;
		int cur_page = HEAD_RECORD_Del_page;
		int cur_record = HEAD_RECORD_Del_recrd;
		int offset;
		int len;
		readPage(fp,pagebuff,cur_page);
		while(1){
			if(cur_page == -1 && cur_record == -1){
				Making_newPage(fp,recrdbuff);
				break;
			}
			// Get information
			memcpy(&offset, pagebuff+HEADER_AREA_totalrecord_SIZE+(HEADER_AREA_offset_SIZE+HEADER_AREA_length_SIZE)*cur_record , 4);
			memcpy(&len, pagebuff + HEADER_AREA_totalrecord_SIZE + (HEADER_AREA_offset_SIZE + HEADER_AREA_length_SIZE) * cur_record + HEADER_AREA_offset_SIZE, 4);
			if (len >= data_len)
			{
				// Get pre_deleted data
				int pre_page, pre_record;
				memcpy(&pre_page, pagebuff + HEADER_AREA_SIZE + len + 1, 4);
				memcpy(&pre_record, pagebuff + HEADER_AREA_SIZE + len + 5, 4);

				// Update Data
				strcpy(pagebuff + HEADER_AREA_SIZE + len, recrdbuff);

				// Update Metadata
				if (next_page == -1 && next_record == -1)
				{

					HEAD_RECORD_Del_page = pre_page;
					HEAD_RECORD_Del_recrd = pre_record;
					Update_HeaderRecord(fp, HEAD_RECORD_Allpage, HEAD_RECORD_Allrecrd, HEAD_RECORD_Del_page, HEAD_RECORD_Del_recrd);
				}
				else 
				{
					char *temp_page_buff = (char *)malloc(sizeof(char) * PAGE_SIZE);
					readPage(fp, temp_page_buff, next_page);
					int next_offset;
					memcpy(&next_offset, pagebuff + HEADER_AREA_totalrecord_SIZE + (HEADER_AREA_offset_SIZE + HEADER_AREA_length_SIZE) * next_record, 4);
					/*
						before Situation.
							next_deleted -> cur_deleted -> pre_deleted 
						After Situation.
							next_deleted -> pre_deleted
							cur_deleted : Not deleted!!!
					*/
					memset(temp_page_buff + HEADER_AREA_SIZE + next_offset, '*', 1);
					memset(temp_page_buff + HEADER_AREA_SIZE + next_offset + 1, pre_page, 4);
					memset(temp_page_buff + HEADER_AREA_SIZE + next_offset + 5, pre_record, 4);
					writePage(fp, temp_page_buff, next_page);
				}
				return;
			}

			// Update cur/next page & record
			next_page = cur_page;
			next_record = cur_record;
			memcpy(&cur_page,pagebuff+HEADER_AREA_SIZE+offset+1,4);
			memcpy(&cur_record,pagebuff+HEADER_AREA_SIZE+offset+5,4);
		}
	}
}
void Making_newPage(FILE *fp , char * databuff){
	#ifdef DEBUG
	printf("======================MAKING_NEWPAGE===================\n");
	#endif
	int HEAD_RECORD_Allpage = -1;
	int HEAD_RECORD_Allrecrd = -1;
	int HEAD_RECORD_Del_page = -1;
	int HEAD_RECORD_Del_recrd = -1;
	Get_HeaderRecord(fp,&HEAD_RECORD_Allpage,&HEAD_RECORD_Allrecrd,&HEAD_RECORD_Del_page,&HEAD_RECORD_Del_recrd);

	char * pagebuff = (char *) malloc (sizeof(char) * PAGE_SIZE);	
	int record_num;
	int cur_record_offset;
	int cur_record_len;

	record_num = 1;
	cur_record_offset = 0;
	cur_record_len = strlen(databuff);
	
	// Update Record Area 
	memcpy(pagebuff,&record_num,4);
	memcpy(pagebuff+HEADER_AREA_totalrecord_SIZE,&cur_record_offset,4);
	memcpy(pagebuff+HEADER_AREA_totalrecord_SIZE+HEADER_AREA_offset_SIZE,&cur_record_len,4);

	// Update Header Record
	HEAD_RECORD_Allpage++;
	HEAD_RECORD_Allrecrd++;
	int cur_page = HEAD_RECORD_Allpage;
	Update_HeaderRecord(fp, HEAD_RECORD_Allpage, HEAD_RECORD_Allrecrd, HEAD_RECORD_Del_page, HEAD_RECORD_Del_recrd);
	#ifdef DEBUG
	printf("Allpage : %d\n",HEAD_RECORD_Allpage);
	printf("Allrecrd : %d\n",HEAD_RECORD_Allrecrd);
	printf("Del_page : %d\n",HEAD_RECORD_Del_page);
	printf("Del_recrd : %d\n",HEAD_RECORD_Del_recrd);
	printf("======================MAKING_NEWPAGE_END===============\n");

	#endif
	// Write Data
	strcpy(pagebuff + HEADER_AREA_SIZE + cur_record_offset, databuff);
	writePage(fp, pagebuff, cur_page);

}
//
// �ֹι�ȣ�� ��ġ�ϴ� ���ڵ带 ã�Ƽ� �����ϴ� ����� �����Ѵ�.
//
void delete(FILE *fp, const char *id)
{
}

int main(int argc, char *argv[])
{
	FILE *fp;  // ���ڵ� ������ ���� ������
	fp = fopen("test.dat","w+");
	if(fp == NULL){
		perror("fopen");
		exit(1);
	}
	Person p1 ={
		"20172600","yimo22","99","suwon","010","test@naver.com"
	};
	Person p2 ={
		"20172610","yimo22","99","suwon","010","test@naver.com"
	};
	Person p3 ={
		"20172620","yimo22","99","suwon","010","test@naver.com"
	};
	Person p4 ={
		"20172630","yimo22","99","suwon","010","test@naver.com"
	};
	add(fp,&p1);
	add(fp,&p2);
	add(fp,&p3);
	add(fp,&p4);
	return 1;
}