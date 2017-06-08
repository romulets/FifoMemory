/******************************************************************************
 * file: main.c 2017-05-31
 * authors: Rômulo M. Farias and Felipe Pereira
 * bachelor: Information Systems
 *******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define PATH_FILE_1 "/home/romulo-farias/Documents/development/c/fifo_memory/resources/arquivo1.txt"
#define PATH_FILE_2 "/home/romulo-farias/Documents/development/c/fifo_memory/resources/arquivo2.txt"
#define PATH_FILE_3 "/home/romulo-farias/Documents/development/c/fifo_memory/resources/arquivo3.txt"
#define PATH_FILE_4 "/home/romulo-farias/Documents/development/c/fifo_memory/resources/arquivo4.txt"

struct node
{
  int value;
  unsigned age;
  struct node *next;
}typedef list;

void perform_nfu(int file_number, char *file_name, int pages_number, int bits_num);
void free_list(list *head);
void age_list(list *head);
list* older_page(list *head);
list* exists(int value, list *head);
list* create_page(int value, unsigned initial_age);
list* read_file(char * file_name);

int main(int argc, char **argv)
{
  clock_t t1, t2;

  t1 = clock();

  perform_nfu(1, PATH_FILE_1, 4, 4);
  perform_nfu(2, PATH_FILE_2, 4, 4);
  perform_nfu(3, PATH_FILE_3, 64, 8);
  perform_nfu(3, PATH_FILE_3, 256, 8);
  perform_nfu(3, PATH_FILE_3, 1024, 8);
  perform_nfu(3, PATH_FILE_3, 2048, 8);
  perform_nfu(4, PATH_FILE_4, 64, 8);
  perform_nfu(4, PATH_FILE_4, 256, 8);
  perform_nfu(4, PATH_FILE_4, 1024, 8);
  perform_nfu(4, PATH_FILE_4, 2048, 8);

  t2 = clock();

  printf("\n\nTotal time: %d\n", t2 - t1);

  return EXIT_SUCCESS;
}

void perform_nfu(int file_number, char *file_name, int pages_number, int bits_num)
{
  unsigned initial_age = 1;
  bits_num -= 1;
  initial_age = initial_age << bits_num;

  list *memory = NULL;
  list *tail;
  list *head = NULL;
  list *removed_page;
  list *pages = read_file(file_name);
  int requests = 0;
  int page_value = 0;
  int page_faults = 0;
  int alocated_pages = 0;

  while (pages != NULL)
  {
    //printf("\n");
    if (pages_number == 64)
    {
      if (requests % 20 == 0)
        age_list(head);
    }
    else if (pages_number == 256)
    {
      if (requests % 100 == 0)
        age_list(head);
    }
    else if (pages_number == 1024)
    {
      if (requests % 400 == 0)
        age_list(head);
    }
    else if (pages_number == 2048)
    {
      if (requests % 800 == 0)
        age_list(head);
    } else {
      age_list(head);
    }

    page_value = pages->value;
    removed_page = pages;
    pages = pages->next;
    free(removed_page);

    requests++;

    if (head == NULL)
    {
      head = create_page(page_value, initial_age);
      tail = head;
      alocated_pages++;
      continue;
    }

    memory = exists(page_value, head);

    if (memory != NULL)
    {
      unsigned old = memory->age;
      memory->age = memory->age | initial_age;
      //printf ("\n\n [ NEW ] page %d  is going from %d to %d! \n", memory->value, old, memory->age);
    }
    else if (alocated_pages < pages_number)
    {
      tail->next = create_page(page_value, initial_age);
      tail = tail->next;
      alocated_pages++;
    }
    else
    {
      memory = older_page(head);
      //printf ("\n\n [ OLD (%d) ] %d wants to go in and %d is the oldest page (age = %d)\n", page_faults, page_value, memory->value, memory->age);
//      //printf ("--------------------------------------------------------------------------------------\n");
      memory->age = initial_age;
      memory->value = page_value;
      page_faults++;
    }
  }

  free_list(head);

  printf ("%5d páginas, %7d requisições: NFU: %5d Page Faults\n", pages_number, requests, page_faults);
}

void free_list(list *head)
{
  list *memory;

  while (head != NULL)
  {
    memory = head;
    free(head);
    head = memory->next;
  }
}

void age_list(list *head)
{
  list *copy = head;

  while (copy != NULL)
  {
    //printf(" %d(%d) ", copy->value, copy->age);

    copy->age = copy->age >> 1;
    copy = copy->next;
  }
}


list* older_page(list *head)
{
  list *copy = head;
  list *older = head;

  while (copy != NULL)
  {
    if (copy->age < older->age)
    {
      older = copy;
    }

    copy = copy->next;
  }

  return older;
}

list* exists(int value, list *head)
{
  list *node = head;
  while (node != NULL)
  {
    if (node->value == value)
      return node;

    node = node->next;
  }
  return NULL;
}

list* create_page(int value, unsigned initial_age)
{
  list *page = malloc(sizeof(list));
  page->value = value;
  page->next = NULL;
  page->age = initial_age;
  return page;
}

list* read_file(char *file_name)
{
  FILE *file;
  int value = 0;
  char req[10];

  file = fopen(file_name, "r");

  if (!file)
  {
    //printf("File %s not found\n", file_name);
    return NULL;
  }

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
