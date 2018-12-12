#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "systime.h"

#include "thread_pool.h"

static void* write_file(void* argv)
{
    char path[128] = {0};
    int file_index = *((int*)argv);

    //please check there is test directory under your current directory
    snprintf(path, 128, "test/%d.txt", file_index);

    FILE* file_ptr = fopen(path, "w+");
    if (file_ptr != NULL)
    {
        char *str="hello,I am a test program!\r\n";
        for(int i = 0; i < 100; i++)
        {
            fwrite(str, sizeof(char), strlen(str), file_ptr);
        }

        fclose(file_ptr);
    }
    else
    {
        printf("write file failed.\n");
    }

    return NULL;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("usage: %s count.\n", argv[0]);
        return -1;
    }

    int task_count = atoi(argv[1]);

    time_t start = get_current_timestamp();

    threadpool_t* pool = threadpool_create(8, 65535, 0);

    int* file_index = (int*)malloc(sizeof(int) * task_count);
    for (int i = 0; i < task_count; i ++)
    {
        file_index[i] = i;
    }

    for (int i = 0 ; i < task_count; i ++)
    {
        if(0 != threadpool_add(pool, write_file, (void*)&file_index[i]))
        {
            printf("add task to thread pool failed.\n");
        }
    }

#if 1
    if (0 != threadpool_destroy(pool, graceful_shutdown))
    {
        printf("destroy thread pool failed.\n");
    }
#endif

    time_t end = get_current_timestamp();
    printf("cost: %d\n", end - start);

    for(;;);

    return 0;
}


