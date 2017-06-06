
# MySQL Brute


### MySQL bruteforcer.

##### MySQL Brute v.0.10


## Purpose

Brute force mysqld using a wordlist file.

MySQL Brute was created for localhost account recovery: where the root account is inaccessible and an ordinary user password is lost.


## OS Support

+ Linux


## Usage

    ./mysqlbrute --help

    ./mysqlbrute -h <host> -u <username> -f <wordlist_file>

There are many wordlists available e.g. [Daniel Miessler's](https://github.com/danielmiessler).

Alternatively a quick wordlist for testing is the Linux dictionary (Debian path):

    ./mysqlbrute -h localhost -u <username> -f /usr/share/dict/words

Other options:

    -p <port_number>


## Speed

The speed bottlenecks are: the MySQL connect (mysql\_real\_connect()), MySQL spawning only a limited number of threads for connections, and - if not a localhost connection - the network connection. (MySQL localhost connection uses a socket instead of TCP/IP.).

MySQL Brute churns through approximately 10,000 to 20,000 passwords per second on a localhost socket connection - considerably faster than the Bash and Python scripts I tried before creating MySQL Brute. When using a network connection, it's much slower.

MySQL Brute can be quite easily converted to multi-threading with the OMP library. On an intermediate-sized wordlist, the OMP version was 3 seconds faster on the same machine.  However, with MySQL bottlenecks and some program instability on large wordlists, I have abandoned multi-threading.


## Build

### Linux-only

Ensure the *libmysqlclient-dev* library (from the repo) is installed:

    locate libmysqlclient-dev

In the directory containing either the clone or the extracted zip files, compile with GCC:

    make

or full process:

    make && make install

compile manually:

**GCC:**

    gcc mysqlbrute.c $(mysql_config --cflags) $(mysql_config --libs) -o mysqlbrute -Ofast -Wall -Wextra -Wuninitialized -Wunused -Werror -std=gnu99 -s

**Clang:**

    clang mysqlbrute.c $(mysql_config --cflags) $(mysql_config --libs) -o mysqlbrute -O3 -Wall -Wextra -Wuninitialized -Wunused -Werror -std=gnu99 -s


## Other

It's more convenient for MySQL Brute to be available from any directory location via the PATH system variable (rather than copying the executable file to the directory where needed).

    make install

Or move the *mysqlbrute* executable to a location such as */usr/local/bin* (location must be present in $PATH).


## Credits

+ MMxM: Bash script that inspired.
+ Vivek Gite: MySQL libraries compiling.
+ Tim Čas: elegant EOL removal.
+ Ben Alpert: microsecond timer.


## License

MySQL Brute is released under the [GPL v.3](https://www.gnu.org/licenses/gpl-3.0.html).
