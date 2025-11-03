#include <stdio.h>

struct Process {
    int pid, burst, remaining, waiting, turnaround;
};

int main() {
    int n;
    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process p[n];
    printf("Enter burst times:\n");
    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        scanf("%d", &p[i].burst);
        p[i].remaining = p[i].burst;
    }

    int quantum[3] = {4, 8, 12};  // Increasing quantum times
    int time = 0;

    printf("\nGantt Chart: ");
    for (int q = 0; q < 3; q++) {
        for (int i = 0; i < n; i++) {
            if (p[i].remaining > 0) {
                int exec = (p[i].remaining > quantum[q]) ? quantum[q] : p[i].remaining;
                printf("| P%d ", p[i].pid);
                time += exec;
                p[i].remaining -= exec;

                if (p[i].remaining == 0) {
                    p[i].turnaround = time;
                    p[i].waiting = p[i].turnaround - p[i].burst;
                }
            }
        }
    }
    printf("|\n");

    printf("\nPID\tBurst\tWaiting\tTurnaround\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\n", p[i].pid, p[i].burst, p[i].waiting, p[i].turnaround);
    }

    return 0;
}
