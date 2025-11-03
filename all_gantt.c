/*
 Combined CPU Scheduling Suite (menu-driven)
 Supports: FCFS, SJF (non-preemptive), SRTF (preemptive), RR (with AT), Priority (non-preemptive),
 Priority (preemptive).
 Contains:
  - validate_processes()
  - Gantt helper (add_gantt_entry, print_gantt)
  - scheduler functions that fill process CT/TAT/WT and populate Gantt.
*/

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#define MAXP 50
#define MAXG 1000

typedef struct {
    int pid;
    int at;   // arrival time
    int bt;   // burst time (original)
    int bt_left; // remaining burst for preemptive
    int pr;   // priority
    int ct;   // completion time
    int tat;  // turnaround time
    int wt;   // waiting time
    int done; // flag for completed
} Process;

typedef struct {
    int pid;   // 0 for idle
    int start; // inclusive
    int end;   // exclusive
} GanttEntry;

void init_processes(Process p[], int n) {
    for (int i=0;i<n;i++){
        p[i].bt_left = p[i].bt;
        p[i].ct = p[i].tat = p[i].wt = 0;
        p[i].done = 0;
    }
}

/* ---------- Validation ---------- */
/* Returns 0 if OK, otherwise non-zero and prints message */
int validate_processes(Process p[], int n, int check_quantum, int tq) {
    if (n <= 0) {
        printf("Validation error: number of processes must be > 0\n");
        return 1;
    }
    if (n > MAXP) {
        printf("Validation error: n exceeds MAXP (%d)\n", MAXP);
        return 2;
    }
    for (int i = 0; i < n; ++i) {
        if (p[i].at < 0) {
            printf("Validation error: Process P%d has negative arrival time (%d)\n", p[i].pid, p[i].at);
            return 3;
        }
        if (p[i].bt <= 0) {
            printf("Validation error: Process P%d has non-positive burst time (%d)\n", p[i].pid, p[i].bt);
            return 4;
        }
    }
    if (check_quantum && tq <= 0) {
        printf("Validation error: Time quantum must be > 0 (given %d)\n", tq);
        return 5;
    }
    return 0;
}

/* ---------- Gantt helpers ---------- */
void add_gantt_entry(GanttEntry g[], int *glen, int pid, int start, int end) {
    if (start >= end) return;
    if (*glen > 0) {
        GanttEntry *last = &g[*glen - 1];
        if (last->pid == pid && last->end == start) {
            last->end = end; // merge
            return;
        }
    }
    g[*glen].pid = pid;
    g[*glen].start = start;
    g[*glen].end = end;
    (*glen)++;
}

void print_gantt(GanttEntry g[], int glen) {
    if (glen == 0) {
        printf("Gantt: <no entries>\n");
        return;
    }
    printf("\n--- Gantt Chart ---\n");
    // boxes (compact)
    for (int i = 0; i < glen; ++i) {
        if (g[i].pid == 0) printf("| idle ");
        else printf("| P%-2d ", g[i].pid);
    }
    printf("|\n");
    // time ticks
    for (int i = 0; i < glen; ++i) {
        printf("%d\t", g[i].start);
    }
    printf("%d\n", g[glen-1].end);
}

/* ---------- Utility: print results ---------- */
void print_results(Process p[], int n) {
    float totalTAT=0, totalWT=0;
    printf("\nPID\tAT\tBT\tPR\tCT\tTAT\tWT\n");
    for (int i=0;i<n;i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n", p[i].pid, p[i].at, p[i].bt, p[i].pr, p[i].ct, p[i].tat, p[i].wt);
        totalTAT += p[i].tat;
        totalWT += p[i].wt;
    }
    printf("\nAverage TAT = %.2f\nAverage WT  = %.2f\n", totalTAT/n, totalWT/n);
}

/* ---------- FCFS (with arrival times) ---------- */
void schedule_FCFS(Process p[], int n, GanttEntry g[], int *glen) {
    // sort by arrival time (stable)
    for (int i=0;i<n-1;i++)
        for (int j=i+1;j<n;j++)
            if (p[i].at > p[j].at) {
                Process tmp = p[i]; p[i]=p[j]; p[j]=tmp;
            }

    init_processes(p,n);
    int time = 0;
    for (int i=0;i<n;i++) {
        if (time < p[i].at) {
            add_gantt_entry(g,glen,0,time,p[i].at); // idle
            time = p[i].at;
        }
        add_gantt_entry(g,glen,p[i].pid,time,time + p[i].bt);
        time += p[i].bt;
        p[i].ct = time;
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;
    }
}

