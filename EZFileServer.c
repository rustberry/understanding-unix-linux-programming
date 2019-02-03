/* EZFileServer.c - a minimal file server which is good for learning
 * usage: ./EZFileServer [port number]
 *   features: supports the GET command only
 *             runs in the current directory
 *             forks a new child to handle each request
 *             has MAJOR security holes, for demo purposes only
 *             has many other weaknesses, but is a good start
 *      build: cc EZFileServer.c socklib.c -o EZFileServer
 */
#include 	<fcntl.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>
#include	<sys/types.h>
#include	<sys/wait.h>
#include 	<unistd.h>

#define DEBUG 1

// #define log(...) \
//         do { if (1) fprintf(stdout, "%d:%s(): " __LINE__, __func__, __VA_ARGS__); } while (0)

#ifdef DEBUG
#define log(fmt, ...) \
        do { if (1) fprintf(stdout, "%s:%d:%s(): \t\t" fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)
#endif

void process_rq(char *, int);


int main(int argc, char *argv[])
{
	int 	sock, fd;
	FILE	*fpin;
	char	request[BUFSIZ];
	int 	port = 8080;
	void 	waiter(int);
	void	read_til_crnl(FILE *);

	// Port defaults to 8080, if not given in cmd line arguments.
	if ( argc == 1 ) {
		sock = make_server_socket(port);
	} else {
		sock = make_server_socket( atoi(argv[1]) );
	}
	if ( sock == -1 ) exit(2);
	log("Start listening on port %d, sock id: %d. Ready.\n", port, sock);
	
	signal(SIGCHLD, waiter);  // Once a child exits, catch and release the resource

	// The main loop
	while(1){
		// Take a call and buffer it
		fd = accept( sock, NULL, NULL );
		fpin = fdopen(fd, "r" );

		// Read request
		fgets(request,BUFSIZ,fpin);
		printf("got a call: request = %s", request);
		read_til_crnl(fpin);  //Should I manually reach EOF?

		// Do what client asks
		process_rq(request, fd);
		
		fclose(fpin);
	}
}


void do_ls(char *, int);
void do_cat(char *, int);
void do_execvp(char *, int);
void err_404(char *, int);
void err_method_support(int);
int is_dir(char *);
int is_exist(char *);
int is_reg_file(char *);


void process_rq(char *request, int fd) {
    // A request looks like this:
    // GET /baz.txt HTTP/1.1
    char method[BUFSIZ], url[BUFSIZ];

    // Create a new process to handle; return if not the child
    if (fork() != 0) {
        return;
    }
    strcpy(url, ".");  // Under current dir
    if (sscanf(request, "%s%s", method, url + 1) != 2) {  // 'url' is leaded by '.', so + 1
		return;
	}
    log("method: (%s), url: (%s)\n", method, url);

    if (strcmp(method, "GET") != 0) {
        err_method_support(fd);
    } else if (!is_exist(url)) {
		log("Hit err_404 func, file name: %s\n", url);
        err_404(url, fd);
    } else if (is_dir(url)) {
        do_ls(url, fd);
    } else if (is_reg_file(url)){
        do_cat(url, fd);
    } else {
		do_execvp(url, fd);
	}
	log("End of process_rq. fd : %d\n", fd);
	exit(0);  // Otherwise child won't return.
}


void header(FILE *fp, char *stat_code, char *reason_phra, char *type) {
	// "HTTP/1.1 200 OK\r\n"

	char code[BUFSIZ], phra[BUFSIZ], t[BUFSIZ];
	if (!stat_code || !reason_phra) {
		strcpy(code, "200\0");
		strcpy(phra, "OK\0");
	} else {
		strcpy(code, stat_code);
		code[strlen(stat_code)] = '\0';
		strcpy(phra, reason_phra);
		phra[strlen(reason_phra)] = '\0';
	}
	if (!type) {
		strcpy(t, "text/plain\0");
	} else {
		strcpy(t, type);
		t[strlen(type)] = '\0';
	}
	fprintf(fp, "HTTP/1.1 %s %s\r\n", code, phra);
	fprintf(fp, "Content-type: %s\r\n", t);
	fprintf(fp,"\r\n");

/*
	if (!stat_code || !reason_phra) {
		fprintf(fp, "HTTP/1.1 200 OK\r\n");
	} else {
		fprintf(fp, "HTTP/1.1 %s %s\r\n", stat_code, reason_phra);
	}
	if (!type) {
		fprintf(fp, "Content-type: text/plain\r\n");
	} else {
		fprintf(fp, "Content-type: %s\r\n", type);
	}
	fprintf(fp,"\r\n");
*/
}

void do_ls(char *f_name, int fd) {
	FILE *fp;
	fp = fdopen(fd, "w");
	header(fp, NULL, NULL, NULL);

	dup2(fd, 1);
	dup2(fd, 2);
	fclose(fp);  // Closes fd as well

	execlp("ls", "ls", "-l", f_name, NULL);
}


char* file_type(char *f_name) {
	char *cp;
	if ((cp = strrchr(f_name, '.')) != NULL) {
		return cp + 1;
	}
	return "";
}


void do_cat(char *f_name, int fd) {
	FILE *fp;
	char content[BUFSIZ] = "text/plain";
	fp = fdopen(fd, "w");

	char *extension = file_type(f_name);
	if (strcmp(extension,"html") == 0)
		strcpy(content, "text/html");
	else if (strcmp(extension, "gif") == 0)
		strcpy(content, "image/gif");
	else if (strcmp(extension, "jpg") == 0)
		strcpy(content, "image/jpeg");
	else if (strcmp(extension, "jpeg") == 0)
		strcpy(content, "image/jpeg");
	header(fp, NULL, NULL, content);

	dup2(fd, 1);
	dup2(fd, 2);
	fclose(fp);

	execlp("cat", "cat", f_name, NULL);
}


void do_execvp(char *cmd, int fd) {
	FILE *fp;
	char **arglist;

	fp = fdopen(fd, "w");
	header(fp, NULL, NULL, NULL);

	dup2(fd, 1);
	dup2(fd, 2);
	fclose(fp);

	arglist = splitline(cmd);

	execvp(arglist[0], arglist);
}


int is_dir(char *f_name) {
	struct stat f;
	return (stat(f_name, &f) == 0 && S_ISDIR(f.st_mode));
}


int is_exist(char *f_name) {
	struct stat f;
	return (stat(f_name, &f) == 0);
}


int is_reg_file(char *f_name) {
	struct stat f;
	return (stat(f_name, &f) ==0 && S_ISREG(f.st_mode));
}


void err_404(char *f_name, int fd) {
	FILE *fp = fdopen(fd, "w");  // Note that w is double-quoted
	header(fp, "404", "Not Found", "text/html");
	// fflush(fp);
	fprintf(fp, "<h1>Not Found</h1>\r\n<p>The file you requested: <b>%s</b><br> \
				is not found</p>\r\n", f_name);
	fclose(fp);
}


void err_method_support(int fd) {
	FILE *fp = fdopen(fd, "w");
	header(fp, "501", "Not Supported", NULL);
	fclose(fp);
}

void waiter(int s) {
	int pid, status;
	pid = waitpid(-1, &status, WNOHANG);
	if (WIFEXITED(status)) {
		log("Received signal: (%d), child pid: (%d), exit status: (%u)\n",
			s, pid, WEXITSTATUS(status));
	}
}

void read_til_crnl(FILE *fp) {
	char buf[BUFSIZ];
	while (fgets(buf,BUFSIZ,fp) != NULL && strcmp(buf,"\r\n") != 0)
		;
}