# starve-free-reader-writer
This repository aims to provide a detailed solution for starve free readers-writers problem.
# Problem Desciption
This is a classical problem of process synchronization and mutual exclusion where a shared file is being accessed by multiple processes (amongst which some want to read it called as readers and others want to write to it called as writers) concurrently.
# Starve Free Solution
All readers and writers will be granted access to the resource in their order of arrival (FIFO order). If a writer arrives while readers are accessing the resource, it will wait until those readers free the resource, and then modify it as soon as the resource is released by the reader. New readers arriving in the meantime will have to wait.
