// Compile :
// gcc interpreter.c -o interpreter ./libraryio.so
// Execute :
// ./interpreter tambah.eko

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

double double_vars[MAX_VARS];
char double_var_names[MAX_VARS][32];
int double_var_count = 0;

// deklarasi fungsi eksternal dari Assembly
extern void cetak(char *buffer);
extern void baca(char *buffer);
extern long panjang_string(char *buffer);
extern long string_to_int(char *buffer);
extern double string_to_double(char *buffer);
extern int tambah(int a, int b);
extern int kurang(int a, int b);
extern int kali(int a, int b);
extern int bagi(int a, int b);
extern int modulus(int a, int b);
extern double tambah_double(double a, double b);
extern double kurang_double(double a, double b);
extern double kali_double(double a, double b);
extern double bagi_double(double a, double b);


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

double* get_double_var(const char* name) {
    for (int i = 0; i < double_var_count; ++i)
        if (strcmp(double_var_names[i], name) == 0)
            return &double_vars[i];
    return NULL;
}

void set_double_var(const char* name) {
    strcpy(double_var_names[double_var_count], name);
    double_vars[double_var_count] = 0.0f;
    double_var_count++;
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
            i++;
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

void run_line(char* line) {
    if (strncmp(line, "cetak(", 6) == 0 ) {
        char* start = strchr(line, '(');
        char* end = strrchr(line, ')');
        if (!start || !end) return;
        start++; *end = '\0';

        while (*start == ' ') start++; // Trim leading spaces

        // Jika string literal
        if (*start == '"' && start[strlen(start) - 1] == '"') {
            start[strlen(start) - 1] = '\0'; // hapus tanda kutip akhir
            cetak_str(start + 1); // hapus tanda kutip awal dan cetak
            return;
        }

        // Jika bukan literal string, perlakukan sebagai nama variabel
        char* val = get_var(start);
        if (val) {
            cetak(val);
        } else {
            int* int_val = get_int_var(start);
            if (int_val) {
                char buf[32];
                snprintf(buf, sizeof(buf), "%d", *int_val);
                cetak(buf);
            } else {
                double* double_val = get_double_var(start);
                if (double_val) {
                    char buf[32];
                    snprintf(buf, sizeof(buf), "%.2f", *double_val);
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

    } else if (strncmp(line, "double ", 7) == 0) {
        char* name = strtok(line + 7, ";");
        if (name) {
            while (*name == ' ') name++;
            set_double_var(name);
        }

    } else if (strchr(line, '=') && strchr(line, '+')) {
        char* name = strtok(line, "=");
        char* expr = strtok(NULL, ";");

        if (name && expr) {
            trim_trailing(name);
            trim_trailing(expr);
            while (*name == ' ') name++;
            while (*expr == ' ') expr++;

            char* left = strtok(expr, "+");
            char* right = strtok(NULL, "+");

            if (!left || !right) return;

            trim_trailing(left);
            trim_trailing(right);
            while (*left == ' ') left++;
            while (*right == ' ') right++;

            int is_double = 0;
            double val1f = 0.0f, val2f = 0.0f;
            int val1i = 0, val2i = 0;

            if (strncmp(left, "string_to_double(", 17) == 0) {
                char* inner = left + 17;
                char* end = strchr(inner, ')');
                if (!end) return;         // tambahan perlindungan
                *end = '\0';
                trim_trailing(inner);     // <-- penting
                char* str = get_var(inner);
                if (str) {
                    trim_trailing(str);  // hapus spasi dan newline
                    val1f = string_to_double(str);
                    is_double = 1;
                }
            } else if (strncmp(left, "string_to_int(", 14) == 0) {
                char* inner = left + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val1i = string_to_int(str);
            } else if (strchr(left, '.')) {
                val1f = atof(left); is_double = 1;
            } else if (isdigit(*left) || (*left == '-' && isdigit(left[1]))) {
                val1i = atoi(left);
            } else {
                int* iv = get_int_var(left);
                if (iv) val1i = *iv;
                else {
                    double* fv = get_double_var(left);
                    if (fv) { val1f = *fv; is_double = 1; }
                    else {
                        char* str = get_var(left);
                        if (str) { val1f = string_to_double(str); is_double = 1; }
                    }
                }
            }

            if (strncmp(right, "string_to_double(", 17) == 0) {
                char* inner = right + 17;
                char* end = strchr(inner, ')');
                if (!end) return;         // tambahan perlindungan
                *end = '\0';
                trim_trailing(inner);     // <-- penting
                char* str = get_var(inner);
                if (str) {
                    trim_trailing(str);  // hapus spasi dan newline
                    val2f = string_to_double(str);
                    is_double = 1;
                }
            } else if (strncmp(right, "string_to_int(", 14) == 0) {
                char* inner = right + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val2i = string_to_int(str);
            } else if (strchr(right, '.')) {
                val2f = atof(right); is_double = 1;
            } else if (isdigit(*right) || (*right == '-' && isdigit(right[1]))) {
                val2i = atoi(right);
            } else {
                int* iv = get_int_var(right);
                if (iv) val2i = *iv;
                else {
                    double* fv = get_double_var(right);
                    if (fv) { val2f = *fv; is_double = 1; }
                    else {
                        char* str = get_var(right);
                        if (str) { val2f = string_to_double(str); is_double = 1; }
                    }
                }
            }

            if (is_double) {
                double* dest = get_double_var(name);
                if (dest) {
                    double lhs = (val1f != 0.0f || strchr(left, '.') || get_double_var(left)) ? val1f : (double)val1i;
                    double rhs = (val2f != 0.0f || strchr(right, '.') || get_double_var(right)) ? val2f : (double)val2i;
                    *dest = tambah_double(lhs, rhs);
                }
            } else {
                int* dest = get_int_var(name);
                if (dest) *dest = tambah(val1i, val2i);
            }
        }
    }  else if (strchr(line, '=') && strchr(line, '-')) {
        char* name = strtok(line, "=");
        char* expr = strtok(NULL, ";");

        if (name && expr) {
            trim_trailing(name);
            trim_trailing(expr);
            while (*name == ' ') name++;
            while (*expr == ' ') expr++;

            char* left = strtok(expr, "-");
            char* right = strtok(NULL, "-");

            if (!left || !right) return;

            trim_trailing(left);
            trim_trailing(right);
            while (*left == ' ') left++;
            while (*right == ' ') right++;

            int is_double = 0;
            double val1f = 0.0f, val2f = 0.0f;
            int val1i = 0, val2i = 0;

            if (strncmp(left, "string_to_double(", 17) == 0) {
                char* inner = left + 17;
                char* end = strchr(inner, ')');
                if (!end) return;         // tambahan perlindungan
                *end = '\0';
                trim_trailing(inner);     // <-- penting
                char* str = get_var(inner);
                if (str) {
                    trim_trailing(str);  // hapus spasi dan newline
                    val1f = string_to_double(str);
                    is_double = 1;
                }
            } else if (strncmp(left, "string_to_int(", 14) == 0) {
                char* inner = left + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val1i = string_to_int(str);
            } else if (strchr(left, '.')) {
                val1f = atof(left); is_double = 1;
            } else if (isdigit(*left) || (*left == '-' && isdigit(left[1]))) {
                val1i = atoi(left);
            } else {
                int* iv = get_int_var(left);
                if (iv) val1i = *iv;
                else {
                    double* fv = get_double_var(left);
                    if (fv) { val1f = *fv; is_double = 1; }
                    else {
                        char* str = get_var(left);
                        if (str) { val1f = string_to_double(str); is_double = 1; }
                    }
                }
            }

            if (strncmp(right, "string_to_double(", 17) == 0) {
                char* inner = right + 17;
                char* end = strchr(inner, ')');
                if (!end) return;         // tambahan perlindungan
                *end = '\0';
                trim_trailing(inner);     // <-- penting
                char* str = get_var(inner);
                if (str) {
                    trim_trailing(str);  // hapus spasi dan newline
                    val2f = string_to_double(str);
                    is_double = 1;
                }
            } else if (strncmp(right, "string_to_int(", 14) == 0) {
                char* inner = right + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val2i = string_to_int(str);
            } else if (strchr(right, '.')) {
                val2f = atof(right); is_double = 1;
            } else if (isdigit(*right) || (*right == '-' && isdigit(right[1]))) {
                val2i = atoi(right);
            } else {
                int* iv = get_int_var(right);
                if (iv) val2i = *iv;
                else {
                    double* fv = get_double_var(right);
                    if (fv) { val2f = *fv; is_double = 1; }
                    else {
                        char* str = get_var(right);
                        if (str) { val2f = string_to_double(str); is_double = 1; }
                    }
                }
            }

            if (is_double) {
                double* dest = get_double_var(name);
                if (dest) {
                    double lhs = (val1f != 0.0f || strchr(left, '.') || get_double_var(left)) ? val1f : (double)val1i;
                    double rhs = (val2f != 0.0f || strchr(right, '.') || get_double_var(right)) ? val2f : (double)val2i;
                    *dest = kurang_double(lhs, rhs);
                }
            } else {
                int* dest = get_int_var(name);
                if (dest) *dest = kurang(val1i, val2i);
            }
        }

    } else if (strchr(line, '=') && strchr(line, '*')) {
        char* name = strtok(line, "=");
        char* expr = strtok(NULL, ";");

        if (name && expr) {
            trim_trailing(name);
            trim_trailing(expr);
            while (*name == ' ') name++;
            while (*expr == ' ') expr++;

            char* left = strtok(expr, "*");
            char* right = strtok(NULL, "*");

            if (!left || !right) return;

            trim_trailing(left);
            trim_trailing(right);
            while (*left == ' ') left++;
            while (*right == ' ') right++;

            int is_double = 0;
            double val1f = 0.0f, val2f = 0.0f;
            int val1i = 0, val2i = 0;

            if (strncmp(left, "string_to_double(", 17) == 0) {
                char* inner = left + 17;
                char* end = strchr(inner, ')');
                if (!end) return;         // tambahan perlindungan
                *end = '\0';
                trim_trailing(inner);     // <-- penting
                char* str = get_var(inner);
                if (str) {
                    trim_trailing(str);  // hapus spasi dan newline
                    val1f = string_to_double(str);
                    is_double = 1;
                }
            } else if (strncmp(left, "string_to_int(", 14) == 0) {
                char* inner = left + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val1i = string_to_int(str);
            } else if (strchr(left, '.')) {
                val1f = atof(left); is_double = 1;
            } else if (isdigit(*left) || (*left == '-' && isdigit(left[1]))) {
                val1i = atoi(left);
            } else {
                int* iv = get_int_var(left);
                if (iv) val1i = *iv;
                else {
                    double* fv = get_double_var(left);
                    if (fv) { val1f = *fv; is_double = 1; }
                    else {
                        char* str = get_var(left);
                        if (str) { val1f = string_to_double(str); is_double = 1; }
                    }
                }
            }

            if (strncmp(right, "string_to_double(", 17) == 0) {
                char* inner = right + 17;
                char* end = strchr(inner, ')');
                if (!end) return;         // tambahan perlindungan
                *end = '\0';
                trim_trailing(inner);     // <-- penting
                char* str = get_var(inner);
                if (str) {
                    trim_trailing(str);  // hapus spasi dan newline
                    val2f = string_to_double(str);
                    is_double = 1;
                }
            } else if (strncmp(right, "string_to_int(", 14) == 0) {
                char* inner = right + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val2i = string_to_int(str);
            } else if (strchr(right, '.')) {
                val2f = atof(right); is_double = 1;
            } else if (isdigit(*right) || (*right == '-' && isdigit(right[1]))) {
                val2i = atoi(right);
            } else {
                int* iv = get_int_var(right);
                if (iv) val2i = *iv;
                else {
                    double* fv = get_double_var(right);
                    if (fv) { val2f = *fv; is_double = 1; }
                    else {
                        char* str = get_var(right);
                        if (str) { val2f = string_to_double(str); is_double = 1; }
                    }
                }
            }

            if (is_double) {
                double* dest = get_double_var(name);
                if (dest) {
                    double lhs = (val1f != 0.0f || strchr(left, '.') || get_double_var(left)) ? val1f : (double)val1i;
                    double rhs = (val2f != 0.0f || strchr(right, '.') || get_double_var(right)) ? val2f : (double)val2i;
                    *dest = kali_double(lhs, rhs);
                }
            } else {
                int* dest = get_int_var(name);
                if (dest) *dest = kali(val1i, val2i);
            }
        }
    } else if (strchr(line, '=') && strchr(line, '/')) {
        char* name = strtok(line, "=");
        char* expr = strtok(NULL, ";");

        if (name && expr) {
            trim_trailing(name);
            trim_trailing(expr);
            while (*name == ' ') name++;
            while (*expr == ' ') expr++;

            char* left = strtok(expr, "/");
            char* right = strtok(NULL, "/");

            if (!left || !right) return;

            trim_trailing(left);
            trim_trailing(right);
            while (*left == ' ') left++;
            while (*right == ' ') right++;

            int is_double = 0;
            double val1f = 0.0f, val2f = 0.0f;
            int val1i = 0, val2i = 0;

            if (strncmp(left, "string_to_double(", 17) == 0) {
                char* inner = left + 17;
                char* end = strchr(inner, ')');
                if (!end) return;         // tambahan perlindungan
                *end = '\0';
                trim_trailing(inner);     // <-- penting
                char* str = get_var(inner);
                if (str) {
                    trim_trailing(str);  // hapus spasi dan newline
                    val1f = string_to_double(str);
                    is_double = 1;
                }
            } else if (strncmp(left, "string_to_int(", 14) == 0) {
                char* inner = left + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val1i = string_to_int(str);
            } else if (strchr(left, '.')) {
                val1f = atof(left); is_double = 1;
            } else if (isdigit(*left) || (*left == '-' && isdigit(left[1]))) {
                val1i = atoi(left);
            } else {
                int* iv = get_int_var(left);
                if (iv) val1i = *iv;
                else {
                    double* fv = get_double_var(left);
                    if (fv) { val1f = *fv; is_double = 1; }
                    else {
                        char* str = get_var(left);
                        if (str) { val1f = string_to_double(str); is_double = 1; }
                    }
                }
            }

            if (strncmp(right, "string_to_double(", 17) == 0) {
                char* inner = right + 17;
                char* end = strchr(inner, ')');
                if (!end) return;         // tambahan perlindungan
                *end = '\0';
                trim_trailing(inner);     // <-- penting
                char* str = get_var(inner);
                if (str) {
                    trim_trailing(str);  // hapus spasi dan newline
                    val2f = string_to_double(str);
                    is_double = 1;
                }
            } else if (strncmp(right, "string_to_int(", 14) == 0) {
                char* inner = right + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val2i = string_to_int(str);
            } else if (strchr(right, '.')) {
                val2f = atof(right); is_double = 1;
            } else if (isdigit(*right) || (*right == '-' && isdigit(right[1]))) {
                val2i = atoi(right);
            } else {
                int* iv = get_int_var(right);
                if (iv) val2i = *iv;
                else {
                    double* fv = get_double_var(right);
                    if (fv) { val2f = *fv; is_double = 1; }
                    else {
                        char* str = get_var(right);
                        if (str) { val2f = string_to_double(str); is_double = 1; }
                    }
                }
            }

            if (is_double) {
                double* dest = get_double_var(name);
                if (dest) {
                    double lhs = (val1f != 0.0f || strchr(left, '.') || get_double_var(left)) ? val1f : (double)val1i;
                    double rhs = (val2f != 0.0f || strchr(right, '.') || get_double_var(right)) ? val2f : (double)val2i;
                    *dest = bagi_double(lhs, rhs);
                }
            } else {
                int* dest = get_int_var(name);
                if (dest) *dest = bagi(val1i, val2i);
            }
        }
    } else if (strchr(line, '=') && strchr(line, '%')) {
        char* name = strtok(line, "=");
        char* expr = strtok(NULL, ";");

        if (name && expr) {
            trim_trailing(name);
            trim_trailing(expr);
            while (*name == ' ') name++;
            while (*expr == ' ') expr++;

            char* left = strtok(expr, "%");
            char* right = strtok(NULL, "%");

            if (!left || !right) return;

            trim_trailing(left);
            trim_trailing(right);
            while (*left == ' ') left++;
            while (*right == ' ') right++;

            int val1i = 0, val2i = 0;

            if (strncmp(left, "string_to_int(", 14) == 0) {
                char* inner = left + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val1i = string_to_int(str);
            } else if (isdigit(*left) || (*left == '-' && isdigit(left[1]))) {
                val1i = atoi(left);
            } else {
                int* iv = get_int_var(left);
                if (iv) val2i = *iv;
                else {
                    char* str = get_var(left);
                    if (str) val1i = string_to_int(str);
                }
            }

            if (strncmp(right, "string_to_int(", 14) == 0) {
                char* inner = right + 14;
                char* end = strchr(inner, ')');
                if (end) *end = '\0';
                char* str = get_var(inner);
                if (str) val2i = string_to_int(str);
            } else if (isdigit(*right) || (*right == '-' && isdigit(right[1]))) {
                val2i = atoi(right);
            } else {
                int* iv = get_int_var(right);
                if (iv) val2i = *iv;
                else {
                    char* str = get_var(right);
                    if (str) val2i = string_to_int(str);
                }
            }

            int* dest = get_int_var(name);
            if (dest) {
                if (val2i == 0) {
                    printf("Error: Pembagian dengan nol tidak diperbolehkan (modulus).\n");
                    return;
                }
                *dest = modulus(val1i, val2i);
            }
        }

    } else if (strncmp(line, "baca(", 5) == 0) {
        char* start = strchr(line, '(');
        char* end = strchr(line, ')');
        if (!start || !end) return;
        *end = '\0'; start++;
        char* var = get_var(start);
        if (var) baca(var);
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