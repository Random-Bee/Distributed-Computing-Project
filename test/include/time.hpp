#include <bits/stdc++.h>

#define LOGS_FILE "logs.txt"
#define DATA_FILE "data.txt"

using namespace std;

// Generate exponential distribution
size_t ran_expo(double lambda)
{
    double ran = rand() / (RAND_MAX + 1.0);
    double waittime = -log(1 - ran) / lambda;
    return (size_t)waittime;
}

// Returns time since epoch
size_t us()
{
    size_t t = std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::high_resolution_clock::now().time_since_epoch())
                   .count();
    return t;
}

// Returns current time in a string
string getSysTime()
{
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm *ltm = localtime(&now);
    size_t t = us();
    // Ensure that the time is in the format HH:MM:SS:MS:MIC
    string hr = to_string(ltm->tm_hour);
    if (hr.size() == 1)
        hr = "0" + hr;
    string min = to_string(ltm->tm_min);
    if (min.size() == 1)
        min = "0" + min;
    string sec = to_string(ltm->tm_sec);
    if (sec.size() == 1)
        sec = "0" + sec;
    string mil = to_string(t / 1000 % 1000);
    if (mil.size() == 1)
        mil = "00" + mil;
    else if (mil.size() == 2)
        mil = "0" + mil;
    string mic = to_string(t % 1000);
    if (mic.size() == 1)
        mic = "00" + mic;
    else if (mic.size() == 2)
        mic = "0" + mic;
    string s = hr + ":" + min + ":" + sec + ":" + mil + ":" + mic;
    return s;
}

