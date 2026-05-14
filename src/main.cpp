#ifdef XXMM
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"   // for esp_timer_get_time()

void logAllTaskStacks() {
    printf("===== Task Stack Usage =====\n");

    TaskStatus_t *taskArray;
    UBaseType_t taskCount = uxTaskGetSystemState(NULL, 0, NULL);

    taskArray = (TaskStatus_t *)malloc(taskCount * sizeof(TaskStatus_t));
    if(taskArray != NULL) {
        UBaseType_t arraySize = uxTaskGetSystemState(taskArray, taskCount, NULL);
        for(UBaseType_t i = 0; i < arraySize; i++) {
            printf("Task: %s | Stack High Water Mark: %lu | Current Priority: %u\n",
                   taskArray[i].pcTaskName,
                   (unsigned long)taskArray[i].usStackHighWaterMark,
                   (unsigned int)taskArray[i].uxCurrentPriority);
        }
        free(taskArray);
    }
    printf("============================\n");
}

void logMemoryStats() {
    printf("===== ESP32 Memory Stats =====\n");
    printf("Free heap: %lu bytes\n", (unsigned long)esp_get_free_heap_size());
    printf("Minimum free heap ever: %lu bytes\n", (unsigned long)esp_get_minimum_free_heap_size());
    printf("Largest free block: %lu bytes\n", (unsigned long)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    printf("Free internal RAM: %lu bytes\n", (unsigned long)heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    printf("Free external RAM (PSRAM): %lu bytes\n", (unsigned long)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    printf("==============================\n");

    logAllTaskStacks();
}

extern "C" void app_main() {
    printf("Starting memory logger...\n");

    while (true) {
        static int64_t lastLog = 0;
        int64_t now = esp_timer_get_time(); // microseconds
        if ((now - lastLog) >= 300000000) { // 5 minutes in µs
            lastLog = now;
            logMemoryStats();
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // sleep 1s
    }
}
#endif