#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <filesystem>

//#include "Source/server/server.h"
//#include "Source/basic.h"
#include "Source/resources.h"
#include "Source/timing.h"
#include "Source/program.h"
//#include "Source/sighandle.h"

StopWatch runtime("Runtime", &std::cout, 0);

void callback(std::atomic_bool& external) {
    std::cin.ignore();
    external = false;
}

void on_exit() {
    runtime.end();
    std::cin.ignore();
    std::cin.ignore();
}

void donut(std::atomic_bool&);

int main(int argc, char* argv[], char* envp[]) {
    runtime.setStart();
    progdir.setDir(argv[0]);
    //sig_handle.setLog(&std::cout);

    //HttpServer server(&std::cout);
    //server.serve();

    atexit(on_exit);

    /*std::string input;
    std::cout << "Enter a directory path:\n";
    std::cin >> input;

    {
        ScopeTimer execution("Filesystem overhead");

        std::filesystem::path directory(input);

        for (std::filesystem::directory_entry const& dir_entry : std::filesystem::directory_iterator{ directory }) {
            if (dir_entry.path() == (directory / "obj")) {
                std::cout << "target directory found\n\n";
            }
            else {
                std::filesystem::path end = dir_entry.path().filename();
                std::cout << end << newline;
                std::cout << ("R:\\Rando\\folder" / end) << "\n\n";
            }
        }
    }*/

    std::atomic_bool running = { true };

    std::thread cb(callback, std::ref(running));
    donut(running);
    cb.join();

    return 0;
}



void donut(std::atomic_bool& external) {
    float A = 0, B = 0;
    float i, j;
    int k;
    float z[1760];
    char b[1760];
    printf("\x1b[2J");
    while(external) {
        memset(b, 32, 1760);
        memset(z, 0, 7040);
        for (j = 0; j < 6.28; j += 0.07) {
            for (i = 0; i < 6.28; i += 0.02) {
                float c = sin(i);
                float d = cos(j);
                float e = sin(A);
                float f = sin(j);
                float g = cos(A);
                float h = d + 2;
                float D = 1 / (c * h * e + f * g + 5);
                float l = cos(i);
                float m = cos(B);
                float n = sin(B);
                float t = c * h * g - f * e;
                int x = 40 + 30 * D * (l * h * m - t * n);
                int y = 12 + 15 * D * (l * h * n + t * m);
                int o = x + 80 * y;
                int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);
                if (22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                    z[o] = D;
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
            }
        }
        printf("\x1b[H");
        for (k = 0; k < 1761; k++) {
            putchar(k % 80 ? b[k] : 10);
            A += 0.00004;
            B += 0.00002;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        external = false;
    }
}