#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TASKS 128

#define TODO 0
#define IN_PROGRESS 1
#define DONE 2

#define LOW 0
#define MEDIUM 1
#define HIGH 2

#define SIMPLE_TASK 0
#define DEADLINE_TASK 1

typedef struct {
    char name[64];
    int priority;
    int status;
    int type;
} Task;

typedef struct {
    Task base;
    int day;
    int month;
    int year;
} DeadlineTask;

typedef struct {
    Task* tasks[MAX_TASKS];
    int count;
} TaskList;

const char* priorityToString(int priority)
{
    switch(priority) {
        case LOW:
            return "LOW";
        case MEDIUM:
            return "MEDIUM";
        case HIGH:
            return "HIGH";
        default:
            return "NONE";
    }
}

const char* statusToString(int status) {
    switch(status) {
        case TODO:
            return "TODO";
        case IN_PROGRESS:
            return "IN_PROGRESS";
        case DONE:
            return "DONE";
        default:
            return "NONE";
    }
}

Task* createTask(const char* name, int priority, int status) {
    Task* task = (Task*)malloc(sizeof(Task));

    strcpy(task->name, name);
    task->priority = priority;
    task->status = status;
    task->type = SIMPLE_TASK;

    return task;
}

DeadlineTask* createDeadlineTask(const char* name, int priority, int status, int day, int month, int year) {
    DeadlineTask* task = (DeadlineTask*)malloc(sizeof(DeadlineTask));

    strcpy(task->base.name, name);

    task->base.priority = priority;
    task->base.status = status;
    task->base.type = DEADLINE_TASK;

    task->day = day;
    task->month = month;
    task->year = year;

    return task;
}

void printTaskInfo(Task* task) {
    if(task->type == SIMPLE_TASK) {
        printf("%s | Приоритет: %s | Статус: %s\n",
            task->name,
            priorityToString(task->priority),
            statusToString(task->status));
    }
    else {
        DeadlineTask* dt = (DeadlineTask*)task;

        printf("%s | Приоритет: %s | Статус: %s | Дедлайн: %d-%d-%d\n",
            dt->base.name,
            priorityToString(dt->base.priority),
            statusToString(dt->base.status),
            dt->day,
            dt->month,
            dt->year);
    }
}

int isCompleted(Task* task) {
    return task->status == DONE;
}

int isOverdue(Task* task) {
    if(task->type == SIMPLE_TASK) {
        return 0;
    }

    DeadlineTask* dt = (DeadlineTask*)task;

    time_t now = time(NULL);
    struct tm* current = localtime(&now);

    int currentDay = current->tm_mday;
    int currentMonth = current->tm_mon + 1;
    int currentYear = current->tm_year + 1900;
    
    if(dt->year < currentYear) {
        return 1;
    }
    
    if(dt->year == currentYear && dt->month < currentMonth) {
        return 1;
    }
    
    if(dt->year == currentYear && dt->month == currentMonth && dt->day < currentDay) {
        return 1;
    }
    
    return 0;
}

void addTask(TaskList* list, Task* task) {
    if(list->count < MAX_TASKS) {
        list->tasks[list->count++] = task;
    }
}

void printAllTasks(TaskList* list) {
    printf("\nВсе задачи\n");

    for(int i = 0; i < list->count; i++) {
        printTaskInfo(list->tasks[i]);
    }
}

void filterByStatus(TaskList* list, int status) {
    printf("\nОтсортированные задачи\n");

    for(int i = 0; i < list->count; i++) {
        if(list->tasks[i]->status == status) {
            printTaskInfo(list->tasks[i]);
        }
    }
}

Task* findHighestPriorityTask(TaskList* list) {
    if(list->count == 0) {
        return NULL;
    }

    Task* best = list->tasks[0];

    for(int i = 1; i < list->count; i++) {
        if(list->tasks[i]->priority > best->priority) {
            best = list->tasks[i];
        }
    }
    
    return best;
}

void sortByPriority(TaskList* list) {
    for(int i = 0; i < list->count - 1; i++) {
        for(int j = i + 1; j < list->count; j++) {
            if(list->tasks[i]->priority < list->tasks[j]->priority) {
                Task* temp = list->tasks[i];
                list->tasks[i] = list->tasks[j];
                list->tasks[j] = temp;
            }
        }
    }
    printAllTasks(list);
}

void showOverdueTasks(TaskList* list) {
    printf("\nПросроченные задачи\n");

    for(int i = 0; i < list->count; i++) {
        if(isOverdue(list->tasks[i]) && list->tasks[i]->status != DONE) {
            printTaskInfo(list->tasks[i]);
        }
    }
}

void changeTaskStatus(Task* task, int newStatus) {
    task->status = newStatus;
}

void freeTasks(TaskList* list) {
    for(int i = 0; i < list->count; i++) {
        free(list->tasks[i]);
    }
}

int main() {
    TaskList list;
    list.count = 0;
    
    // Добавление задач
    addTask(&list, createTask("Первая задача", MEDIUM, TODO));
    addTask(&list, (Task*)createDeadlineTask("Вторая задача", HIGH, IN_PROGRESS, 15, 5, 2026));
    addTask(&list, (Task*)createDeadlineTask("Третья задача", HIGH, DONE, 10, 1, 2024));
    addTask(&list, createTask("Четвертая задача", LOW, DONE));
    addTask(&list, (Task*)createDeadlineTask("Пятая задача", MEDIUM, IN_PROGRESS, 10, 7, 2026));
    
    // Вывод всех задач
    printAllTasks(&list);
    
    // Сортировка по приоритету
    sortByPriority(&list);

    // Сортировка по статусу DONE
    filterByStatus(&list, DONE);

    // Сортировка по наивысшему приоритету
    Task* best = findHighestPriorityTask(&list);
    
    // Чтобы не убило, если не найдет
    if(best != NULL) {
        printTaskInfo(best);
    }
    
    // Сортировка по просроченным задачам
    showOverdueTasks(&list);
    
    // Очистка
    freeTasks(&list);
}
