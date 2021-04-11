/*
  학부 : 컴퓨터학부
  학번 : 20172612
  성명 : 김영도

주어진 두 개의 파일, 파일2와 파일3을 병합한 파일1을 생성한다. 
병합은 파일2와 파일3의 순서를 따른다.

a.out <파일명1> <파일명2> <파일명3>
*/
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FILESIZE 1024
int main(int args, char *argv[])
{
  // <파일명 1>
  int des = open(argv[1], O_WRONLY | O_TRUNC | O_CREAT);
  if (des == -1)
  {
    perror("error");
    exit(1);
  }
  char *buff = (char *)malloc(sizeof(char) * FILESIZE);

  int fp = 0;
  for (int i = 2; i < args; i++)
  {
    fp = open(argv[i], O_RDONLY);
    if (fp == -1)
    {
      perror("error");
      exit(1);
    }
    read(fp, buff, FILESIZE);
    write(des, buff, strlen(buff));
  }
  close(des);
  close(fp);

  return 0;
}