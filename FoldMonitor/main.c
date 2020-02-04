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

#define QUIT				'x'				// �ش� Ű �Է����� ���α׷��� �����Ѵ�.
#define CHECK_EXT			".txt"			// �������� ������ Ȯ���ڸ� �����Ѵ�.
#define PATH				"D:/Document"	// ������ ������ ���
#define INIT_FILE_CAPACITY	128				// ����� ������ ù �뷮
#define FILE_PATH_MAX		200				// ������ ����� �ִ����
#define STRING_MAX			200				// �ѹ��� �о�� ������ �ִ����

typedef struct TxtList {
	// ���� ���ϰ�θ� ������ ����.
	char** list;
	// ���� ����� ���ϰ���� ����.
	int count;
	// �ִ� ���尡���� ���ϰ���� ����.
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
	// ������ ������ ��θ� ������ ���� path
	const char* path = PATH;
	// �̹� ����� ������ ������ ���� txtlist
	TxtList txtlist;

	// txtlist�� ������ �ʱ�ȭ�Ѵ�.
	init_txtlist(&txtlist);

	// ������ Ű�Է��� �ޱ������� ���α׷� ����
	while (!quit_check())
	{
		// �������� ���� ���������� ���� dirent����
		dirent** namelist;
		// �������� ���� ������ ������ ���� ����
		int count;

		// ������ ��ĵ�Ͽ� ������ Ȯ���ڸ� ���� ���ϸ� �������� ������ �ݺ���Ż��
		if ((count = scandir(path, &namelist, txt_check, alphasort)) == -1)
		{
			break;
		}

		// ��ϵ� ������ Ȯ���Ͽ� ������ ����Ѵ�.
		print_non_exist_txt(&txtlist, namelist, count, path);

		// namelist�� �����Ҵ�� ���� �����Ѵ�.
		for (int i = 0; i < count; i++)
		{
			free(namelist[i]);
		}
		free(namelist);
	}

	// ��ϵ� ���������� �����Ѵ�.
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
	// Ȯ������ ��ġ�� ã�Ƽ� �����ͷ� �����Ѵ�.
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
	// ������ ���� ������ŭ ������ 2���� �迭�� �����Ҵ��Ѵ�.
	txtlist->list = (char**)malloc(sizeof(char*) * INIT_FILE_CAPACITY);
	// ��ϵ� ������ �������̹Ƿ� 0
	txtlist->count = 0;
	// INIT_FILE_CAPACITY��ŭ �����Ҽ��ִ�.
	txtlist->capacity = INIT_FILE_CAPACITY;
}

void release_txtList(TxtList* txtlist)
{
	// 2�� ������ �̹Ƿ� �� �迭�� ����
	for (int i = 0; i < txtlist->count; i++)
	{
		free(txtlist->list[i]);
	}
	// ���������� 2�������͵� �����Ѵ�.
	free(txtlist->list);
}

void print_non_exist_txt(TxtList* txtlist, const dirent** namelist, int namelist_count, const char* path)
{
	// scandir�� ���� ���ϸ���� Ȯ���Ѵ�.
	for (int i = 0; i < namelist_count; i++)
	{
		// ������ ������ ��ο� �����̸��� ��ģ��.
		char filepath[FILE_PATH_MAX] = { NULL };
		sprintf(filepath, "%s/%s", path, namelist[i]->d_name);

		// �ش� �ؽ�Ʈ ������ ����� �ȵǾ� �ִٸ�
		if (!check_list(txtlist, filepath))
		{
			// ������ ����ϰ�
			file_print(filepath);
			// ����Ʈ�� ����Ѵ�.
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
	// txtlist�� �������ٸ� �뷮�� �÷��ش�.
	if (check_capacity_txtlist(txtlist))
	{
		expand_txtlist(txtlist);
	}

	// ���ϰ�θ� �Է¹޾� �����Ҵ� ���� �迭�� �����Ѵ�.
	char* tmpstr = (char*)malloc(sizeof(char) * (strlen(filepath) + 1));
	if (tmpstr == NULL)
	{
		printf("���� �����Ҵ� ����...\n");
		exit(-1);
	}
	strcpy(tmpstr, filepath);

	// �������� ������ �����ϰ� ����� ������ ���������ش�.
	txtlist->list[txtlist->count++] = tmpstr;
}

void expand_txtlist(TxtList* txtlist)
{
	// ����� ������ 2�踸ŭ �Ҵ����ش�.
	char** tmplist = (char**)malloc(sizeof(char*) * txtlist->capacity * 2);
	if (tmplist == NULL)
	{
		printf("���� �����Ҵ� ����...\n");
		exit(-1);
	}

	// ����Ǿ� �ִ� ������ ���� �����Ѵ�.
	for (int i = 0; i < txtlist->capacity; i++)
	{
		// ����Ǿ� �ִ� string�� ���� + 1��ŭ �Ҵ��Ѵ�.
		char* tmpstr = (char*)malloc(sizeof(char) * (strlen(txtlist->list[i]) + 1));
		if (tmpstr == NULL)
		{
			printf("���� �����Ҵ� ����...\n");
			exit(-1);
		}

		// ������ �ش������� �����Ѵ�.
		strcpy(tmpstr, txtlist->list[i]);
		tmplist[i] = tmpstr;
		// ���� �ִ������� ���������Ƿ� �����Ѵ�.
		free(txtlist->list[i]);
	}
	// ���������� �����Ѵ�.
	free(txtlist->list);

	// ���� 2��� �Ҵ������ ������ �����Ѵ�.
	txtlist->list = tmplist;
	// �뷮�� 2��� �÷����Ƿ� 2�� ������Ų��.
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
	// ������ ���ϰ�η� �б���� ������ ����.
	FILE* file = fopen(filepath, "r");

	if (file != NULL)
	{
		// ������ ��θ� �켱 ������ش�.
		printf("filepath : %s\n", filepath);

		// ������ ������ �Է¹��� ���۸� ����
		char input[STRING_MAX];
		// ������ ���پ� �о� �ܼ�ȭ�鿡 ����Ѵ�.
		while (fgets(input, STRING_MAX, file) != NULL)
		{
			printf("%s", input);
		}
		fclose(file);
		printf("\n\n");
	}
}
