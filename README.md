# Memory-Performance-Modeling
Memory model development, load &amp; store functionality, performance modeling


Here is the design considering 4 banks.There are three queues in the memory controller.
At a time one operation is posible in the bank, either read or write.
Queues in Memory Controller:
Request Queue: Contains all the request (read/write) to be processed.
               At a time maximum 4 unique request can be placed for the banks. 
Request Wait Queue:
               Is there is any bank conflict, then the request are placed into
               "request wait queue".

Response Queue:
              It holds all the response send for each request. All the response should be in order they executed in a specific bank.


Execution:
%> ./a.out <Memory in KB> <Bank Count> <Input Address> <TC NO Bank Conflict>
%> ./a.out 32 4 input_address.txt tc_no_bank_conflict.txt

Test Description:

Test Case1: tc_no_bank_conflict.txt 
Test Case2: tc1_bank_confilct.txt
Test Case3: tc2_bank_conflict.txt
Test Case4: tc4_bank_conflict.txt
          Write and Read from part of it from that memory.
