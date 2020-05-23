# Branches 
Branches should be named `b<nn>_<task_name>`, for example `b03_disk_offloading`

Task name should describe feature being implemented / task being solved
When you create new branch, add a line with the branch name, description, status and assignee to this file 


## List of branches 

Branch name | Description | Assignee | Status
--- | --- | --- | ---
B00_dummy_branch | This is where you should briefly describe the purpose of this branch. | Joe | MERGED /  IN PROGRESS / ABANDONED ...
B01_basic_query_parser | Initial implementation of parser parsing SQL requests | Jacob | MERGED
B02_connection_protocol_engine_and_sql_parser | Connection SQL_parser and protocol engine | Daniel, Jacob | MERGED 
B03_mq_communication_setup | IPC communication using message queues between protocol-engine and transaction-manager | Jacob | MERGED
B04_dummy_in-memory_db | Threaded transaction manager operating on in-memory db  | Jacob | IN PROGRESS
