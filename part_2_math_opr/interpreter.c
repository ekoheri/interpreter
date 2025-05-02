// Compile :
// gcc interpreter.c -o interpreter ./libraryio.so
// Execute :
// ./interpreter halo.eko

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VARS 10
#define MAX_LEN  100

char variables[MAX_VARS][MAX_LEN];
char var_names[MAX_VARS][32];
int var_count = 0;

int int_vars[MAX_VARS];
char int_var_names[MAX_VARS][32];
int int_var_count = 0;

// deklarasi fungsi eksternal dari Assembly
extern void cetak(char *buffer);
extern void baca(char *buffer);
extern long panjang_string(char *buffer);
extern long string_to_int(char *buffer);
extern int tambah(int a, int b);

int* get_int_var(const char* name) {
    for (int i = 0; i < int_var_count; ++i)
        if (strcmp(int_var_names[i], name) == 0)
            return &int_vars[i];
    return NULL;
}

void set_int_var(const char* name) {
    strcpy(int_var_names[int_var_count], name);
    int_vars[int_var_count] = 0;
    int_var_count++;
}

char* get_var(const char* name) {
    for (int i = 0; i < var_count; ++i)
        if (strcmp(var_names[i], name) == 0)
            return variables[i];
    return NULL;
}

void set_var(const char* name) {
    strcpy(var_names[var_count], name);
    variables[var_count][0] = '\0';
    var_count++;
}

void cetak_str(const char* str) {
    char temp[MAX_LEN];
    int j = 0;
    for (int i = 0; str[i] != '\0' && j < MAX_LEN - 1; i++) {
        if (str[i] == '\\' && str[i + 1] == 'n') {
            temp[j++] = '\n';
            i++; // lewati 'n'
        } else {
            temp[j++] = str[i];
        }
    }
    temp[j] = '\0';
    cetak(temp);
}

void trim_trailing(char* str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\n')) {
        str[--len] = '\0';
    }
}

/*void run_line(char* line) {
    if (strncmp(line, "cetak(", 6) == 0) {
        char* start = strchr(line, '(');
        char* end = strrchr(line, ')');
        if (!start || !end) return;

        start++;
        *end = '\0';

        if (start[0] == '"') {
            start++;
            end = strchr(start, '"');
            if (end) *end = '\0';
            cetak_str(start);
        } else {
            char* val = get_var(start);
            if (val) {
                cetak(val);
            } else {
                int* int_val = get_int_var(start);
                if (int_val) {
                    char buf[32];
                    snprintf(buf, sizeof(buf), "%d", *int_val);
                    cetak(buf);
                }
            }
        }

    } else if (strncmp(line, "string ", 7) == 0) {
        char* name = strtok(line + 7, ";");
        if (name) set_var(name);
    } else if (strncmp(line, "integer ", 8) == 0) {
        char* name = strtok(line + 8, ";");
        if (name) {
            while (*name == ' ') name++;
            set_int_var(name);
        }
    } else if (strchr(line, '=') && strchr(line, '+')) {
        char* name = strtok(line, "=");
        char* expr = strtok(NULL, ";");

        if (name && expr) {
            while (*name == ' ') name++;
            while (*expr == ' ') expr++;
            
            trim_trailing(name);   // ← tambah ini!
            trim_trailing(expr);

            int a, b;
            sscanf(expr, "%d + %d", &a, &b);
            int* target = get_int_var(name);
            if (target) {
                *target = tambah(a, b);
            }
        }
    }
    else if (strncmp(line, "baca(", 5) == 0) {
        char* start = strchr(line, '(');
        char* end = strchr(line, ')');
        if (!start || !end) return;
        *end = '\0';
        start++;
        char* var = get_var(start);
        if (var) baca(var);

    } else if (strncmp(line, "panjang_string(", 15) == 0) {
        char* start = strchr(line, '(');
        char* end = strchr(line, ')');
        if (!start || !end) return;
        *end = '\0';
        start++;

        char* var = get_var(start);
        if (var) {
            long len = panjang_string(var);
            char buf[32];
            snprintf(buf, sizeof(buf), "%ld\n", len);
            cetak(buf);
        }
    } else if (strncmp(line, "string_to_int(", 14) == 0) {
        char* start = strchr(line, '(');
        char* end = strchr(line, ')');
        if (!start || !end) return;
        *end = '\0';
        start++;

        char* var = get_var(start);
        if (var) {
            long val = string_to_int(var);
            char buf[32];
            snprintf(buf, sizeof(buf), "%ld\n", val);
            cetak(buf);
        }
    }
}*/

