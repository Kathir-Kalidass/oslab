#include <stdio.h>

struct Process {
    int pid;
    int burst_time;
    int priority;
    int waiting_time;
    int turnaround_time;
    int remaining_time;
};

int main() {
    int n, i, time = 0, completed = 0, highest;
    float avg_wt = 0, avg_tat = 0;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process p[n];

    for (i = 0; i < n; i++) {
        p[i].pid = i + 1;
        printf("Enter burst time and priority for process %d: ", p[i].pid);
        int bt, pr;
        if (scanf("%d %d", &bt, &pr) != 2) {
            fprintf(stderr, "Invalid input\n");
            return 1;
        }
        p[i].burst_time = bt;
        p[i].priority = pr;
        p[i].remaining_time = p[i].burst_time;
    }

    printf("\nProcess\tBurst Time\tPriority\tWaiting Time\tTurnaround Time\n");

    while (completed != n) {
        highest = -1;

        // Find process with highest priority among remaining
        for (i = 0; i < n; i++) {
            if (p[i].remaining_time > 0) {
                if (highest == -1 || p[i].priority < p[highest].priority) {
                    highest = i;
                }
            }
        }

        // Execute the selected process for 1 unit of time
        p[highest].remaining_time--;
        time++;

        // If process completed
        if (p[highest].remaining_time == 0) {
            completed++;
            p[highest].turnaround_time = time;
            p[highest].waiting_time = p[highest].turnaround_time - p[highest].burst_time;
            
            avg_wt += p[highest].waiting_time;
            avg_tat += p[highest].turnaround_time;

            printf("P%d\t%d\t\t%d\t\t%d\t\t%d\n", 
                   p[highest].pid, 
                   p[highest].burst_time, 
                   p[highest].priority, 
                   p[highest].waiting_time, 
                   p[highest].turnaround_time);
        }
    }

    printf("\nAverage Waiting Time = %.2f", avg_wt / n);
    printf("\nAverage Turnaround Time = %.2f\n", avg_tat / n);

    return 0;
}

