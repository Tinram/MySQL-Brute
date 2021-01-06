
# MySQL Brute

### MySQL brute-forcer.

##### MySQL Brute v.0.11


[1]: https://tinram.github.io/images/mysqlbrute.png
![mysqlbrute][1]


## Purpose

Brute force a MySQL user using a wordlist file.

MySQL Brute was created for MySQL localhost account recovery  
e.g.

+ root account inaccessible
+ mysqld not able to be restarted in safe mode for root reset
+ user password is lost.

PLESK-managed MySQL *admin* user is a candidate.


## OS

+ Linux


## Usage

```bash
    ./mysqlbrute --help

    ./mysqlbrute -h <host> -u <username> -f <wordlist_file>

    ./mysqlbrute -h localhost -u wordpress -f top_100000.txt
```

Download a password-only wordlist e.g. [Daniel Miessler's](https://github.com/danielmiessler/SecLists/tree/master/Passwords) (others can be username:password combinations).

Alternatively a simple wordlist for testing can be the Linux dictionary (Debian path):

```bash
    ./mysqlbrute -h localhost -u <username> -f /usr/share/dict/words
```

`MAX_WORD_LEN` of `50` in *mysqlbrute.c* is okay for most wordlists. However, some wordlists have borked entries (e.g. long email addresses included). For such wordlists, increase `MAX_WORD_LEN` to `140` (or more precisely, the output of `wc -L <wordlist_file>` + 1), and re-compile the source to avoid the resultant buffer overrun / segfault.

Other options:

```bash
    -p <port_number>
```

## Executables

+ x86 64-bit
+ ARM 32-bit

Download the executables from [Releases](https://github.com/Tinram/MySQL-Brute/releases/latest).


## Speed

MySQL Brute churns through approximately 20,000 passwords per second (vanilla Core i3 desktop CPU) on a Unix localhost socket connection &ndash; considerably faster than the Bash and Python scripts I tried before creating MySQL Brute (and curiously, faster than the vaunted multi-threaded *Hydra*). However, when using a network connection, MySQL Brute is much slower &ndash; around 1,000 per second on a local network.

MySQL Brute's speed bottlenecks are:

+ MySQL connect ( `mysql_real_connect()` ),
+ MySQL spawning only a limited number of threads for connections,
+ if not a localhost connection, the network connection (MySQL localhost connection uses a socket instead of TCP/IP).

If more speed is needed, there is 0x0mar's multi-threaded [Mysql-bruteforce](https://github.com/0x0mar/Mysql-bruteforce) or [my fork](https://github.com/Tinram/Mysql-bruteforce) of this.


### Hydra Comparison

```bash
    hydra -l wordpress -P top_100000.txt -t 4 -F localhost mysql
```

(As per the example in **Usage**, using 4 threads, ~1,050 tries per second on a Core i3.)

### Nmap Comparison

Nmap has a MySQL attack script which cycles through common usernames.

On the same Core i3:

*Statistics: Performed 50009 guesses in 9 seconds, average tps: 5556*


## Diagnosing Remote MySQL Connections

Unless you intimately know the MySQL set-up on a remote server, some of MySQL's configuration can silently (and righteously) impede MySQL Brute.

First attempt to connect to a remote MySQL connection from the terminal (use any random input when prompted for password):

```bash
    mysql -h <ip_addr> -u wordpress -p
```

*ERROR 1045 (28000): Access denied for user 'wordpress'@'host' (using password: YES)*

... shows MySQL is accepting remote user connections.

*ERROR 2003 (HY000): Can't connect to MySQL server on 'host' (111)*

... will be the bind address locked to localhost or a blocking firewall rule, or both.

### Checklist

+ `bind-address = 127.0.0.1` (*my.cnf*; if line present: comment out with `#`, then restart mysqld)
+ `skip-networking` (*my.cnf* &ndash; disables TCP/IP; if line present, comment out with `#`, then restart mysqld)
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
+ mysqld is down (on the server command-line, use: `pgrep mysql` &ndash; no number output means mysqld is not running).


## Build

### Linux


#### Make Installation

##### MySQL libraries (see further below for MariaDB).

```bash
    make deps && make && make install
```

(Assumes *libmysqlclient-dev* and *libssl-dev* libraries are not installed.)


#### Manual Installation

#### MySQL Libraries

Ensure the *libmysqlclient-dev* and *libssl-dev* dependencies (from distro repo) are installed:

```bash
    locate libmysqlclient-dev
    locate libssl-dev
```

If `locate` does not find each library, install on Debian-based distros with:

```bash
    make deps
```

or:

```bash
    sudo apt install libmysqlclient-dev libssl-dev
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

#### MariaDB Libraries

Delete *makefile* and rename *makefile_mariadb* to *makefile*.

```bash
    make deps && make && make install
```

or:

```bash
    sudo apt install libmariadb-dev libssl-dev
    make
```

([h0ek](https://github.com/h0ek) also specifies *libmariadb-dev-compat* as a dependency; in testing on Ubuntu 18.04 this library was not required for compilation, but it may well be in other scenarios.)

or:

**GCC:**

```bash
    gcc mysqlbrute.c $(mariadb_config --cflags) $(mariadb_config --libs) -o mariabrute -Ofast -Wall -Wextra -Wuninitialized -Wunused -Werror -std=gnu99 -s
```


## Other

### Warning

MySQL Brute will rapidly enlarge the MySQL error log files:

+ version 5.x &ndash; */var/log/mysql/error.log*
+ version 8 &ndash; */var/lib/mysql/&lt;pc_name&gt;.log* (with `general_log` variable enabled)

<small>(locations for Debian-based distros)</small>

### Location

It may be more convenient for MySQL Brute to be available from any directory location via the *$PATH* system variable (rather than copying the executable file to the directory where needed).

```bash
    make install
```

Or move the *mysqlbrute* executable to a location such as */usr/local/bin* (location must be present in *$PATH*).


## Credits

+ MMxM: slow Bash script that inspired.
+ Vivek Gite: MySQL libraries compiling.
+ Tim Čas: elegant EOL removal.
+ Ben Alpert: microsecond timer.


## License

MySQL Brute is released under the [GPL v.3](https://www.gnu.org/licenses/gpl-3.0.html).
