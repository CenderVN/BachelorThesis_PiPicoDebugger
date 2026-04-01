#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

// Task 1: Fibonacci (The "Math" Task)
void fibonacci_task(void *pvParameters) {
    uint64_t t1 = 0, t2 = 1, next;
    while (true) {
        printf("[Task 1 - Math] Fibonacci: %llu\n", t1);
        next = t1 + t2; t1 = t2; t2 = next;
        if (next < t1) t1 = 0, t2 = 1; // Reset on overflow

        // Yield to let Task 2 run
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}

// Task 2: Status (The "Heartbeat" Task)
void heartbeat_task(void *pvParameters) {
    while (true) {
        printf("[Task 2 - System] Heartbeat... Core: %d\n", get_core_num());
        
        // Yield to let Task 1 run
        vTaskDelay(pdMS_TO_TICKS(200)); 
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("=== Starting Virtual Concurrency (Single Core) ===\n");

    // We create both tasks on Core 0. 
    // FreeRTOS will manage the switching (Time-slicing).
    xTaskCreate(fibonacci_task, "Fib", 1024, NULL, 1, NULL);
    xTaskCreate(heartbeat_task, "Heart", 1024, NULL, 1, NULL);

    vTaskStartScheduler();
    while (1);
}