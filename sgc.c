#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


typedef enum _object_type {
    STRING,
    INT,
} object_type;


typedef struct _object {
    void *data;
    object_type type;
    bool marked;
    struct _object* next;
} object;


object* head = NULL;
object** free_list = NULL;
int num_free_list = 0;


object* new_object(void* data, object_type type)
{
    object* obj = (object*) malloc(sizeof(object));
    obj->data = data;
    obj->marked = false;
    obj->next = NULL;

    if (head == NULL) {
        head = obj;
    } else {
        object* ptr = head;
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = obj;
    }

    free_list = (object**) realloc(free_list, sizeof(object*) * (num_free_list + 1));
    free_list[num_free_list++] = obj;
    
    return obj;
}


void mark(object* obj)
{
    object* current = obj;

    while (current != NULL) {
        if (current->marked) {
            break;
        }
        current->marked = true;

        current = current->next;
    }
}


void sweep()
{
    int deleted = 0;
    for (size_t i = 0; i < num_free_list; i++) {
        if (!free_list[i]->marked) {
            printf("%p was freed by the garbage collector.\n", free_list[i]);

            object* tmp = free_list[i];
            free_list[i] = free_list[i + 1];

            free(tmp);
            deleted++;
        }
    }

    num_free_list -= deleted;
    free_list = (object**) realloc(free_list, sizeof(object*) * (num_free_list));
}


void gc()
{
    for (object* current = head; current != NULL; current = current->next) {
        if (current->marked) {
            break;
        }
        mark(current);
    }
    sweep();
}


int main()
{
    int a = 1, b = 2, c = 3;

    object* obj1 = new_object(&a, INT);
    object* obj2 = new_object(&b, INT);
    object* obj3 = new_object(&c, INT);

    // Delete `obj2`
    obj1->next = obj3;

    printf("obj1: %p\n", obj1);
    printf("obj2: %p\n", obj2);
    printf("obj3: %p\n", obj3);

    gc();

    /* Clean up */
    for (size_t i = 0; i < num_free_list; i++) {
        free(free_list[i]);
    }

    free(free_list);

    return 0;
}

