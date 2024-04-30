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

long long diffTime(string s1, string s2){
    int hr1 = stoi(s1.substr(0, 2));
    int min1 = stoi(s1.substr(3, 2));
    int sec1 = stoi(s1.substr(6, 2));
    int mil1 = stoi(s1.substr(9, 3));
    int mic1 = stoi(s1.substr(13, 3));
    int hr2 = stoi(s2.substr(0, 2));
    int min2 = stoi(s2.substr(3, 2));
    int sec2 = stoi(s2.substr(6, 2));
    int mil2 = stoi(s2.substr(9, 3));
    int mic2 = stoi(s2.substr(13, 3));
    cout << s1 << " " << s2 << endl;
    long long diff = (hr2 - hr1) * 3600000000 + (min2 - min1) * 60000000 + (sec2 - sec1) * 1000000 + (mil2 - mil1) * 1000 + (mic2 - mic1);
    return diff;
}