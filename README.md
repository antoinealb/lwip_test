lwip_test
=========

My test app for the LWIP IP stack. It does the following :

* It initializes the stack.
* It creates a loopback interface and a Serial Line IP interface.
* It sends some TCP data to itself over the loopback.


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
