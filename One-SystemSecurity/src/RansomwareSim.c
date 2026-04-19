// Minimal ransomware behavior simulator for ROFBSα testing

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define TEST_DIR "/mnt/testfs/victim"
#define MAX_FILES 200          // configurable
#define FILE_SIZE 4096         // 4KiB per file 
#define ITERATIONS 5           // re-open/modify each file

void create_random_filename(char* buf, size_t len) {
    snprintf(buf, len, "%s/file_%06d.dat", TEST_DIR, rand() % 100000);
}

int main() {
    srand(time(NULL));
    mkdir(TEST_DIR, 0755);  // ensure test dir exists

    printf("[SIM] Starting ransomware simulation — %d files, %d iterations each\n", MAX_FILES, ITERATIONS);
    printf("[SIM] Target dir: %s\n\n", TEST_DIR);

    for (int i = 0; i < MAX_FILES; i++) {
        char path[256];
        create_random_filename(path, sizeof(path));

        for (int iter = 0; iter < ITERATIONS; iter++) {
            // === critical: open, modify, close ===
            int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open failed");
                continue;
            }

            // simulates encryption
            char buffer[FILE_SIZE];
            for (size_t j = 0; j < FILE_SIZE; j++) {
                buffer[j] = (char)(rand() % 256);   // entropy spike
            }

            if (write(fd, buffer, FILE_SIZE) != FILE_SIZE) {
                perror("write failed");
            }

            close(fd);

            // delay to simulate real ransomware pacing
            usleep(5000);  // 5ms, tweak as needed
        }

        if ((i + 1) % 50 == 0) {
            printf("[SIM] Processed %d/%d files...\n", i + 1, MAX_FILES);
        }
    }

    printf("[SIM] Finished. All files modified %d times.\n", ITERATIONS);
    printf("[SIM] Check backup ratio and timing in your eBPF hooks now.\n");

    return 0;
}
