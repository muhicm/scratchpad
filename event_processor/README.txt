TO BUILD:
1. Create "build" directory under "event_processor" directory.
2. From "build" directory run "cmake .."
3. From "build" directory run "make -j4"

TO EXECUTE:
From "build" directory run "./deploy/bin/event_processor 20 4"
Where the first arg 20 is number of producers and the second arg 4 is number of events per producer.