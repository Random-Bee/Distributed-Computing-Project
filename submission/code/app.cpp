#include <bits/stdc++.h>
#include <zmq.hpp>
#include <semaphore.h>
#include <sys/time.h>
using namespace std;

vector<string> facts = {
    "The first computer virus was created in 1983",
    "Mars appears red because it's rusty",
    "Elephants are the only animals that can't jump",
    "The shortest war in history lasted only 38 minutes",
    "The first alarm clock could only ring at 4 a.m.",
    "The first computer mouse was made of wood",
    "The first email was sent in 1971",
    "Iceland does not have a railway system",
    "The first camera took eight hours to snap a photo",
    "The first website is still online",
};

string base_endpoint;

// Socket for sending messages to other processes
zmq::socket_t sender;

// Socket for receiving messages from other processes
zmq::socket_t receiver;

// Context for zmq
zmq::context_t context;

void init()
{
    context = zmq::context_t(1);
    sender = zmq::socket_t(context, ZMQ_PUSH);
    receiver = zmq::socket_t(context, ZMQ_PULL);
    string sendAdr = base_endpoint;
    while(sendAdr.back() != ':') sendAdr.pop_back();
    sendAdr += "5555";
    sender.connect(sendAdr);
    receiver.bind(base_endpoint);
}

void doWork()
{
    while (1)
    {
        cout << "\nChoose function" << endl;
        cout << "1. Login" << endl;
        cout << "2. Sign Up" << endl;
        cout << "3. Exit" << endl;
        int choice;
        cin >> choice;

        if (choice == 1)
        {
            string key, value;
            cout << "Enter Username: ";
            cin >> key;
            cout << "Enter Password: ";
            cin >> value;

            string message = "retrieve " + base_endpoint + " " + key;
            zmq::message_t msg(message.size());
            memcpy(msg.data(), message.c_str(), message.size());
            sender.send(msg, zmq::send_flags::none);

            msg = zmq::message_t();
            zmq::recv_result_t res = receiver.recv(msg, zmq::recv_flags::none);
            message = string(static_cast<char *>(msg.data()), msg.size());
            stringstream ss(message);
            string type, sender;
            ss >> type >> sender;
            if (type == "fail")
            {
                cout << "\nUser not found" << endl;
            }
            else
            {
                string k1, v1;
                ss >> k1 >> v1;
                if (v1 == value)
                {
                    cout << "\nLogin successful" << endl;
                    cout << "Type exit to logout" << endl;
                    cout << "Press a number < 10 to get a fact" << endl;
                    string inp;
                    cin >> inp;
                    while(inp != "exit")
                    {
                        if(inp.length() == 1 && inp[0] >= '0' && inp[0] <= '9')
                        {
                            cout << facts[inp[0]-'0'] << endl;
                        }
                        else
                        {
                            cout << "Invalid choice" << endl;
                        }
                        cout << "\nType exit to logout" << endl;
                        cout << "Press a number < 10 to get a fact" << endl;
                        cin >> inp;
                    }
                    cout << "\nLogged out" << endl;
                }
                else
                {
                    cout << "\nIncorrect password" << endl;
                }
            }
        }
        else if (choice == 2)
        {
            string key, value;
            cout << "Enter Username: ";
            cin >> key;
            cout << "Enter Password: ";
            cin >> value;
            string message = "store " + base_endpoint + " " + key + " " + value;
            zmq::message_t msg(message.size());
            memcpy(msg.data(), message.c_str(), message.size());
            sender.send(msg, zmq::send_flags::none);

            msg = zmq::message_t();
            zmq::recv_result_t res = receiver.recv(msg, zmq::recv_flags::none);
            message = string(static_cast<char *>(msg.data()), msg.size());
            stringstream ss(message);
            string type, sender;
            ss >> type >> sender;
            if(type == "success") {
                cout << "\nUser created" << endl;
            }
            else {
                cout << "\nUser already exists" << endl;
            }
        }
        else if (choice == 3)
        {
            break;
        }
        else
        {
            cout << "\nInvalid choice" << endl;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: ./main <machine_address:port>" << endl;
        return 0;
    }
    string adr = argv[1];
    base_endpoint = "tcp://" + adr;
    init();
    doWork();
    sender.close();
    receiver.close();
    return 0;
}