/* ---------- SJF Non-preemptive ---------- */
void schedule_SJF_np(Process p[], int n, GanttEntry g[], int *glen) {
    init_processes(p,n);
    int completed = 0, time = 0;
    while (completed < n) {
        int idx = -1;
        int min_bt = INT_MAX;
        for (int i=0;i<n;i++) {
            if (!p[i].done && p[i].at <= time) {
                if (p[i].bt < min_bt) {
                    min_bt = p[i].bt;
                    idx = i;
                } else if (p[i].bt == min_bt) {
                    // tie-breaker: earlier arrival then lower pid
                    if (p[i].at < p[idx].at || (p[i].at == p[idx].at && p[i].pid < p[idx].pid))
                        idx = i;
                }
            }
        }
        if (idx == -1) {
            // no ready process
            int next_at = INT_MAX;
            for (int i=0;i<n;i++) if (!p[i].done && p[i].at < next_at) next_at = p[i].at;
            add_gantt_entry(g,glen,0,time,next_at);
            time = next_at;
            continue;
        }
        add_gantt_entry(g,glen,p[idx].pid,time,time + p[idx].bt);
        time += p[idx].bt;
        p[idx].done = 1;
        p[idx].ct = time;
        p[idx].tat = p[idx].ct - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        completed++;
    }
}

/* ---------- SRTF (preemptive SJF) ---------- */
void schedule_SRTF(Process p[], int n, GanttEntry g[], int *glen) {
    init_processes(p,n);
    int completed = 0, time = 0;
    while (completed < n) {
        int idx = -1;
        int min_rt = INT_MAX;
        for (int i=0;i<n;i++) {
            if (p[i].at <= time && p[i].bt_left > 0) {
                if (p[i].bt_left < min_rt) {
                    min_rt = p[i].bt_left;
                    idx = i;
                } else if (p[i].bt_left == min_rt) {
                    // tie-breaker: earlier arrival
                    if (p[i].at < p[idx].at) idx = i;
                }
            }
        }
        if (idx == -1) {
            // CPU idle for 1 unit or jump to next arrival
            int next_at = INT_MAX;
            for (int i=0;i<n;i++) if (p[i].bt_left>0 && p[i].at < next_at && p[i].at > time) next_at = p[i].at;
            if (next_at==INT_MAX) break;
            add_gantt_entry(g,glen,0,time,next_at);
            time = next_at;
            continue;
        }
        // run for 1 time unit (preemptive stepping)
        int start = time;
        time++;
        p[idx].bt_left--;
        add_gantt_entry(g,glen,p[idx].pid,start,time);
        if (p[idx].bt_left == 0) {
            p[idx].ct = time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
            completed++;
        }
    }
}

/* ---------- Round Robin (supports arrival times) ---------- */
void schedule_RR(Process p[], int n, int tq, GanttEntry g[], int *glen) {
    init_processes(p,n);
    int time = 0, completed = 0;
    int q[MAXP];
    int front = 0, rear = 0;
    int inqueue[MAXP]; for (int i=0;i<n;i++) inqueue[i]=0;

    // push processes that arrive at time 0
    for (int i=0;i<n;i++) if (p[i].at == 0) { q[rear++]=i; inqueue[i]=1; }
    if (rear==0) {
        // jump to first arrival
        int first_at = INT_MAX, first_idx=-1;
        for (int i=0;i<n;i++) if (p[i].at < first_at) { first_at=p[i].at; first_idx=i; }
        add_gantt_entry(g,glen,0,0,first_at);
        time = first_at;
        for (int i=0;i<n;i++) if (p[i].at==time) { q[rear++]=i; inqueue[i]=1; }
    }

    while (completed < n) {
        if (front == rear) {
            // queue empty, jump to next arrival
            int next_at = INT_MAX;
            for (int i=0;i<n;i++) if (p[i].bt_left>0 && p[i].at > time && p[i].at < next_at) next_at = p[i].at;
            if (next_at==INT_MAX) break;
            add_gantt_entry(g,glen,0,time,next_at);
            time = next_at;
            for (int i=0;i<n;i++) if (p[i].at==time && p[i].bt_left>0 && !inqueue[i]) { q[rear++]=i; inqueue[i]=1; }
            continue;
        }

        int idx = q[front++]; // dequeue
        if (p[idx].bt_left <= 0) continue; // already done (safety)
        int exec = (p[idx].bt_left > tq) ? tq : p[idx].bt_left;
        add_gantt_entry(g,glen,p[idx].pid,time,time+exec);
        time += exec;
        p[idx].bt_left -= exec;

        // enqueue newly arrived processes at this time
        for (int i=0;i<n;i++) if (p[i].at > (time-exec) && p[i].at <= time && p[i].bt_left>0 && !inqueue[i]) { q[rear++]=i; inqueue[i]=1; }

        if (p[idx].bt_left > 0) {
            // not finished, requeue
            q[rear++]=idx;
        } else {
            // finished
            p[idx].ct = time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
            completed++;
        }
    }
}

/* ---------- Priority Non-preemptive (lower number -> higher priority) ---------- */
void schedule_Priority_np(Process p[], int n, GanttEntry g[], int *glen) {
    init_processes(p,n);
    int completed = 0, time = 0;
    while (completed < n) {
        int idx = -1;
        int bestPr = INT_MAX;
        for (int i=0;i<n;i++) {
            if (!p[i].done && p[i].at <= time) {
                if (p[i].pr < bestPr) {
                    bestPr = p[i].pr;
                    idx = i;
                } else if (p[i].pr == bestPr) {
                    if (p[i].at < p[idx].at) idx = i;
                }
            }
        }
        if (idx == -1) {
            // idle until next arrival
            int next_at = INT_MAX;
            for (int i=0;i<n;i++) if (!p[i].done && p[i].at < next_at) next_at = p[i].at;
            add_gantt_entry(g,glen,0,time,next_at);
            time = next_at;
            continue;
        }
        add_gantt_entry(g,glen,p[idx].pid,time,time + p[idx].bt);
        time += p[idx].bt;
        p[idx].done = 1;
        p[idx].ct = time;
        p[idx].tat = p[idx].ct - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        completed++;
    }
}

