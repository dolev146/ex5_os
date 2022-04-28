#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// gcc -o test main.c
// fcntl-india-example


int main (int argc, char* argv[])
{
char* file = argv[1]; // File is the input from the user. I.e. file name.
int fd;
//  For the file descriptor, Everything is tracked through file descriptor.

struct flock lock;

// An instance of flock, lock is created. This is mandatory.

printf ("I am opening the file %sln", file);

fd = open ("1.txt", O_WRONLY);

printf ("I am locking it buddy!In");
memset (&lock, 0, sizeof(lock)); // We have seen this earlier.
// Here, I initialize. Means, set this up for action.
lock.l_type = F_WRLCK;
//I There are locks available as write lock, read lock, refer man page please
// Here, we try a write lock. F_RDLCK has to be used for read lock.
fcntl (fd, F_SETLKW, &lock);
 // I have set the lock.
printf ("I Locked the file, Press Enter to unlock In ");
//Here, we got to wait for user stroke
getchar ();
// We got to release the lock.
printf ("Releasing Buddyln");
lock.l_type = F_UNLCK;
fcntl (fd, F_SETLKW, &lock);




}