// тестирование sizeof от указателя

#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include <cassert>

void func() {
    double *foo;
    foo = (double *)malloc(sizeof(foo));
}

void func(int) {
    double *foo;
    foo = (double *)malloc(sizeof(*foo));
}

enum auth_status {
    AUTH_FAIL,
    AUTH_SUCCESS
};

char *username = "admin";
char *pass = "password";

struct {
    double a;
    double b;
    double c;
} data[] = {{.a = 1, .b = 1, .c = 1}, {.a = 1, .b = 1, .c = 1}, {.a = 1, .b = 1, .c = 1}};

void *wrong_raw_data = malloc(sizeof(data) * 3);
void *correct_raw_data = malloc(sizeof(*data) * 3);

int AuthenticateUser(char *inUser, char *inPass) {
    printf("Sizeof username = %d\n", sizeof(username));
    printf("Sizeof pass = %d\n", sizeof(pass));

    if (strncmp(username, inUser, sizeof(username))) {
        printf("Auth failure of username using sizeof\n");
        return AUTH_FAIL;
    }
/* Because of CWE-467, the sizeof returns 4 on many platforms and architectures. */

    if (! strncmp(pass, inPass, sizeof(pass))) {
        printf("Auth success of password using sizeof\n");
        return AUTH_SUCCESS;
    }
    else {
        printf("Auth fail of password using sizeof\n");
        return AUTH_FAIL;
    }
}

int main(int argc, char **argv) {
    int authResult;

    if (argc < 3) {
        assert("Usage: Provide a username and password" && 0);
    }
    authResult = AuthenticateUser(argv[1], argv[2]);
    if (authResult != AUTH_SUCCESS) {
        assert("Authentication failed" && 0);
    }
}
