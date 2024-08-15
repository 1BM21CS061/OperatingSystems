#ifndef BUILTINFUNCS_H_
#define BUILTINFUNCS_H_

extern char *builtin_str[];
extern int (*builtin_func[]) (char **);

int lsh_num_builtins();

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

#endif