/* ---------- Priority Preemptive (lower number -> higher priority) ---------- */
void schedule_Priority_p(Process p[], int n, GanttEntry g[], int *glen) {
    init_processes(p,n);
    int completed = 0, time = 0;
    while (completed < n) {
        int idx = -1;
        int bestPr = INT_MAX;
        for (int i=0;i<n;i++) {
            if (p[i].at <= time && p[i].bt_left > 0) {
                if (p[i].pr < bestPr) {
                    bestPr = p[i].pr;
                    idx = i;
                } else if (p[i].pr == bestPr) {
                    if (p[i].at < p[idx].at) idx = i;
                }
            }
        }
        if (idx == -1) {
            // jump to next arrival
            int next_at = INT_MAX;
            for (int i=0;i<n;i++) if (p[i].bt_left>0 && p[i].at > time && p[i].at < next_at) next_at = p[i].at;
            if (next_at == INT_MAX) break;
            add_gantt_entry(g,glen,0,time,next_at);
            time = next_at;
            continue;
        }
        int start = time;
        time++;
        p[idx].bt_left--;
        add_gantt_entry(g,glen,p[idx].pid,start,time);
        if (p[idx].bt_left == 0) {
            p[idx].ct = time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
            completed++;
        }
    }
}

/* ---------- main: menu & driver ---------- */
int main() {
    Process proc[MAXP];
    GanttEntry gantt[MAXG];
    int glen = 0;
    int n;

    printf("Enter number of processes: ");
    if (scanf("%d",&n) != 1) return 0;
    if (n <= 0 || n > MAXP) { printf("Invalid n\n"); return 0; }

    for (int i=0;i<n;i++) {
        proc[i].pid = i+1;
        printf("P%d - Arrival Time: ", i+1); scanf("%d",&proc[i].at);
        printf("P%d - Burst Time  : ", i+1); scanf("%d",&proc[i].bt);
        printf("P%d - Priority    : ", i+1); scanf("%d",&proc[i].pr);
    }

    printf("\nChoose algorithm:\n");
    printf("1. FCFS\n2. SJF (Non-preemptive)\n3. SRTF (Preemptive)\n4. Round Robin (with Time Quantum, supports AT)\n5. Priority (Non-preemptive)  (lower number = higher priority)\n6. Priority (Preemptive)      (lower number = higher priority)\n");
    int choice; printf("Enter choice: "); scanf("%d",&choice);

    int tq = 0;
    if (choice == 4) {
        printf("Enter Time Quantum: "); scanf("%d",&tq);
    }

    // validate
    int chk = validate_processes(proc, n, (choice==4), tq);
    if (chk != 0) return 0;

    // reset gantt
    glen = 0;

    // Because some schedulers sort the process array, create a copy to preserve original ordering for final printing
    Process copy[MAXP];
    for (int i=0;i<n;i++) copy[i]=proc[i];

    // call scheduler
    switch(choice) {
        case 1: schedule_FCFS(proc,n,gantt,&glen); break;
        case 2: schedule_SJF_np(proc,n,gantt,&glen); break;
        case 3: schedule_SRTF(proc,n,gantt,&glen); break;
        case 4: schedule_RR(proc,n,tq,gantt,&glen); break;
        case 5: schedule_Priority_np(proc,n,gantt,&glen); break;
        case 6: schedule_Priority_p(proc,n,gantt,&glen); break;
        default: printf("Invalid choice\n"); return 0;
    }

    // For final result printing, if any scheduler sorted the array or changed order, we want to show results by PID ascending.
    // Build a result array indexed by pid-1
    Process res[MAXP];
    for (int i=0;i<n;i++) {
        // initialize defaults if some fields not set
        res[i] = copy[i];
        res[i].ct = res[i].tat = res[i].wt = 0;
    }
    for (int i=0;i<n;i++) {
        int pid = proc[i].pid;
        // map by pid to res array
        res[pid-1].ct = proc[i].ct;
        res[pid-1].tat = proc[i].tat;
        res[pid-1].wt = proc[i].wt;
    }

    // But some functions (RR, SRTF, preemptive priority) might not fill entries for processes that finished; ensure mapping by searching if ct==0
    for (int i=0;i<n;i++) {
        if (res[i].ct == 0) {
            // search proc[]
            for (int j=0;j<n;j++) if (proc[j].pid == i+1) {
                res[i].ct = proc[j].ct;
                res[i].tat = proc[j].tat;
                res[i].wt = proc[j].wt;
                break;
            }
        }
    }

    // print results ordered by PID
    print_results(res,n);

    // print Gantt
    print_gantt(gantt,glen);

    return 0;
}
