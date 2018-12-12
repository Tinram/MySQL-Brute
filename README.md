
# MySQL Brute

### MySQL brute-forcer.

##### MySQL Brute v.0.10


[1]: https://tinram.github.io/images/mysqlbrute.png
![mysqlbrute][1]


## Purpose

Brute force a MySQL user using a wordlist file.

MySQL Brute was created for localhost account recovery  
e.g.

+ root account inaccessible
+ mysqld not to be restarted in safe mode for root reset
+ ordinary user password is lost.

PLESK-managed MySQL is a candidate.


## OS

+ Linux


## Usage

```bash
    ./mysqlbrute --help

    ./mysqlbrute -h <host> -u <username> -f <wordlist_file>

    ./mysqlbrute -h localhost -u wordpress -f top_100000.txt
```

There are many wordlists available e.g. [Daniel Miessler's](https://github.com/danielmiessler/SecLists/tree/master/Passwords).

Alternatively a simple list for testing is the Linux dictionary (Debian path):

```bash
    ./mysqlbrute -h localhost -u <username> -f /usr/share/dict/words
```

`MAX_WORD_LEN` of `50` in *mysqlbrute.c* is fine for most wordlists. However, some wordlists have borked entries (e.g. long email addresses included). For these wordlists, increase `MAX_WORD_LEN` to `140` (or, more precisely, output of `wc -L <wordlist_file>` + 1), and re-compile to avoid the resultant buffer overrun / segfault.

Other options:

    -p <port_number>


## Executables

+ x86 64-bit
+ ARM 32-bit

Download from [Releases](https://github.com/Tinram/MySQL-Brute/releases/latest).


## Speed

MySQL Brute churns through approximately 20,000 passwords per second (vanilla Core i3 desktop CPU) on a localhost socket connection &ndash; considerably faster than the Bash and Python scripts I tried before creating MySQL Brute (and curiously, faster than the vaunted multi-threaded *Hydra*). However, when using a network connection, MySQL Brute is much slower &ndash; around 1,000 per second on a local network.

MySQL Brute's speed bottlenecks are:

+ MySQL connect ( `mysql_real_connect()` ),
+ MySQL spawning only a limited number of threads for connections,
+ if not a localhost connection, the network connection (MySQL localhost connection uses a socket instead of TCP/IP).

If more speed is needed, there is 0x0mar's multi-threaded [Mysql-bruteforce](https://github.com/0x0mar/Mysql-bruteforce) or [my fork](https://github.com/Tinram/Mysql-bruteforce) of this.


### Hydra Comparison

    hydra -l wordpress -P top_100000.txt -t 4 -F localhost mysql

(As per the example in **Usage**, using 4 threads, ~1,050 tries per second on a Core i3.)

### Nmap Comparison

Nmap has a MySQL attack script which cycles through common usernames.

On same Core i3:

*Statistics: Performed 50009 guesses in 9 seconds, average tps: 5556*


## Diagnosing Remote MySQL Connections

Unless you intimately know the MySQL set-up on a remote server, some of MySQL's configuration can silently (and righteously) impede MySQL Brute.

First attempt to connect to a remote MySQL connection from the terminal (use any password when prompted):

```bash
    mysql -h <ip_addr> -u wordpress -p
```

*ERROR 1045 (28000): Access denied for user 'wordpress'@'host' (using password: YES)*

... shows MySQL is accepting remote user connections.

*ERROR 2003 (HY000): Can't connect to MySQL server on 'host' (111)*

... will be the bind address locked to localhost or a blocking firewall rule, or both.

### Checklist

+ `bind-address = 127.0.0.1` (*my.cnf*; if line present: comment out with `#`, then restart mysqld)
+ `skip-networking` (*my.cnf* - disables TCP/IP; if line present, comment out with `#`, then restart mysqld)
+ firewall rules
+ `mysql> SELECT host, user FROM mysql.user;`

---
    +-------------+------------+
    | host        | user       |
    +-------------+------------+
    | localhost   | wordpress  |
    | 10.0.0.%    | xyz        |
    +-------------+------------+
---
... no remote connection permitted for user *wordpress*, but local network access for user *xyz*.

+ mysqld can listen on a port other than 3306 (for port 3307, use `mysqlbrute ... -p 3307`)
+ mysqld is down (on the server command-line, use: `pgrep mysql`)


## Build

### Linux


#### Make Installation

```bash
    make deps && make && make install
```

(Assumes *libmysqlclient-dev* is not installed.)


#### Manual Installation

Ensure the *libmysqlclient-dev* library dependency (from distro repo) is installed:

```bash
    locate libmysqlclient-dev
```

If `locate` does not find the library, install on Debian-based distros with:

```bash
    make deps
```

or:

```bash
    sudo apt install libmysqlclient-dev
```

In the directory containing either the clone or the extracted zip files, compile with GCC:

```bash
    make
```

or:

**GCC:**

```bash
    gcc mysqlbrute.c $(mysql_config --cflags) $(mysql_config --libs) -o mysqlbrute -Ofast -Wall -Wextra -Wuninitialized -Wunused -Werror -std=gnu99 -s
```

**Clang:**

```bash
    clang mysqlbrute.c $(mysql_config --cflags) $(mysql_config --libs) -o mysqlbrute -O3 -Wall -Wextra -Wuninitialized -Wunused -Werror -std=gnu99 -s
```


## Other

### Warning

MySQL Brute will rapidly enlarge the MySQL error log file ( */var/log/mysql/error.log* for Debian-based distros).

### Location

It is more convenient for MySQL Brute to be available from any directory location via the *$PATH* system variable (rather than copying the executable file to the directory where needed).

```bash
    make install
```

Or move the *mysqlbrute* executable to a location such as */usr/local/bin* (location must be present in *$PATH*).


## Credits

+ MMxM: Bash script that inspired.
+ Vivek Gite: MySQL libraries compiling.
+ Tim Čas: elegant EOL removal.
+ Ben Alpert: microsecond timer.


## License

MySQL Brute is released under the [GPL v.3](https://www.gnu.org/licenses/gpl-3.0.html).
