Two bank accounts synchronization

This project demonstrates how even complexed atomicity can be achieved using Linux's Kernel features. 

Problem of race condition bases on the fact that each modification variable in memory requires: 
copying it to processor's register,
changing it's value, 
copying back from CPU to RAM. 

Those three operations must be done separately. @semop@ gives such a possibility. 

<quote="man sysop">The set of operations contained in sops is performed in array order, and atomically, that is, the operations are  performed
       either  as  a complete unit, or not at all.</quote>

1. Open project SSO5.sublime-project in sublime using Project>Open project 
2. choose RUNSSO5-without-semaphores in Build>Build systems.
3. run by hitting ctrl+b
4. try to press few times enter and note that transactions are being simulated. 
5. Now enter q[enter]. In one window you will find report (see screenshot 215.Workspace 1.png). First one will proove that transactions did not provide comsistency of system. 

6. choose RunSSO5-with-semaphores in Build>Build systems
7. press enter few times - do some transactions,
8. press q and enter. You will see report (see screenshot 213.Workspace 1.png).
9. In this case all transactions sum to 0 (like in accounting! http://en.wikipedia.org/wiki/Debits_and_credits#Example)