void run_line(char* line) {
    if (strncmp(line, "cetak(", 6) == 0) {
        char* start = strchr(line, '(');
        char* end = strrchr(line, ')');
        if (!start || !end) return;

        start++;
        *end = '\0';

        if (start[0] == '"') {
            start++;
            end = strchr(start, '"');
            if (end) *end = '\0';
            cetak_str(start);
        } else {
            char* val = get_var(start);
            if (val) {
                cetak(val);
            } else {
                int* int_val = get_int_var(start);
                if (int_val) {
                    char buf[32];
                    snprintf(buf, sizeof(buf), "%d", *int_val);
                    cetak(buf);
                }
            }
        }

    } else if (strncmp(line, "string ", 7) == 0) {
        char* name = strtok(line + 7, ";");
        if (name) set_var(name);

    } else if (strncmp(line, "integer ", 8) == 0) {
        char* name = strtok(line + 8, ";");
        if (name) {
            while (*name == ' ') name++;
            set_int_var(name);
        }

    } else if (strchr(line, '=') && strchr(line, '+')) {
        // Assignment with addition
        char* name = strtok(line, "=");
        char* expr = strtok(NULL, ";");

        if (name && expr) {
            trim_trailing(name);
            trim_trailing(expr);
            while (*name == ' ') name++;
            while (*expr == ' ') expr++;

            // Pisahkan operand kiri dan kanan dari '+'
            char* left = strtok(expr, "+");
            char* right = strtok(NULL, "+");

            if (!left || !right) return;

            trim_trailing(left);
            trim_trailing(right);
            while (*left == ' ') left++;
            while (*right == ' ') right++;

            int val1 = 0, val2 = 0;

            // ----------------------------
            // Parsing operand kiri
            if (strncmp(left, "string_to_int(", 14) == 0) {
                char* inner = left + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val1 = string_to_int(str);
            } else if (isdigit(*left) || (*left == '-' && isdigit(left[1]))) {
                val1 = atoi(left);
            } else {
                int* temp = get_int_var(left);
                if (temp) val1 = *temp;
                else {
                    char* str = get_var(left);
                    if (str) val1 = string_to_int(str);
                }
            }

            // ----------------------------
            // Parsing operand kanan
            if (strncmp(right, "string_to_int(", 14) == 0) {
                char* inner = right + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val2 = string_to_int(str);
            } else if (isdigit(*right) || (*right == '-' && isdigit(right[1]))) {
                val2 = atoi(right);
            } else {
                int* temp = get_int_var(right);
                if (temp) val2 = *temp;
                else {
                    char* str = get_var(right);
                    if (str) val2 = string_to_int(str);
                }
            }

            int* target = get_int_var(name);
            if (target) {
                *target = tambah(val1, val2);
            }
        }

    } else if (strncmp(line, "baca(", 5) == 0) {
        char* start = strchr(line, '(');
        char* end = strchr(line, ')');
        if (!start || !end) return;
        *end = '\0';
        start++;
        char* var = get_var(start);
        if (var) baca(var);

    } else if (strncmp(line, "panjang_string(", 15) == 0) {
        char* start = strchr(line, '(');
        char* end = strchr(line, ')');
        if (!start || !end) return;
        *end = '\0';
        start++;

        char* var = get_var(start);
        if (var) {
            long len = panjang_string(var);
            char buf[32];
            snprintf(buf, sizeof(buf), "%ld\n", len);
            cetak(buf);
        }

    } else if (strncmp(line, "string_to_int(", 14) == 0) {
        char* start = strchr(line, '(');
        char* end = strchr(line, ')');
        if (!start || !end) return;
        *end = '\0';
        start++;

        char* var = get_var(start);
        if (var) {
            long val = string_to_int(var);
            char buf[32];
            snprintf(buf, sizeof(buf), "%ld\n", val);
            cetak(buf);
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Penggunaan: %s <nama_file_program>\n", argv[0]);
        return 1;
    }
    
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Gagal membuka program %s\n", argv[1]);
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        run_line(line);
    }

    fclose(f);
    return 0;
}
