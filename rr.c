#include <stdio.h>

struct Process {
    int pid, bt, rt, ct, tat, wt;
};

int main() {
    int n, tq, time = 0, completed = 0;
    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process p[n];
    for (int i = 0; i < n; i++) {
        printf("Enter Burst Time for P%d: ", i+1);
        scanf("%d", &p[i].bt);
        p[i].pid = i+1;
        p[i].rt = p[i].bt;   // remaining time
    }

    printf("Enter Time Quantum: ");
    scanf("%d", &tq);

    // keep looping until all processes are completed
    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (p[i].rt > 0) {
                if (p[i].rt > tq) {
                    time += tq;
                    p[i].rt -= tq;
                } else {
                    time += p[i].rt;
                    p[i].rt = 0;
                    p[i].ct = time;
                    completed++;
                }
            }
        }
    }

    float total_tat = 0, total_wt = 0;

    // calculate TAT & WT
    for (int i = 0; i < n; i++) {
        p[i].tat = p[i].ct;          // since AT=0 assumed
        p[i].wt  = p[i].tat - p[i].bt;

        total_tat += p[i].tat;
        total_wt  += p[i].wt;
    }

    printf("\nPID\tBT\tCT\tTAT\tWT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\n",
               p[i].pid, p[i].bt, p[i].ct,
               p[i].tat, p[i].wt);
    }

    printf("\nAverage TAT = %.2f", total_tat/n);
    printf("\nAverage WT = %.2f\n", total_wt/n);

    return 0;
}
