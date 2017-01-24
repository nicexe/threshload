#include <iostream>
#include <cstdlib>
#include <string.h>
#include <spawn.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

static void show_usage(string);
int spawn (char*, char**);

int main(int argc, char* argv[]) {

    if (argc < 3) {
        show_usage(argv[0]);
        return 1;
    }

    short int samples;
    double loads[3];

    samples = getloadavg(loads, 3);

    if (samples == -1) {
        cerr << "couldn't obtain load average\n";
        return 1;
    }

    int arg_sample;
    double arg_threshold;
    char* arg_program;
    // if one argument is parsed then program_arguments should be 0
    // if no arguments are parsed then program_arguments should be -1
    unsigned short program_arguments = argc - 5;

    arg_sample = stoi(argv[1]);
    arg_threshold = stod(argv[2]);
    arg_program = argv[3];

    char** arg_list = new char*[program_arguments+2];
    for (unsigned short i = 0; i<=program_arguments+1; ++i) {
        arg_list[i] = argv[i + 3];
    }
    arg_list[program_arguments+2]=NULL;


    switch (arg_sample) {
        case 0:
            for (unsigned short i = 0; i < 3; ++i) {
                if (loads[i]>=arg_threshold) {
                    spawn(arg_program, arg_list);
                    return 0;
                }
            }
            break;
        case 1:
            if (loads[0]>=arg_threshold) { spawn(arg_program, arg_list); return 0; } break;
        case 5:
            if (loads[1]>=arg_threshold) { spawn(arg_program, arg_list); return 0; } break;
        case 15:
            if (loads[2]>=arg_threshold) { spawn(arg_program, arg_list); return 0; } break;
        default:
            cerr << "invalid sample\n"; show_usage(argv[0]); return 1; break;
    }

    return 0;
}

static void show_usage(string name) {
    cerr << "Usage: " << name << " SAMPLE THRESHOLD PROGRAM [ARGUMENT [ARGUMENT...]]\n"
    << "\nSAMPLE:\t\tSpecifies which sample(s) to use\n"
    << "\t0\t\tCheck all samples\n"
    << "\t1\t\tCheck the 1 minute sample\n"
    << "\t5\t\tCheck the 5 minute sample\n"
    << "\t15\t\tCheck the 15 minute sample\n"
    << "\nTHRESHOLD:\tA real positive number\n"
    << "\nPROGRAM:\tThe program to be executed\n"
    << "\nARGUMENT:\tAn argument to be passed to the program\n";
}

int spawn (char* program, char** arg_list) {
    pid_t child_pid;
    // Duplicate this process.
    child_pid = fork ();
    if (child_pid != 0) {
        // This is the parent process.
        return child_pid; } else {
        // Now execute PROGRAM.
        execvp (program, arg_list);
        // The execvp function returns only if an error occurs.
        cerr << "an error occurred in execvp\n";
        abort (); }
}