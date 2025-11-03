#include <stdio.h>

struct Process {
    int pid, at, bt, pri, ct, tat, wt, done;
};

int main() {
    int n, time = 0, completed = 0;
    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process p[n];
    for (int i = 0; i < n; i++) {
        printf("Enter AT, BT & Priority for P%d: ", i+1);
        scanf("%d%d%d", &p[i].at, &p[i].bt, &p[i].pri);
        p[i].pid = i+1;
        p[i].done = 0;
    }

    float total_tat = 0, total_wt = 0;

    while (completed < n) {
        int idx = -1, max_pri = -9999;

        // pick the process with highest priority available
        for (int i = 0; i < n; i++) {
            if (!p[i].done && p[i].at <= time && p[i].pri > max_pri) {
                max_pri = p[i].pri;
                idx = i;
            }
        }

        if (idx == -1) {
            time++; // CPU idle
        } else {
            time += p[idx].bt;           // run till completion
            p[idx].ct = time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;

            p[idx].done = 1;
            completed++;

            total_tat += p[idx].tat;
            total_wt  += p[idx].wt;
        }
    }

    printf("\nPID\tAT\tBT\tPRI\tCT\tTAT\tWT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid, p[i].at, p[i].bt, p[i].pri,
               p[i].ct, p[i].tat, p[i].wt);
    }

    printf("\nAverage TAT = %.2f", total_tat/n);
    printf("\nAverage WT = %.2f\n", total_wt/n);

    return 0;
}
