# Chapter 2 Cmake List

add_exec(mspoller.c)
add_exec(msreader.c)
add_exec(rrbroker.c)
add_exec(rrclient.c)
add_exec(rrworker.c)
add_exec(msgqueue.c)
add_exec(tasksink2.c)
add_exec(taskwork2.c)
add_exec(interrupt.c)
add_exec(syncsub.c)
add_exec(syncpub.c)
add_exec(psenvsub.c)
add_exec(psenvpub.c)

add_pthread_exec(mtrelay.c)
add_pthread_exec(mtserver.c)
