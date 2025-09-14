#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PROC_NAME "./test.rb"

typedef struct Pipes {
  int out;
  int err;
} Pipes;

int do_select(Pipes pipes, fd_set *set, int max_fd);
Pipes run();

int main() {
  Pipes pipes = run();
  char buf[BUFSIZ] = {0};

  fd_set set;
  FD_ZERO(&set);
  FD_SET(pipes.out, &set);
  FD_SET(pipes.err, &set);
  int max_fd = ((pipes.out > pipes.err) ? pipes.out : pipes.err);

  printf("pipes = (%d, %d)\tmax_fd = %d\n", pipes.out, pipes.err, max_fd);

  while (1) {
    int fd = do_select(pipes, &set, max_fd);
    ssize_t bytes_read = read(fd, buf, BUFSIZ);
    if (bytes_read == 0) {
      FD_CLR(fd, &set);
    } else {
      buf[bytes_read] = '\0';
      printf("%s\n", buf);
    }

    //buf[0] = '\0';
  }
  return 0;
}

Pipes run() {
  int stdout_pipe[2];
  int stderr_pipe[2];
  if (pipe(stdout_pipe) == -1) {
    exit(__LINE__);
  }
  if (pipe(stderr_pipe) == -1) {
    exit(__LINE__);
  }
  int read_stdout = stdout_pipe[0];
  int write_stdout = stdout_pipe[1];
  int read_stderr = stderr_pipe[0];
  int write_stderr = stderr_pipe[1];

  int pid = fork();
  if (pid == 0) {
    // in child
    dup2(write_stdout, STDOUT_FILENO);
    dup2(write_stderr, STDERR_FILENO);
    // close(read_stdout);
    // close(read_stderr);
    execvp(PROC_NAME, (char *[]){PROC_NAME, NULL});
  } else if (pid == -1) {
    // error
    exit(__LINE__);
  }
  close(write_stdout);
  close(write_stderr);
  // Parent
  return (Pipes){
      .out = read_stdout,
      .err = read_stderr,
  };
}

static const int timeout_secs = 1;

int do_select(Pipes pipes, fd_set *set, int max_fd) {
  // Note, select will mutate this struct
  struct timeval tv = (struct timeval){
      .tv_sec = timeout_secs,
      .tv_usec = 0,
  };

  int err = select(max_fd + 1,
                   set,  // read
                   NULL, // write
                   NULL, // except
                   &tv);

  if (err == 0) {
    fprintf(stderr, "timeout after %d seconds\n", timeout_secs);
    exit(__LINE__);
  } else if (err == -1) {
    fprintf(stderr, "failure\n");
    exit(__LINE__);
  }
  // Success
  if (FD_ISSET(pipes.out, set)) {
    return pipes.out;
  } else if (FD_ISSET(pipes.err, set)) {
    return pipes.err;
  }
  fprintf(stderr, "Unreachable\n");
  exit(__LINE__);
}
