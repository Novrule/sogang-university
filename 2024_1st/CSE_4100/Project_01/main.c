#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitmap.h"
#include "hash.h"
#include "list.h"

int main() {
  char input[100];
  char **command = (char **)malloc(sizeof(char *) * 500);

  BITMAP *bitmap = (BITMAP *)malloc(sizeof(BITMAP) * 100);
  HASH *hash = (HASH *)malloc(sizeof(HASH) * 100);
  LIST *list = (LIST *)malloc(sizeof(LIST) * 100);

  while (1) {
    fgets(input, 500, stdin);
    input[strlen(input) - 1] = '\0';

    int idx = 0;
    char *str = strtok(input, " ");

    while (str != NULL) {
      command[idx++] = str;
      str = strtok(NULL, " ");
    }

    if (!strcmp(command[0], "quit")) {  // 1. 공통 명령어
      break;
    } else if (!strcmp(command[0], "create")) {
      if (!strcmp(command[1], "list")) {
        char *name = command[2];
        int listIndex = name[strlen(name) - 1] - '0';

        list_init(&list[listIndex]);
      } else if (!strcmp(command[1], "hashtable")) {
        char *name = command[2];
        int hashIndex = name[strlen(name) - 1] - '0';

        hash_init(&hash[hashIndex], hash_func, hash_cmp, NULL);
      } else if (!strcmp(command[1], "bitmap")) {
        char *name = command[2];
        int bitmapIndex = name[strlen(name) - 1] - '0';
        size_t size = atoi(command[3]);

        bitmap[bitmapIndex] = *bitmap_create(size);
      }
    } else if (!strcmp(command[0], "delete")) {
      char *name = command[1];

      if (name[0] == 'l') {
        int listIndex = name[strlen(name) - 1] - '0';

        list_destroy(&list[listIndex]);
      } else if (name[0] == 'h') {
        int hashIndex = name[strlen(name) - 1] - '0';

        hash_destroy(&hash[hashIndex], hash_destructor);
      } else if (name[0] == 'b') {
        int bitmapIndex = name[strlen(name) - 1] - '0';

        bitmap_destroy(&bitmap[bitmapIndex]);
      }
    } else if (!strcmp(command[0], "dumpdata")) {
      char *name = command[1];

      if (name[0] == 'l') {
        int listIndex = name[strlen(name) - 1] - '0';

        if (!list_empty(&list[listIndex])) {
          struct list_elem *cur = list_begin(&list[listIndex]);

          while (cur != list_end(&list[listIndex])) {
            struct list_item *item = list_entry(cur, LIST_ITEM, elem);

            printf("%d ", item->data);

            cur = list_next(cur);
          }

          printf("\n");
        }
      } else if (name[0] == 'h') {
        int hashIndex = name[strlen(name) - 1] - '0';

        if (!hash_empty(&hash[hashIndex])) {
          struct hash_iterator it;

          hash_first(&it, &hash[hashIndex]);

          while (hash_next(&it)) {
            int data = hash_entry(hash_cur(&it), HASH_ITEM, elem)->data;

            printf("%d ", data);
          }

          printf("\n");
        }
      } else if (name[0] == 'b') {
        int bitmapIndex = name[strlen(name) - 1] - '0';

        if ((int)(bitmap_size(&bitmap[bitmapIndex]) != 0)) {
          for (int i = 0; i < bitmap_size(&bitmap[bitmapIndex]); i++) {
            if (bitmap_test(&bitmap[bitmapIndex], i)) {
              printf("1");
            } else {
              printf("0");
            }
          }

          printf("\n");
        }
      }
    } else if (!strcmp(command[0], "bitmap_size")) {  // 2. bitmap 명령어
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';

      printf("%zu\n", bitmap_size(&bitmap[bitmapIndex]));
    } else if (!strcmp(command[0], "bitmap_set")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int index = atoi(command[2]);

      if (!strcmp(command[3], "true")) {
        bitmap_set(&bitmap[bitmapIndex], index, true);
      } else {
        bitmap_set(&bitmap[bitmapIndex], index, false);
      }
    } else if (!strcmp(command[0], "bitmap_mark")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int index = atoi(command[2]);

      bitmap_mark(&bitmap[bitmapIndex], index);
    } else if (!strcmp(command[0], "bitmap_reset")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int index = atoi(command[2]);

      bitmap_reset(&bitmap[bitmapIndex], index);
    } else if (!strcmp(command[0], "bitmap_flip")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int index = atoi(command[2]);

      bitmap_flip(&bitmap[bitmapIndex], index);
    } else if (!strcmp(command[0], "bitmap_test")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int index = atoi(command[2]);

      if (bitmap_test(&bitmap[bitmapIndex], index)) {
        printf("true\n");
      } else {
        printf("false\n");
      }
    } else if (!strcmp(command[0], "bitmap_set_all")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';

      if (!strcmp(command[2], "true")) {
        bitmap_set_all(&bitmap[bitmapIndex], true);
      } else {
        bitmap_set_all(&bitmap[bitmapIndex], false);
      }
    } else if (!strcmp(command[0], "bitmap_set_multiple")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int start = atoi(command[2]);
      int range = atoi(command[3]);

      if (!strcmp(command[4], "true")) {
        bitmap_set_multiple(&bitmap[bitmapIndex], start, range, true);
      } else {
        bitmap_set_multiple(&bitmap[bitmapIndex], start, range, false);
      }
    } else if (!strcmp(command[0], "bitmap_count")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int start = atoi(command[2]);
      int range = atoi(command[3]);

      if (!strcmp(command[4], "true")) {
        printf("%zu\n", bitmap_count(&bitmap[bitmapIndex], start, range, true));
      } else {
        printf("%zu\n",
               bitmap_count(&bitmap[bitmapIndex], start, range, false));
      }
    } else if (!strcmp(command[0], "bitmap_contains")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int start = atoi(command[2]);
      int range = atoi(command[3]);

      if (!strcmp(command[4], "true")) {
        if (bitmap_contains(&bitmap[bitmapIndex], start, range, true)) {
          printf("true\n");
        } else {
          printf("false\n");
        }
      } else {
        if (bitmap_contains(&bitmap[bitmapIndex], start, range, false)) {
          printf("true\n");
        } else {
          printf("false\n");
        }
      }
    } else if (!strcmp(command[0], "bitmap_any")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int start = atoi(command[2]);
      int range = atoi(command[3]);

      if (bitmap_any(&bitmap[bitmapIndex], start, range)) {
        printf("true\n");
      } else {
        printf("false\n");
      }
    } else if (!strcmp(command[0], "bitmap_none")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int start = atoi(command[2]);
      int range = atoi(command[3]);

      if (bitmap_none(&bitmap[bitmapIndex], start, range)) {
        printf("true\n");
      } else {
        printf("false\n");
      }
    } else if (!strcmp(command[0], "bitmap_all")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int start = atoi(command[2]);
      int range = atoi(command[3]);

      if (bitmap_all(&bitmap[bitmapIndex], start, range)) {
        printf("true\n");
      } else {
        printf("false\n");
      }
    } else if (!strcmp(command[0], "bitmap_scan")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int start = atoi(command[2]);
      int range = atoi(command[3]);

      if (!strcmp(command[4], "true")) {
        printf("%zu\n", bitmap_scan(&bitmap[bitmapIndex], start, range, true));
      } else {
        printf("%zu\n", bitmap_scan(&bitmap[bitmapIndex], start, range, false));
      }
    } else if (!strcmp(command[0], "bitmap_scan_and_flip")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int start = atoi(command[2]);
      int range = atoi(command[3]);

      if (!strcmp(command[4], "true")) {
        printf("%zu\n",
               bitmap_scan_and_flip(&bitmap[bitmapIndex], start, range, true));
      } else {
        printf("%zu\n",
               bitmap_scan_and_flip(&bitmap[bitmapIndex], start, range, false));
      }
    } else if (!strcmp(command[0], "bitmap_dump")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';

      bitmap_dump(&bitmap[bitmapIndex]);
    } else if (!strcmp(command[0], "bitmap_expand")) {
      char *name = command[1];
      int bitmapIndex = name[strlen(name) - 1] - '0';
      int size = atoi(command[2]);

      bitmap_expand(&bitmap[bitmapIndex], size);
    } else if (!strcmp(command[0], "hash_clear")) {  // 3. hash 명령어
      char *name = command[1];
      int hashIndex = name[strlen(name) - 1] - '0';

      hash_clear(&hash[hashIndex], hash_destructor);
    } else if (!strcmp(command[0], "hash_insert")) {
      char *name = command[1];
      int hashIndex = name[strlen(name) - 1] - '0';
      int data = atoi(command[2]);
      struct hash_item *item =
          (struct hash_item *)malloc(sizeof(struct hash_item *));

      item->data = data;

      hash_insert(&hash[hashIndex], &(item->elem));
    } else if (!strcmp(command[0], "hash_replace")) {
      char *name = command[1];
      int hashIndex = name[strlen(name) - 1] - '0';
      int data = atoi(command[2]);
      struct hash_item *item =
          (struct hash_item *)malloc(sizeof(struct hash_item *));

      item->data = data;

      hash_replace(&hash[hashIndex], &(item->elem));
    } else if (!strcmp(command[0], "hash_find")) {
      char *name = command[1];
      int hashIndex = name[strlen(name) - 1] - '0';
      int data = atoi(command[2]);
      struct hash_item *item =
          (struct hash_item *)malloc(sizeof(struct hash_item *));

      item->data = data;

      if (hash_find(&hash[hashIndex], &(item->elem)) != NULL) {
        int findData = hash_entry(hash_find(&hash[hashIndex], &(item->elem)),
                                  HASH_ITEM, elem)
                           ->data;

        printf("%d\n", findData);
      }
    } else if (!strcmp(command[0], "hash_delete")) {
      char *name = command[1];
      int hashIndex = name[strlen(name) - 1] - '0';
      int data = atoi(command[2]);
      struct hash_item *item =
          (struct hash_item *)malloc(sizeof(struct hash_item *));

      item->data = data;

      hash_delete(&hash[hashIndex], &(item->elem));
    } else if (!strcmp(command[0], "hash_apply")) {
      char *name = command[1];
      char *apply = command[2];
      int hashIndex = name[strlen(name) - 1] - '0';

      if (!strcmp(apply, "square")) {
        hash_apply(&hash[hashIndex], hash_double);
      } else if (!strcmp(apply, "triple")) {
        hash_apply(&hash[hashIndex], hash_triple);
      }
    } else if (!strcmp(command[0], "hash_size")) {
      char *name = command[1];
      int hashIndex = name[strlen(name) - 1] - '0';

      printf("%zu\n", hash_size(&hash[hashIndex]));
    } else if (!strcmp(command[0], "hash_empty")) {
      char *name = command[1];
      int hashIndex = name[strlen(name) - 1] - '0';

      if (hash_empty(&hash[hashIndex])) {
        printf("true\n");
      } else {
        printf("false\n");
      }
    } else if (!strcmp(command[0], "list_insert")) {  // 4. list 명령어
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';
      int index = atoi(command[2]);
      int data = atoi(command[3]);
      struct list_elem *cur = list_begin(&list[listIndex]);
      struct list_item *item =
          (struct list_item *)malloc(sizeof(struct list_item *));

      item->data = data;

      while (index--) {
        cur = list_next(cur);
      }

      list_insert(cur, &(item->elem));
    } else if (!strcmp(command[0], "list_splice")) {
      char *beforeName = command[1];
      int beforeListIndex = beforeName[strlen(beforeName) - 1] - '0';
      int beforeIndex = atoi(command[2]);
      struct list_elem *before = list_begin(&list[beforeListIndex]);

      char *afterName = command[3];
      int afterListIndex = afterName[strlen(afterName) - 1] - '0';
      int firstIndex = atoi(command[4]);
      int lastIndex = atoi(command[5]);
      struct list_elem *first = list_begin(&list[afterListIndex]);
      struct list_elem *last = list_begin(&list[afterListIndex]);

      while (beforeIndex--) {
        before = list_next(before);
      }

      while (firstIndex--) {
        first = list_next(first);
      }

      while (lastIndex--) {
        last = list_next(last);
      }

      list_splice(before, first, last);
    } else if (!strcmp(command[0], "list_push_front")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';
      int data = atoi(command[2]);
      struct list_item *item =
          (struct list_item *)malloc(sizeof(struct list_item *));

      item->data = data;

      list_push_front(&list[listIndex], &(item->elem));
    } else if (!strcmp(command[0], "list_push_back")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';
      int data = atoi(command[2]);
      struct list_item *item =
          (struct list_item *)malloc(sizeof(struct list_item *));

      item->data = data;

      list_push_back(&list[listIndex], &(item->elem));
    } else if (!strcmp(command[0], "list_remove")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';
      int index = atoi(command[2]);
      struct list_elem *cur = list_begin(&list[listIndex]);

      while (index--) {
        cur = list_next(cur);
      }

      list_remove(cur);
    } else if (!strcmp(command[0], "list_pop_front")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';

      list_pop_front(&list[listIndex]);
    } else if (!strcmp(command[0], "list_pop_back")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';

      list_pop_back(&list[listIndex]);
    } else if (!strcmp(command[0], "list_front")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';
      int data =
          list_entry(list_front(&list[listIndex]), LIST_ITEM, elem)->data;

      printf("%d\n", data);
    } else if (!strcmp(command[0], "list_back")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';
      int data = list_entry(list_back(&list[listIndex]), LIST_ITEM, elem)->data;

      printf("%d\n", data);
    } else if (!strcmp(command[0], "list_size")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';

      printf("%zu\n", list_size(&list[listIndex]));
    } else if (!strcmp(command[0], "list_empty")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';

      if (list_empty(&list[listIndex])) {
        printf("true\n");
      } else {
        printf("false\n");
      }
    } else if (!strcmp(command[0], "list_reverse")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';

      list_reverse(&list[listIndex]);
    } else if (!strcmp(command[0], "list_sort")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';

      list_sort(&list[listIndex], list_cmp, NULL);
    } else if (!strcmp(command[0], "list_insert_ordered")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';
      int data = atoi(command[2]);
      struct list_item *item =
          (struct list_item *)malloc(sizeof(struct list_item *));

      item->data = data;

      list_insert_ordered(&list[listIndex], &(item->elem), list_cmp, NULL);
    } else if (!strcmp(command[0], "list_unique")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';

      if (idx < 3) {
        list_unique(&list[listIndex], NULL, list_cmp, NULL);
      } else {
        char *dupName = command[2];
        int dupListIndex = dupName[strlen(dupName) - 1] - '0';

        list_unique(&list[listIndex], &list[dupListIndex], list_cmp, NULL);
      }
    } else if (!strcmp(command[0], "list_max")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';
      int data = list_entry(list_max(&list[listIndex], list_cmp, NULL),
                            LIST_ITEM, elem)
                     ->data;

      printf("%d\n", data);
    } else if (!strcmp(command[0], "list_min")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';
      int data = list_entry(list_min(&list[listIndex], list_cmp, NULL),
                            LIST_ITEM, elem)
                     ->data;

      printf("%d\n", data);
    } else if (!strcmp(command[0], "list_swap")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';
      int aIndex = atoi(command[2]);
      int bIndex = atoi(command[3]);

      struct list_elem *a = list_begin(&list[listIndex]);
      while (aIndex--) {
        a = list_next(a);
      }

      struct list_elem *b = list_begin(&list[listIndex]);
      while (bIndex--) {
        b = list_next(b);
      }

      list_swap(a, b);
    } else if (!strcmp(command[0], "list_shuffle")) {
      char *name = command[1];
      int listIndex = name[strlen(name) - 1] - '0';

      list_shuffle(&list[listIndex]);
    }
  }

  return 0;
}