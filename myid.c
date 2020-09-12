#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <ctype.h>

char str_is_num(const char *str)
{
	while (*str)
		if (!isdigit(*(str++)))
			return 0;
	return 1;
}

int main(int argc, char **argv)
{
	long ngroups_max = sysconf(_SC_NGROUPS_MAX) + 1;
	gid_t *groups = (gid_t *)calloc(ngroups_max, sizeof(gid_t));
	int ngroups;

	uid_t uid = 0;
	gid_t gid = 0;
	if (argc == 1)
	{
		uid = getuid();
		gid = getgid();
		int ngroups = getgroups(ngroups_max, groups);
	}
	else if (str_is_num(argv[1]))
	{
		struct passwd *pw = getpwuid(atoi(argv[1]));
		if (!pw)
		{
			fprintf(stderr, "no such uid\n");
			return 139;
		}
		uid = pw->pw_uid;
		gid = pw->pw_gid;
		getgrouplist(pw->pw_name, NULL, groups, &ngroups);
	}
	else
	{
		struct passwd *pw = getpwnam(argv[1]);
		if (!pw)
		{
			fprintf(stderr, "no such username\n");
			return 139;
		}
		uid = pw->pw_uid;
		gid = pw->pw_gid;
		getgrouplist(pw->pw_name, NULL, groups, &ngroups);
	}
	printf("uid=%d(%s) gid=%d(%s) группы=", uid, getpwuid(uid)->pw_name, gid, getgrgid(gid)->gr_name);
	
	if (groups)
		printf("%d(%s),", groups[0], getgrgid(groups[0])->gr_name);
	for (int i = 1; i < ngroups; i++)
		printf(" %d(%s),", groups[i], getgrgid(groups[i])->gr_name);
	printf("%d(%s)", groups[ngroups - 1], getgrgid(ngroups - 1)->gr_name);
	putchar('\n');
}
