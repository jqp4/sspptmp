#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

enum { abs_max_gen_val = 10 };

struct input_data
{
    int help_passed;
    int gen_matrix;
    int gen_size;
    char *ifn_binv;
    char *ofn_binv;
};

static void
print_help(const char *arg0)
{
    fprintf(stderr, "usage: %s <options>\n\n"
            "Options:\n\n"
            "    r <file>   read and print binary matrix\n\n"
            "    w <file>   write matrix of size nxn\n"
            "               to the file, the matrix is read from stdin\n"
            "               in text format: <n> <el1> <el2>...\n\n"
            "    g <n>      generate random matrix and write it to the\n"
            "               file, passed with 'w' option\n"
            "               in this case stdin is ignored\n\n"
            "    -h --help  print this help message and exit\n\n",
            arg0);
}

static int
parse_arg(int argc, char **argv, int *idx, struct input_data *inp_data)
{
    if (0 == strcmp("-h", argv[*idx]) || 0 == strcmp("--help", argv[*idx])) {
        inp_data->help_passed = 1;
        return 0;
    } else if (0 == strcmp("r", argv[*idx])) {
        if (*idx + 1 >= argc) {
            fputs("missing filename for option 'r'\n", stderr);
            return 1;
        }
        inp_data->ifn_binv = argv[*idx + 1];
        ++*idx;
    } else if (0 == strcmp("w", argv[*idx])) {
        if (*idx + 1 >= argc) {
            fputs("missing filename for option 'w'\n", stderr);
            return 1;
        }
        inp_data->ofn_binv = argv[*idx + 1];
        ++*idx;
    } else if (0 == strcmp("g", argv[*idx])) {
        if (*idx + 1 >= argc) {
            fputs("missing value for option '-g'\n", stderr);
            return 1;
        }
        inp_data->gen_matrix = 1;
        if (sscanf(argv[*idx + 1], "%d", &(inp_data->gen_size)) != 1) {
            fputs("bad value for option '-g'\n", stderr);
            return 1;
        }
        ++*idx;
    } else {
        fprintf(stderr, "unknown option '%s'\n", argv[*idx]);
        return 1;
    }
    return 0;
}

static int
check_args(struct input_data *inp_data)
{
    if (inp_data->help_passed) {
        return 0;
    }
    if (inp_data->gen_matrix) {
        if (inp_data->ifn_binv) {
            fputs("option 'g' conficts with option 'r'\n", stderr);
            return 1;
        }
        if (!inp_data->ofn_binv) {
            fputs("option 'g' requires output filename (option 'w')\n",
                  stderr);
            return 1;
        }
        return 0;
    }
    if (inp_data->ifn_binv) {
        if (inp_data->ofn_binv) {
            fputs("option 'w' conflicts with option 'r'\n", stderr);
            return 1;
        }
        return 0;
    }
    if (!inp_data->ofn_binv) {
        fputs("error: no options given\n", stderr);
        return 1;
    }
    return 0;
}

static int
read_args(int argc, char **argv, struct input_data *inp_data)
{
    int i;

    inp_data->help_passed = 0;
    inp_data->gen_matrix = 0;
    inp_data->gen_size = 0ul;
    inp_data->ifn_binv = NULL;
    inp_data->ofn_binv = NULL;

    for (i = 1; i < argc; ++i) {
        if (parse_arg(argc, argv, &i, inp_data)) {
            fputs("see '--help'\n", stderr);
            return 1;
        }
    }
    if (check_args(inp_data)) {
        fputs("see '--help'\n", stderr);
        return 1;
    }
    return 0;
}

static int
print_matrix_from_file(const char *filename)
{
    int msize, i, j;
    FILE *f = NULL;

    f = fopen(filename, "rb");
    
    assert(f != NULL);
    assert(fread(&msize, sizeof(msize), 1, f) == 1);

    printf("%d\n", msize);

    for (i = 0; i < msize; ++i) {
        int val;
        assert(fread(&val, sizeof(val), 1, f) == 1);
        printf("%d", val);
        for (j = 1; j < msize; ++j) {
            assert(fread(&val, sizeof(val), 1, f) == 1);
            printf(" %d", val);
        }
        putchar('\n');
    }
    fclose(f);
    return 0;
}

static int
write_matrix(const char *filename, int do_gen_matrix, int gen_size)
{
    int msize, i;
    FILE *f = NULL;

    f = fopen(filename, "wb");
    assert(f != NULL);

    if (!do_gen_matrix) {
        assert(scanf("%d", &msize) == 1);
    } else {
        msize = gen_size;
        srand(time(NULL));
    }
    assert(fwrite(&msize, sizeof(msize), 1, f) == 1);

    for (i = 0; i < msize * msize; ++i) {
        int val;
        if (do_gen_matrix) {
            val = rand() % (2 * abs_max_gen_val + 1) - abs_max_gen_val;
        } else {
            assert(scanf("%d", &val) == 1);
        }
        assert(fwrite(&val, sizeof(val), 1, f) == 1);
    }

    assert(fclose(f) == 0);
    return 0;
}

int
main(int argc, char **argv)
{
    struct input_data inp_data;

    if (read_args(argc, argv, &inp_data)) {
        return 1;
    }
    if (inp_data.help_passed) {
        print_help(argv[0]);
        return 0;
    }

    if (inp_data.ifn_binv) {
        return print_matrix_from_file(inp_data.ifn_binv);
    }

    return write_matrix(inp_data.ofn_binv, inp_data.gen_matrix,
                        inp_data.gen_size);
}
