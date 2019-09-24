# Home task:
1. Create kernel module which start separate thread using kthread_create or kthread_run  (what is the main diffs?)
2. "Create kernel module which start N separate threads using kthread_create or kthread_run with logging once per second for each thread. Thread should be stoped from parent thread by kthread_stop and kthread_should_stop after T seconds"
3. Add critical section within thread loop using spinlock which is common for all threads in the system(developed module)
4. Replace spinlock with real time mutex

