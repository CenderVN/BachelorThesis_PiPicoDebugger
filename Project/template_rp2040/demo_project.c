#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
int main() {
    // 1. Initialize Serial (UART/USB)
    stdio_init_all();


    // 3. Fibonacci Variables (using unsigned long long for large numbers)
    uint64_t t1 = 0;
    uint64_t t2 = 1;
    uint64_t nextTerm = 0;
    int count = 1;

    // Small delay to let you open the terminal window
    sleep_ms(2000); 
    printf("\n=== Starting Fibonacci Sequence ===\n");

    while (true) {
        // Print the current term
        // %llu is the format specifier for unsigned long long
        printf("Term %d: %llu\n", count, t1);

        // Calculate next term
        nextTerm = t1 + t2;
        t1 = t2;
        t2 = nextTerm;
        count++;


        // Reset logic: If numbers get too big (overflow), restart
        // (18,446,744,073,709,551,615 is the max for uint64)
        if (nextTerm < t1) { 
            printf("\n--- Max Value Reached! Resetting Sequence ---\n");
            t1 = 0;
            t2 = 1;
            count = 1;
            sleep_ms(2000);
        }
    }
}