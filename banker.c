/*
 * Banker's Algorithm
 * Name: Celena Valenzuela
 * 
 * Build: gcc -o banker banker.c
 * Run: ./banker A B C D [max_filename]
 * Example: ./banker 10 5 7 8 data.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

static int available[NUMBER_OF_RESOURCES];
static int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
static int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
static int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// Utilities
static void print_array_1d(const char *label, const int *a, int n) {
    printf("%s: [", label);
    for (int i = 0; i < n; ++i) {
        printf("%d%s", a[i], (i+1<n) ? ", " : "");
    }
    printf("]\n");
}

static void print_array_2d(const char *label, int rows, int cols, int m[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES]) {
    printf("%s:\n", label);
    for (int i = 0; i < rows; ++i) {
        printf("  C%d: [", i);
        for (int j = 0; j < cols; ++j) {
            printf("%d%s", m[i][j], (j+1<cols) ? ", " : "");
        }
        printf("]\n");
    }
}

static void print_state(void) {
    print_array_1d("Available", available, NUMBER_OF_RESOURCES);
    print_array_2d("Maximum", NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES, maximum);
    print_array_2d("Allocation", NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES, allocation);
    print_array_2d("Need", NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES, need);
}

// Initialization
static bool load_maximums(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) return false;

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
        for (int j = 0; j < NUMBER_OF_RESOURCES; ++j)
            if (fscanf(fp, "%d", &maximum[i][j]) != 1) {
                fclose(fp);
                return false;
            }

    fclose(fp);
    return true;
}

static void init_allocation_and_need(void) {
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j];
        }
    }
}

// Safe state check
static bool is_safe_state(int safe_seq_out[NUMBER_OF_CUSTOMERS]) {
    int work[NUMBER_OF_RESOURCES];
    bool finish[NUMBER_OF_CUSTOMERS] = {false};

    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) work[j] = available[j];

    int count = 0;
    while (count < NUMBER_OF_CUSTOMERS) {
        bool progress = false;
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
            if (finish[i]) continue;
            bool can_finish = true;
            for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
                if (need[i][j] > work[j]) { can_finish = false; break; }
            }
            if (can_finish) {
                for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
                    work[j] += allocation[i][j];
                }
                finish[i] = true;
                if (safe_seq_out) safe_seq_out[count] = i;
                count++;
                progress = true;
            }
        }
        if (!progress) break;
    }
    return (count == NUMBER_OF_CUSTOMERS);
}

// Request & Release
static bool validate_customer(int c) {
    return (0 <= c && c < NUMBER_OF_CUSTOMERS);
}

static bool validate_vector(const int v[NUMBER_OF_RESOURCES]) {
    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) if (v[j] < 0) return false;
    return true;
}

static bool handle_request(int c, const int req[NUMBER_OF_RESOURCES]) {
    if (!validate_customer(c) || !validate_vector(req)) {
        puts("Invalid RQ: bad customer id or negative request.");
        return false;
    }
    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
        if (req[j] > need[c][j]) {
            puts("Request denied. System would be left in an unsafe state.");
            return false;
        }
        if (req[j] > available[j]) {
            puts("Request denied. System would be left in an unsafe state.");
            return false;
        }
    }

    // Tentatively allocate
    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
        available[j] -= req[j];
        allocation[c][j] += req[j];
        need[c][j]      -= req[j];
    }

    int seq[NUMBER_OF_CUSTOMERS];
    if (is_safe_state(seq)) {
        puts("Request granted. System is in a safe state.");
        return true;
    }

    // Rollback
    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
        available[j] += req[j];
        allocation[c][j] -= req[j];
        need[c][j]      += req[j];
    }
    puts("Request denied. System would be left in an unsafe state.");
    return false;
}

static bool handle_release(int c, const int rel[NUMBER_OF_RESOURCES]) {
    if (!validate_customer(c) || !validate_vector(rel)) {
        puts("Invalid RL: bad customer id or negative release.");
        return false;
    }
    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
        if (rel[j] > allocation[c][j]) {
            puts("Request denied. System would be left in an unsafe state.");
            return false;
        }
    }
    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
        allocation[c][j] -= rel[j];
        need[c][j]       += rel[j];
        available[j]     += rel[j];
    }
    puts("Resources released successfully.");
    return true;
}

// Main function
int main(int argc, char *argv[]) {
    // Accept 4 resources
    if (argc != (1 + NUMBER_OF_RESOURCES) && argc != (2 + NUMBER_OF_RESOURCES)) {
        fprintf(stderr,
                "Usage: %s r1 r2 r3 r4 [max_filename]\n"
                "Example: %s 10 5 7 8 data.txt\n",
                argv[0], argv[0]);
        return 1;
    }

    // Max file path
    const char *max_path = (argc == (2 + NUMBER_OF_RESOURCES))
        ? argv[1 + NUMBER_OF_RESOURCES]
        : "data.txt";

    for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
        available[j] = atoi(argv[1 + j]);
        if (available[j] < 0) {
            fprintf(stderr, "Resource values must be non-negative.\n");
            return 1;
        }
    }

    printf("Welcome to the Banker's Algorithm Simulation\n");
    printf("Initializing system with resources:\n");
    print_array_1d("Available", available, NUMBER_OF_RESOURCES);

    printf("Reading maximum request file...\n");
    if (!load_maximums(max_path)) {
        fprintf(stderr, "Failed to read maximums from file \"%s\". Expected 5 lines of 4 integers.\n", max_path);
        return 1;
    }
    printf("Maximum requests initialized.\n");
    init_allocation_and_need();

    printf("Please enter commands:\n");
    printf("- 'RQ customer_id r1 r2 r3 r4' to request resources\n");
    printf("- 'RL customer_id r1 r2 r3 r4' to release resources\n");
    printf("- '*' to display the current system state\n");
    printf("- 'exit' to quit\n");

    char line[256];
    for (;;) {
        printf("\nCommand: ");
        if (!fgets(line, sizeof(line), stdin)) break;

        // strip trailing newlines
        size_t n = strlen(line);
        while (n && (line[n - 1] == '\n' || line[n - 1] == '\r')) line[--n] = '\0';

        // Skip leading spaces
        char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '\0') continue;

        // Single-word commands
        if (strcmp(p, "*") == 0) {
            print_state();
            continue;
        }
        if (strcmp(p, "exit") == 0) {
            break;
        }

        // parse RQ, RL
        char cmd[8];
        int c, r[NUMBER_OF_RESOURCES];
        int scanned = sscanf(p, "%7s %d %d %d %d %d", cmd, &c, &r[0], &r[1], &r[2], &r[3]);

        if (scanned == 6 && strcmp(cmd, "RQ") == 0) {
            handle_request(c, r);
            continue;
        }
        if (scanned == 6 && strcmp(cmd, "RL") == 0) {
            handle_release(c, r);
            continue;
        }

        printf("Invalid command or wrong number of arguments: \"%s\"\n", p);
    }
    return 0;
}
