#include <stdio.h>

struct Process {
    int pid, at, bt, ct, tat, wt;
};

int main() {
    int n;
    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process p[n];
    for (int i = 0; i < n; i++) {
        printf("Enter AT & BT for P%d: ", i+1);
        scanf("%d%d", &p[i].at, &p[i].bt);
        p[i].pid = i+1;
    }

    // Sort by Arrival Time (for FCFS)
    for (int i = 0; i < n-1; i++) {
        for (int j = i+1; j < n; j++) {
            if (p[i].at > p[j].at) {
                struct Process temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }

    int time = 0;
    float total_tat = 0, total_wt = 0;

    for (int i = 0; i < n; i++) {
        if (time < p[i].at) time = p[i].at;  // CPU idle till arrival
        p[i].ct = time + p[i].bt;
        time = p[i].ct;

        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;

        total_tat += p[i].tat;
        total_wt += p[i].wt;
    }

    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\n");
    for (int i = 0; i < n; i++)
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid, p[i].at, p[i].bt,
               p[i].ct, p[i].tat, p[i].wt);

    printf("\nAverage TAT = %.2f", total_tat/n);
    printf("\nAverage WT = %.2f\n", total_wt/n);

    return 0;
}
