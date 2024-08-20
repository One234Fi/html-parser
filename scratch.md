
# parallelism
"in parallel" - steps that are run one after another
"in parallel" != parallel
there are separate parallel queues that can be used for "in parallel" segments

## creation process for parallel queues
```
let parallelQueue = new ParallelQueue();
while (true)
    let steps = dequeue(parallelQueue.algorithmQueue)
    if steps != nil
        res = run(steps)
        assert(res != exception | error)
return parallelQueue
```


