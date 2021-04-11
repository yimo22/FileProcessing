/*
  학부 : 컴퓨터학부
  학번 : 20172612
  성명 : 김영도

  주어진 파일에서 <오프셋>과 <오프셋+1> 사이에 <데이터>를 끼워 넣는다.
  a.out <파일명> <오프셋> <데이터>
*/

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FILESIZE __INT_MAX__
int main(int args, char * argv[]){
  int fp = open(argv[1],O_RDWR);
  if(fp==-1){
    perror("error");
    exit(1);
  }
  char * buff = (char*)malloc(sizeof(char)*FILESIZE);
  int offset = atoi(argv[2]);
  lseek(fp,offset+1,SEEK_SET);
  read(fp,buff,FILESIZE);


  lseek(fp,offset+1,SEEK_SET);
  write(fp,argv[3],strlen(argv[3]));
  write(fp,buff,strlen(buff));

}