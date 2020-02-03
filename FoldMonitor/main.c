/*
  filename - main.c
  version - 1.0
  description - �⺻ ���� �Լ�
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

// QUIT�� ������ Ű�Է��� �´ٸ� 1�� ��ȯ�Ѵ�.
int quit_check();
// �ؽ�Ʈ ����Ȯ���� ���� �Լ�
int txt_check(const dirent* info);
// TxtList�� �ʱ�ȭ �Ѵ�.
void init_txtlist(TxtList* txtlist);
// TxtList�� �����Ѵ�.
void release_txtList(TxtList* txtlist);
// ���� ������ ���ϸ���Ʈ�� ��������ϸ���Ʈ�� ���Ѵ�.
void print_non_exist_txt(TxtList* txtlist, const dirent** namelist, int namelist_count, const char* path);
// ����Ʈ�� �����ϴ��� Ȯ���Ѵ�.
int check_list(const TxtList* txtlist, const char* filepath);
// TxtList�� ������ Ȯ���ϸ� ���ڿ��� �߰��Ѵ�
void append_txtlist(TxtList* txtlist, const char* filepath);
// TxtList�� Capacity�� 2�辿 �ø���.
void expand_txtlist(TxtList* txtlist);
// TxtList�� �뷮�� ������ Ȯ���Ѵ�.
int check_capacity_txtlist(const TxtList* txtlist);
// �Է¹��� ���ϰ�η� ������ ����Ѵ�.
void file_print(char* filepath);

// �����Լ�
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
	// Ű �Է��� Ȯ���Ѵ�.
	if (_kbhit())
	{
		// �Է��� ���� ��� ���۸� �о� �����´�.
		char input = _getch();

		// ������ ������ ��� ����
		if (input == QUIT)
		{
			return 1;
		}
	}

	// �ƴҰ�� �������.
	return 0;
}

int txt_check(const dirent* info)
{
	char* ext = strrchr(info->d_name, '.');

	if (ext == NULL)
	{
		return 0; /* Ȯ���ڰ� ������ skip�� */
	}

	if (strcmp(ext, CHECK_EXT) == 0)
	{
		return 1; /* ��Ͽ� ���� ��Ŵ */
	}
	else {
		return 0; /* ��Ͽ� ���� ��Ű�� ���� */
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
		// �ش� �ؽ�Ʈ ������ ����� �ȵǾ� �ִٸ�
		// ��� �� ����Ѵ�.
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
			// �̹� ���� �����̸� 1
			return 1;
		}
	}
	// ���� ���� ���� �����̸� 0
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
		printf("���� �����Ҵ� ����...\n");
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
		printf("���� �����Ҵ� ����...\n");
		exit(-1);
	}

	for (int i = 0; i < txtlist->capacity; i++)
	{
		char* tmpstr = (char*)malloc(sizeof(char) * (strlen(txtlist->list[i]) + 1));
		if (tmpstr == NULL)
		{
			printf("���� �����Ҵ� ����...\n");
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
	// �뷮�� ���� ���ٸ� 1
	if (txtlist->capacity == txtlist->count)
		return 1;

	// ���������� �ִٸ� 0
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
