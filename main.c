/*
 * main.c
 *
 *  Created on: 3 de mai de 2017
 *      Author: romulo-farias
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define PATH_FILE_1 "./arquivo1.txt"
#define PATH_FILE_2 "./arquivo2.txt"
#define PATH_FILE_3 "./arquivo3.txt"
#define PATH_FILE_4 "./arquivo4.txt"

struct node
{
  int value;
  struct node *next;
}typedef list;

void perform_fifo(int file_number, char *file_name, int pages_number);
int exists(int value, list *memory);
list* create_page(int value);
list* last_page(list *memory);
list* read_file(char * file_name);

int main(int argc, char **argv)
{
  clock_t t1, t2;

  t1 = clock();

  perform_fifo(1, PATH_FILE_1, 4);
  perform_fifo(2, PATH_FILE_2, 4);
  perform_fifo(3, PATH_FILE_3, 64);
  perform_fifo(3, PATH_FILE_3, 256);
  perform_fifo(3, PATH_FILE_3, 1024);
  perform_fifo(3, PATH_FILE_3, 2048);
  perform_fifo(4, PATH_FILE_4, 64);
  perform_fifo(4, PATH_FILE_4, 256);
  perform_fifo(4, PATH_FILE_4, 1024);
  perform_fifo(4, PATH_FILE_4, 2048);

  t2 = clock();

  printf("Total time: %d\n", t2 - t1);

  return EXIT_SUCCESS;
}

void perform_fifo(int file_number, char *file_name, int pages_number)
{
  list *memory = NULL;
  list *last;
  list *first;
  list *pages = read_file(file_name);
  int requests = 0;
  int page_value = 0;
  int page_faults = 0;
  int alocated_pages = 0;

  while (pages != NULL)
  {
    page_value = pages->value;
    pages = pages->next;
    requests++;

        if (memory == NULL)
        {
          memory = create_page(page_value);
          alocated_pages++;
          continue;
        }

        if (exists(page_value, memory))
          continue;

        if (alocated_pages < pages_number)
        {
          last = last_page(memory);
          last->next = create_page(page_value);
          alocated_pages++;
        }
        else
        {
          first = memory;
          memory = first->next;
          free(first);

          last = last_page(memory);
          last->next = create_page(page_value);
          page_faults++;
        }
  }

  printf("Arquivo %d, %5d páginas, %7d requisições: FIFO: %7d Page Faults\n", file_number, pages_number, requests,
      page_faults);
}

int exists(int value, list *memory)
{
  list *node = memory;
  while (node != NULL)
  {
    if (node->value == value)
      return 1;

    node = node->next;
  }

  return 0;
}

list* create_page(int value)
{
  list *page = malloc(sizeof(page));
  page->value = value;
  page->next = NULL;
  return page;
}

list* last_page(list *memory)
{
  if (memory == NULL)
    return NULL;

  list *node = memory;
  while (node->next != NULL)
  {
    node = node->next;
  }

  return node;
}

list* read_file(char *file_name)
{
  FILE *file;
  int value = 0;
  char req[10];

  file = fopen(file_name, "r");
  list *page = malloc(sizeof(list));
  list *head = page;
  list *predecessor = page;
  head->value = value;

  fgets(req, 9, file);
  value = atoi(req);
  head->value = value;
  head->next = NULL;

  while (!feof(file))
  {
    fgets(req, 9, file);
    if (!feof(file))
    {
      value = atoi(req);
      page = malloc(sizeof(list));
      page->value = value;

      predecessor->next = page;
      predecessor = page;
      page->next = NULL;
    }
  }

  fclose(file);

  return head;
}
