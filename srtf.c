#include <stdio.h>
#include <limits.h>

struct Process {
    int pid, at, bt, rt, ct, tat, wt;
};

int main() {
    int n, completed = 0, time = 0;
    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process p[n];
    for (int i = 0; i < n; i++) {
        printf("Enter AT & BT for P%d: ", i+1);
        scanf("%d%d", &p[i].at, &p[i].bt);
        p[i].pid = i+1;
        p[i].rt = p[i].bt;   // remaining time
    }

    float total_tat = 0, total_wt = 0;

    while (completed != n) {
        int idx = -1, min_rt = INT_MAX;

        // find process with shortest remaining time till now
        for (int i = 0; i < n; i++) {
            if (p[i].at <= time && p[i].rt > 0 && p[i].rt < min_rt) {
                min_rt = p[i].rt;
                idx = i;
            }
        }

        if (idx == -1) {
            time++; // CPU idle
            continue;
        }

        // execute for 1 unit of time
        p[idx].rt--;
        time++;

        // if process finished
        if (p[idx].rt == 0) {
            completed++;
            p[idx].ct = time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;

            total_tat += p[idx].tat;
            total_wt += p[idx].wt;
        }
    }

    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid, p[i].at, p[i].bt,
               p[i].ct, p[i].tat, p[i].wt);
    }

    printf("\nAverage TAT = %.2f", total_tat/n);
    printf("\nAverage WT = %.2f\n", total_wt/n);

    return 0;
}
