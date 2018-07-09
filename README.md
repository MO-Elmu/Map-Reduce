# Map-Reduce
This was the final big assignment in Stanford CS110. The objective is to harness the power of multiprocessing, networking, threads, concurrency, distributed computing, the Stanford "myth" computer cluster, and the shared AFS file system to build a fully operational MapReduce framework.
## Map-Reduce files
This is a complete list of all files contributing to code base.
#### mr.cc
mr.cc defines the main entry point for the MapReduce server. The entire file is very short, as all
it does is pass responsibility to a single instance of MapReduceServer
#### mapreduce-server.h/cc
These two files collectively define and implement the MapReduceServer class.
mapreduce-server.cc is by far the largest file , because the MapReduceServer
class is the central player in the whole MapReduce system. Each MapReduceServer
instance is responsible for:

● ingesting the contents of the command-line-provided configuration file and using the contents
of that file to self-configure

● establishing itself as a server, launching a server thread to aggressively poll for incoming requests
from mappers and reducers

● using the system function and the ssh command to launch remote workers (initially just one
on the main thread, and eventually many in separate threads)

● logging anything and everything that’s interesting about its conversations with workers, and

● detecting when all reducers have completed so it can shut itself down.
#### mrm.cc
mrm.cc defines the main entry point for the MapReduce mapper. mrm.cc is to the mapper
what mr.cc is to the server. It is very short, because the heart and lungs of a worker have
been implanted inside the MapReduceMapper class, which is defined and implemented in
mapreduce-mapper.h/cc. mrm.cc actually does little more than create
an instance of a MapReduceMapper, instructing it to coordinate with the server to do some work,
invoking mapper executables to process input files (the names of which are shared via messages
from the server), and then shutting itself down once the server says all input files have been processed.
#### mapreduce-mapper.h/cc
These two files collectively define and implement the MapReduceMapper class. The meat of its implementation
can be found in its map method, where the worker churns for as long as necessary
to converse with the server, accepting tasks, applying mapper executables to input files, reporting
back to the server when a job succeeds and when a job fails, and then shutting itself down—or
rather, exiting from its map method—when it hears from the server that all input files have been
processed.
#### mrr.cc
mrr.cc defines the main entry point for the MapReduce reducer.Just like mrm.cc file above for reducer.
#### mapreduce-reducer.h/cc
These two files collectively define and implement the MapReduceReducer class.
#### mapreduce-worker.h/cc
These two files define a parent class to unify state and logic common to both the
MapReduceMapper and MapReduceReducer classes.  Each of
MapReduceMapper and MapReduceReducer subclass MapReduceWorker.  
#### mr-nodes.h/cc
The mr-nodes module exports a single function that tells us what myth cluster machines are up
and running and able to contribute to our mr system.
#### mr-messages.h/cc
The mr-messages module defines the small set of messages that can be exchanged between workers
and servers.
#### mr-env.h/cc
The mr-env module defines a small collection of functions that helps surface shell variables, like
that for the logged-in user, the current host machine, and the current working directory.
#### mr-random.h/cc
Defines a pair of very short functions that you’ll likely ignore. The two exported functions—
sleepRandomAmount and randomChance—are used to fake the failure of the word-count-mapper
and word-count-reducer binaries. Real MapReduce jobs fail from time to time, so we should understand
how to build a system that’s sensitive—even if superficially so—to failure.
#### mr-names.h/cc
The mr-names module defines another small collection of helper functions that help use generate
the names of intermediate and final output files. 
#### mr-hash.h
This interface file is short but dense, and defines a class that can be used to generate a hash code
for an ifstream (or rather, the payload of the file it’s layered on top of ).
#### mr-utils.h/cc
The mr-utils module defines another small collection of helper functions that didn’t fit well in
mr-env, mr-random, or mr-names. Like functionality for parsing strings, managing string-to-number conversions, and ensuring that directories that need to exist actually exist.
#### server-socket.h/cc
Supplies the interface and implementation for the createServerSocket routine. The mr executable, which is responsible for spawning workers and exchanging messages
with them, acts as the one server that all workers contact. As a result, the mr executable—or more
specifically, the mapreduce-server module—must bind a server socket to a particular port on the
local host, and then share the server host and port with all spawned workers so they know how to
get back in touch. As a result, you’ll find a call to createServerSocket slap dab in the middle of
mapreduce-server.cc.
#### client-socket.h/cc
Supplies the interface and implementation for the createClientSocket routine we know must
contribute to the implementation of the MapReduceWorker class if the workers are to establish contact
with the server.
#### mapreduce-server-exception.h
Defines the one exception type—the MapReduceServerException—used to identify exceptional
circumstances (configuration issues, network hiccups, malformed messages between server and
worker, etc.) The implementation is so short and so obvious that it’s been inlined into a .h file.
#### thread-pool.h
Interface file represents a pool of threads since we are re-using threads after they finish so we rely on a ThreadPool.
#### word-count-mapper.cc, word-count-reducer.cc
These are standalone C++ executables that conform to the MapReduce programming model. Each
takes two arguments: an input file name and an output file name.
