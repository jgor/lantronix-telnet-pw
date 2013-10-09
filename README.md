<pre>
DESCRIPTION:
Lantronix serial-to-ethernet device configurations can be retrieved remotely,
divulging their 4-character telnet access password if one is set. This program
retrieves the device's setup record and extracts the telnet password.


COMPILE:
Run "make" within the lantronix-telnet-pw directory to compile the program.
$ make


USAGE:
lantronix-telnet-pw -[hv] [-p PORT] HOST

Options:
  -p, --port=PORT  configuration port (default: 30718)
  -h, --help       display this screen
  -v, --version    display version


AUTHOR:
John Gordon <jgor@indiecom.org>
</pre>
