/*
  filename - main.c
  version - 1.0
  description - Watch Folder If txt file added than display txt content
  --------------------------------------------------------------------------------
  first created - 2020.02.03
  writer - Karunio
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "include/dirent.h"

#define QUIT				'x'				// 해당 키 입력으로 프로그램을 종료한다.
#define CHECK_EXT			".txt"			// 폴더에서 감시할 확장자를 지정한다.
#define PATH				"D:/Document"	// 감시할 폴더의 경로
#define INIT_FILE_CAPACITY	128				// 등록할 파일의 첫 용량
#define FILE_PATH_MAX		200				// 파일의 경로의 최대길이
#define STRING_MAX			200				// 한번에 읽어올 한줄의 최대길이

typedef struct TxtList {
	// 여러 파일경로를 저장할 변수.
	char** list;
	// 현재 저장된 파일경로의 갯수.
	int count;
	// 최대 저장가능한 파일경로의 갯수.
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
	// 감시할 폴더의 경로를 저장할 변수 path
	const char* path = PATH;
	// 이미 출력할 파일의 정보를 담을 txtlist
	TxtList txtlist;

	// txtlist의 정보를 초기화한다.
	init_txtlist(&txtlist);

	// 지정된 키입력을 받기전까지 프로그램 가동
	while (!quit_check())
	{
		// 폴더에서 읽은 파일정보를 담을 dirent변수
		dirent** namelist;
		// 폴더에서 읽은 파일의 갯수를 담은 변수
		int count;

		// 폴더를 스캔하여 지정된 확장자를 가진 파일만 가져오며 오류시 반복문탈출
		if ((count = scandir(path, &namelist, txt_check, alphasort)) == -1)
		{
			break;
		}

		// 등록된 정보를 확인하여 없으면 출력한다.
		print_non_exist_txt(&txtlist, namelist, count, path);

		// namelist의 동적할당된 램을 해제한다.
		for (int i = 0; i < count; i++)
		{
			free(namelist[i]);
		}
		free(namelist);
	}

	// 등록된 파일정보를 해제한다.
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
	// 확장자의 위치를 찾아서 포인터로 저장한다.
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
	// 지정된 파일 갯수만큼 저장할 2차원 배열을 동적할당한다.
	txtlist->list = (char**)malloc(sizeof(char*) * INIT_FILE_CAPACITY);
	// 등록된 파일이 없을것이므로 0
	txtlist->count = 0;
	// INIT_FILE_CAPACITY만큼 저장할수있다.
	txtlist->capacity = INIT_FILE_CAPACITY;
}

void release_txtList(TxtList* txtlist)
{
	// 2중 포인터 이므로 각 배열을 해제
	for (int i = 0; i < txtlist->count; i++)
	{
		free(txtlist->list[i]);
	}
	// 마지막으로 2중포인터도 해제한다.
	free(txtlist->list);
}

void print_non_exist_txt(TxtList* txtlist, const dirent** namelist, int namelist_count, const char* path)
{
	// scandir로 받은 파일목록을 확인한다.
	for (int i = 0; i < namelist_count; i++)
	{
		// 감시할 폴더의 경로와 파일이름을 합친다.
		char filepath[FILE_PATH_MAX] = { NULL };
		sprintf(filepath, "%s/%s", path, namelist[i]->d_name);

		// 해당 텍스트 파일이 등록이 안되어 있다면
		if (!check_list(txtlist, filepath))
		{
			// 파일을 출력하고
			file_print(filepath);
			// 리스트에 등록한다.
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
	// txtlist가 가득찻다면 용량을 늘려준다.
	if (check_capacity_txtlist(txtlist))
	{
		expand_txtlist(txtlist);
	}

	// 파일경로를 입력받아 동적할당 받은 배열에 복사한다.
	char* tmpstr = (char*)malloc(sizeof(char) * (strlen(filepath) + 1));
	if (tmpstr == NULL)
	{
		printf("파일 동적할당 오류...\n");
		exit(-1);
	}
	strcpy(tmpstr, filepath);

	// 복사한후 정보를 저장하고 저장된 갯수를 증가시켜준다.
	txtlist->list[txtlist->count++] = tmpstr;
}

void expand_txtlist(TxtList* txtlist)
{
	// 저장된 공간의 2배만큼 할당해준다.
	char** tmplist = (char**)malloc(sizeof(char*) * txtlist->capacity * 2);
	if (tmplist == NULL)
	{
		printf("파일 동적할당 오류...\n");
		exit(-1);
	}

	// 저장되어 있는 정보를 돌며 복사한다.
	for (int i = 0; i < txtlist->capacity; i++)
	{
		// 저장되어 있는 string의 길이 + 1만큼 할당한다.
		char* tmpstr = (char*)malloc(sizeof(char) * (strlen(txtlist->list[i]) + 1));
		if (tmpstr == NULL)
		{
			printf("파일 동적할당 오류...\n");
			exit(-1);
		}

		// 복사후 해당정보를 저장한다.
		strcpy(tmpstr, txtlist->list[i]);
		tmplist[i] = tmpstr;
		// 원래 있던정보를 복사했으므로 해제한다.
		free(txtlist->list[i]);
	}
	// 마찬가지로 해제한다.
	free(txtlist->list);

	// 새로 2배로 할당시켜준 정보를 저장한다.
	txtlist->list = tmplist;
	// 용량을 2배로 늘렸으므로 2배 증가시킨다.
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
	// 지정된 파일경로로 읽기모드로 파일을 연다.
	FILE* file = fopen(filepath, "r");

	if (file != NULL)
	{
		// 파일의 경로를 우선 출력해준다.
		printf("filepath : %s\n", filepath);

		// 파일의 내용을 입력받을 버퍼를 정의
		char input[STRING_MAX];
		// 파일을 한줄씩 읽어 콘솔화면에 출력한다.
		while (fgets(input, STRING_MAX, file) != NULL)
		{
			printf("%s", input);
		}
		fclose(file);
		printf("\n\n");
	}
}
