#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>



int main(){
    if(fork() && fork()){
        printf("hello1\n");
    }

    fork() ? printf("world2\n") : printf("hello2\n");
    exit(0);
}


// int main(void) {
//   if (fork() || fork()) {
//     printf("hello1\n");
//   } else {
//     printf("world1\n");
//   }

//   fork() ? printf("hello2\n") : printf("world2\n");
//   return 0;
// }



// int main() {
// 	if ( fork() && fork() ) 
// 		printf("hello1\n");
// 	else
// 		printf("world1\n");
// 	fork() ? printf("hello2\n") : printf("world2\n");
// 	return 0;
// }        

