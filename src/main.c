#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 4 // макс потоков по умолчанию

// структура для сортировки в каждом потоке
struct ThreadInfo 
{
    int* array;
    int left;
    int right;
};

void quicksort(int* array, int left, int right){
    int i_left = left;
    int j_right = right;
    int pivot = array[(left + right) / 2];
    
    do
    {
        while(array[i_left] < pivot) i_left++;
        while(array[j_right] > pivot) j_right--;

        if ( i_left <= j_right){
            if (array[i_left] > array[j_right]){
                int temp = array[i_left];
                array[i_left] = array[j_right];
                array[j_right] = temp;
            }
            i_left++;
            j_right--;
        }
    } while (i_left <= j_right);
    
    if (i_left < right)
        quicksort(array, i_left, right);
    if (left < j_right)
        quicksort(array, left, j_right);
}

void* quicksort_thread(void* arg){
    struct ThreadInfo* info = (struct ThreadInfo*)arg;
    quicksort(info->array, info->left, info->right);
    pthread_exit(NULL);
}

void merge(int* array, int left, int right){
    int i, j, k;
    int left_el_num = left;
    int size_of_chunk = right - left + 1;
    int L_array[left_el_num]; // массив для элементов левее куска потока i
    int Chunk_array[size_of_chunk]; // массив для элементов в куске потока i

    for(int i = 0; i < left_el_num; i++){
        L_array[i] = array[i];
    }
    for(int j = 0; j < size_of_chunk; j++){
        Chunk_array[j] = array[left + j];
    }

    // Мешаем массивы для упорядочивания
    i = 0;
    j = 0;
    k = 0;
    while(i < left_el_num & j < size_of_chunk){
        if (L_array[i] <= Chunk_array[j]) {
            array[k] = L_array[i];
            i++;
        } else {
            array[k] = Chunk_array[j];
            j++;
        }
        k++;
    }

    // Копируем оставшиеся элементы L_array
    while(i < left_el_num) {
        array[k] = L_array[i];
        i++;
        k++;
    }

    // Копируем оставшиеся элементы Chunk_array
    while(j < size_of_chunk) {
        array[k] = Chunk_array[j];
        j++;
        k++;
    }

}


int main(int argc, char* argv[]){

    if (argc < 2) {
        printf("Недостаточно аргументов \n");
        return 1;
    }

    int array_size = atoi(argv[1]);

    if (array_size <= 0){
        printf("размер массива не может быть меньше 0 \n");
        return 1;
    }

    int* array = malloc(sizeof(int) * array_size);

    srand(time(NULL));

    // заполняем случайными числами
    for (int i = 0; i < array_size; i++){
        array[i] = rand() % 100;
    }

    printf("Исходный массив \n");
    for(int i = 0; i < array_size; i++){
        printf("%d ", array[i]);
    }
    printf("\n");

    int max_threads = MAX_THREADS; 
    if ( argc == 3){
        max_threads = atoi(argv[2]);
    }

    clock_t start_time = clock();

    struct ThreadInfo thread_info[max_threads];
    pthread_t threads[max_threads];

    // разделяем массив на куски и распределяем их по потокам
    int chunk_size = array_size / max_threads;
    for(int i = 0; i < max_threads; i++){
        thread_info[i].array = array;
        thread_info[i].left = i * chunk_size;
        thread_info[i].right = (i + 1) * chunk_size - 1;
        pthread_create(&threads[i], NULL, quicksort_thread, &thread_info[i]);

    }

    // соединяем результаты в один массив
    for(int i = 0; i < max_threads; i++){
        pthread_join(threads[i], NULL);
        merge(array, thread_info[i].left, thread_info[i].right);
    }

    clock_t end_time = clock();
    double exec_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Отсортированный массив \n");
    for (int i = 0; i < array_size; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");

    printf("Время выполнения программы %f \n", exec_time);

    free(array);

    return 0;
}