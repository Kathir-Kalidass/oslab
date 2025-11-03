#include <stdio.h>

struct Process {
    int pid, burst, waiting, turnaround, queue;
};

int main() {
    int n;
    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process p[n];
    printf("Enter burst time and queue number (0=System, 1=User) for each process:\n");

    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        scanf("%d %d", &p[i].burst, &p[i].queue);
    }

    // System queue (0) executes first, then User queue (1)
    int time = 0;
    printf("\nGantt Chart: ");
    for (int q = 0; q <= 1; q++) {
        for (int i = 0; i < n; i++) {
            if (p[i].queue == q) {
                p[i].waiting = time;
                time += p[i].burst;
                p[i].turnaround = time;
                printf("| P%d ", p[i].pid);
            }
        }
    }
    printf("|\n");

    printf("\nPID\tQueue\tBurst\tWaiting\tTurnaround\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n", p[i].pid, p[i].queue, p[i].burst, p[i].waiting, p[i].turnaround);
    }

    return 0;
}
