NoController
============

It's not a controller.

# About
NoController is an extremely simple OpenFlow controller. So simple it may not even be an actual controller (hence the name).

It does not specify a single OpenFlow version, you can choose to use any version from 1.0 to 1.3. This, of course, greatly limits its API, forcing you to parse and build messages manually. It supports only one application running at a time.

The OpenFlow version to be used is defined by the application, not the controller. You can also choose some default behaviors for NoController.

It was created to test the auxiliary connections scheme in OpenFlow 1.3, but was extended to support other versions.

# Running
To compile the controller using an application, run:
```
$ make APP=appname
```

At the time, there are two hub applications: for OpenFlow 1.0 (`hub10`) and 1.2 (`hub12`).

After the compilation, run the controller:
```
$ ./nocontroller
```

By default, it listens at port 6633. To change this, modify the `nocontrolle.h` file.

# Writing applications

The `init` function is called by the controller when the application starts. It should configure the controller.

The `configure` function is called by the application to set the controller behavior.

An application must specify the following options:
```
VERSION: OpenFlow version
DEAL_WITH_HELLO: send and receive hello messages
DEAL_WITH_ECHO_REQUEST: respond to echo requests
DEAL_WITH_SWITCH_FEATURES: receive a switch features message and register the datapath ID
DEAL_WITH_SENDING_ECHO_REQUESTS: send an echo request every 5 seconds
```
Using the flags above, your application won't receive the respective messages being automatically treated (except for the switch features message, which will be treated and redirected).

The `process_message` function is called whenever a message goes through the controller to the application. The reply can be returned as one or more OpenFlow messages inside a single message, or NULL.

The `add_message_sender` starts a thread running a function you specify. This function can have any behavior, though it is intended to send one or more message asynchronously, possibly at regular intervals.

For examples, see the `apps` subdirectory and the `nocontroller.c` file.

# Shortcomings
This toy controller is obviously (and intentionally) very much limited.
There's a lot of room for improvement in the controller inner workings:

* It creates several threads like there's no tomorrow
* It deals very poorly with sockets
* It treats memory terribly
* It possibly violates good practices I may or may not be aware about
* There are some rough edges and lack of compliance with the protocol

Suggestions and code are welcome, and I'll try to improve the most pressing issues (especially the ones mentioned above).

