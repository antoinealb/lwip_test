lwip_test
=========

[![Build Status](https://travis-ci.org/antoinealb/lwip_test.png)](http://travis-ci.org/antoinealb/lwip_test)

My test app for the LWIP IP stack. It does the following :

* It initializes the stack.
* It creates a loopback interface and a Serial Line IP interface.
* It sends some TCP data to itself over the loopback.

Build on Linux
==============
It is now possible to build the test application and run it on Linux for debug purposes.
Your kernel needs to be compiled with tun/tap interface support, which seems to be the case for at least Arch Linux.
You will also need CMake and GCC.

To do so, you will need to clone this repository along with cvra/lwip_ucos2. For example .

    git clone https://github.com/antoinealb/lwip_test.git
    git clone https://github.com/cvra/lwip_ucos2.git

Then, to build, it is standard cmake :

    cd lwip_test
    mkdir build && cd build
    cmake ..
    make

And finally to test :

    sudo gdb ./robot

You should now be able to ping 10.0.0.1. Wireshark can also be used to listen on interface tap0 to see what's going on.






Notes about multithreading programming
======================================
* Locks are a thing.
  You should **never** use a loop with a shared variable to achieve synchronization: if the waiting thread is of higher priority than the one being waited on, then you are locked.
  Semaphores, on the other hand, really suspend the higher priority thread, giving the other CPU time to complete its task. Simply remember to delete them once you have used them.
* Maintaining a priority list (see below) helps preventing locks and reasoning about them.
* Tasks never return. If they do, they should delete themselves before continuing.
* In case of weird variable modifications, stack overflow is a possibility. Use the mapfile to check for nearby stacks.

TODO List
=========
1. Try to output some of the serial signal to the outside world. [In progress]
2. Check if we can easily import SLIP/PPPoS data into Wireshark. Maybe custom python script is needed ? [In progress]
3. Write some docs about connecting the board to a PC.


Priority list
=============
Here is a list of all tasks that are running on this specific app.
Remember : The lower the priority number, the higher the priority (i.e., its importance).
**This list should always be maintained up-to-date.**
Really bad things can happen when you don't set your priority correctly.

| Task name | Priority | Notes
|-----------|----------|-------
| Init      | 22       | Is self-deleted once init is complete.
| TCP/IP    | 30       | The main lwIP thread
| SLIP      | 31       | The Serial Line IP Input thread. Continuously polls the serial line.
| Receiver  | 32       | The receiving thread for the ping application.
| Sender    | 33       | The sending thread for the ping app.
| SNTP      | 34       | The Network Time Protocol thread.
| Blink     | 35       | Blinks a led to detect hang.

# Enabling IP forwarding on my Linux box
    sudo sysctl -w net.ipv4.ip_forward=1
    sudo iptables --append FORWARD --in-interface tap0 -j ACCEPT
    sudo iptables --table nat --append POSTROUTING --out-interface wlan0 -j MASQUERADE

# UARTs
* COMBT2 is the first one from the right when looking into the connector
* COMBT1 is the second one from the right
* For some reason, it is not possible to open an uart port in "rw" mode.
* Having multiple screen instances opened is possible and will cause dropped bytes.

