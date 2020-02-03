/*
  filename - main.c
  version - 1.0
  description - 기본 메인 함수
  --------------------------------------------------------------------------------
  first created - 2020.02.03
  writer - Karunio
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "include/dirent.h"

#define QUIT				'x'
#define CHECK_EXT			".txt"
#define PATH				"D:/Document"
#define INIT_FILE_CAPACITY	128	
#define FILE_PATH_MAX		200

typedef struct TxtList {
	char** list;
	int count;
	int capacity;
}TxtList;

// QUIT에 지정한 키입력이 온다면 1을 반환한다.
int quit_check();
// 텍스트 파일확인을 위한 함수
int txt_check(const dirent* info);
// TxtList를 초기화 한다.
void init_txtlist(TxtList* txtlist);
// TxtList를 해제한다.
void release_txtList(TxtList* txtlist);
// 읽은 폴더의 파일리스트와 출력한파일리스트를 비교한다.
void print_non_exist_txt(TxtList* txtlist, const dirent** namelist, int namelist_count, const char* path);
// 리스트에 존재하는지 확인한다.
int check_list(const TxtList* txtlist, const char* filepath);
// TxtList의 공간을 확인하며 문자열을 추가한다
void append_txtlist(TxtList* txtlist, const char* filepath);
// TxtList의 Capacity를 2배씩 늘린다.
void expand_txtlist(TxtList* txtlist);
// TxtList의 용량이 찻는지 확인한다.
int check_capacity_txtlist(const TxtList* txtlist);
// 입력받은 파일경로로 파일을 출력한다.
void file_print(char* filepath);

// 메인함수
int main(int argc, char* args[])
{
	const char* path = PATH;
	TxtList txtlist;

	init_txtlist(&txtlist);

	while (!quit_check())
	{
		dirent** namelist;
		int count;

		if ((count = scandir(path, &namelist, txt_check, alphasort)) == -1)
		{
			break;
		}

		print_non_exist_txt(&txtlist, namelist, count, path);

		for (int i = 0; i < count; i++)
		{
			free(namelist[i]);
		}
		free(namelist);
	}
	release_txtList(&txtlist);

	return EXIT_SUCCESS;
}

int quit_check()
{
	// 키 입력을 확인한다.
	if (_kbhit())
	{
		// 입력이 있을 경우 버퍼를 읽어 가져온다.
		char input = _getch();

		// 지정된 문자일 경우 종료
		if (input == QUIT)
		{
			return 1;
		}
	}

	// 아닐경우 종료안함.
	return 0;
}

int txt_check(const dirent* info)
{
	char* ext = strrchr(info->d_name, '.');

	if (ext == NULL)
	{
		return 0; /* 확장자가 없으면 skip함 */
	}

	if (strcmp(ext, CHECK_EXT) == 0)
	{
		return 1; /* 목록에 포함 시킴 */
	}
	else {
		return 0; /* 목록에 포함 시키지 않음 */
	}
}

void init_txtlist(TxtList* txtlist)
{
	txtlist->list = (char**)malloc(sizeof(char*) * INIT_FILE_CAPACITY);
	txtlist->count = 0;
	txtlist->capacity = INIT_FILE_CAPACITY;
}

void release_txtList(TxtList* txtlist)
{
	for (int i = 0; i < txtlist->count; i++)
	{
		free(txtlist->list[i]);
	}
	free(txtlist->list);
}

void print_non_exist_txt(TxtList* txtlist, const dirent** namelist, int namelist_count, const char* path)
{
	for (int i = 0; i < namelist_count; i++)
	{
		// 해당 텍스트 파일이 등록이 안되어 있다면
		// 등록 및 출력한다.
		char filepath[FILE_PATH_MAX] = { NULL };
		sprintf(filepath, "%s/%s", path, namelist[i]->d_name);
		if (!check_list(txtlist, filepath))
		{
			file_print(filepath);
			append_txtlist(txtlist, filepath);
		}
	}
}

int check_list(const TxtList* txtlist, const char* target)
{
	for (int i = 0; i < txtlist->count; i++)
	{
		if (strcmp(txtlist->list[i], target) == 0)
		{
			// 이미 읽은 파일이면 1
			return 1;
		}
	}
	// 아직 읽지 않은 파일이면 0
	return 0;
}

void append_txtlist(TxtList* txtlist, const char* filepath)
{
	if (check_capacity_txtlist(txtlist))
	{
		expand_txtlist(txtlist);
	}

	char* tmpstr = (char*)malloc(sizeof(char) * (strlen(filepath) + 1));
	if (tmpstr == NULL)
	{
		printf("파일 동적할당 오류...\n");
		exit(-1);
	}
	strcpy(tmpstr, filepath);
	txtlist->list[txtlist->count++] = tmpstr;
}

void expand_txtlist(TxtList* txtlist)
{
	char** tmplist = (char**)malloc(sizeof(char*) * txtlist->capacity * 2);
	if (tmplist == NULL)
	{
		printf("파일 동적할당 오류...\n");
		exit(-1);
	}

	for (int i = 0; i < txtlist->capacity; i++)
	{
		char* tmpstr = (char*)malloc(sizeof(char) * (strlen(txtlist->list[i]) + 1));
		if (tmpstr == NULL)
		{
			printf("파일 동적할당 오류...\n");
			exit(-1);
		}
		strcpy(tmpstr, txtlist->list[i]);
		tmplist[i] = tmpstr;
		free(txtlist->list[i]);
	}
	free(txtlist->list);

	txtlist->list = tmplist;
	txtlist->capacity *= 2;
}

int check_capacity_txtlist(const TxtList* txtlist)
{
	// 용량이 가득 찻다면 1
	if (txtlist->capacity == txtlist->count)
		return 1;

	// 여유공간이 있다면 0
	return 0;
}

void file_print(char* filepath)
{
	FILE* file = fopen(filepath, "r");

	if (file != NULL)
	{
		printf("filepath : %s\n", filepath);
		char input;
		while (!feof(file))
		{
			input = fgetc(file);
			putchar(input);
		}
		fclose(file);
		printf("\n\n");
	}
}
