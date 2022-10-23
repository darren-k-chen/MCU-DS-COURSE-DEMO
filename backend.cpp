// Author: Darren K.J. Chen | STU ID: 07460045
// MCU Data Structure Course, Homework 1, 111-1

#include <csignal>
#include <cstdlib>
#include <ctime>
#include <bits/stdc++.h>

// [Sys Env.] Ubuntu 20.04 LTS
// [Required] https://github.com/wfrest/wfrest
#include "workflow/WFFacilities.h"
#include "wfrest/HttpServer.h"
#include "wfrest/json.hpp"

#define SWAP(x,y,t) (t=x, x=y, y=t);

using namespace std;
using namespace wfrest;
using Json = nlohmann::json;

clock_t Begin, End;

string perResult = "";
string hanoiResult = "";
string magicResult = "";

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
    wait_group.done();
}

void permute(string a, int l, int r) 
{
    int tmp;
    if (l == r) perResult += a + "\n";
    else for (int i = l; i <= r; i++) 
    {
        SWAP(a[l], a[i], tmp);
        permute(a, l+1, r);
        SWAP(a[l], a[i], tmp);
    }
}

void hanoi(int n, string from, string to, string aux)
{
    if (n == 1)
    {
        hanoiResult += ( "Move disk 1 from rod " + from + " to rod " + to + "\n" );
        return;
    }
    hanoi(n - 1, from, aux, to);
    hanoiResult += ( "Move disk " + to_string(n) + " from rod " + from + " to rod " + to + "\n" );
    hanoi(n - 1, aux, to, from);
}

Json selectionSort(Json data, int n)
{
    int min_idx, tmp;
    for (int step = 0; step < n - 1; step++)
    {
        min_idx = step;
        for (int i = step + 1; i < n; i++)
            if (data[i] < data[min_idx]) min_idx = i;
        SWAP(data[min_idx], data[step], tmp);
    }
    return data;
}

Json bubbleSort(Json data, int n) 
{
    for (int step = 0; step < n; ++step) 
        for (int i = 0; i < n - step; ++i)
            if (data[i] > data[i + 1]) 
            {
                int tmp = data[i];
                data[i] = data[i + 1];
                data[i + 1] = tmp;
            }
    return data;
}

int main()
{
    signal(SIGINT, sig_handler);

    HttpServer svr;

    svr.Static("/static", "./frontend");

    // CMD: curl -v http://<ip>:<port>/test
    svr.GET("/test", [](const HttpReq *req, HttpResp *resp)
    {
        resp->String("API Service is work!!!\nPowered by Darren K.J. Chen | STU ID: 07460045");
    });

    // HW1 - Input: 'n=<Int: n>&range=<Int: range>'
    // Output: {"randomNumber": {"time": <double>, "data": <Array>}, "selectionSort": {"time": <double>, "data": <Array>}, "bubbleSort": {"time": <double>, "data": <Array>}}
    // MEMO: NEED RETURN [randaom number * n] & [Sort the random number via Selection Sort] & [Sort the random number via Bubble Sort]
    // CMD: curl -X POST http://<ip>:<port>/hw1 -H "body-type:application/x-www-form-urlencoded" \ -d 'n=<Int: n>&pswd=<Int: range>'
    svr.POST("/hw1", [](const HttpReq *req, HttpResp *resp)
    { // try {
        Json data; std::map<std::string, std::string> &form_kv = req->form_kv();
        
        for (auto &kv: form_kv) data[kv.first] = std::stoi(kv.second);
        int n = data["n"], range = data["range"];
        for (int i = 0; i < n; i++) data["randomNumber"][i] = rand() % range;
        
        Begin = clock(); data["selectionSort"]["data"] = selectionSort(data["randomNumber"], n);
        End   = clock(); data["selectionSort"]["time"] = (double)(End - Begin) / CLOCKS_PER_SEC;

        Begin = clock(); data["bubbleSort"]["data"]    = bubbleSort(data["randomNumber"], n);
        End   = clock(); data["bubbleSort"]["time"]    = (double)(End - Begin) / CLOCKS_PER_SEC;

        resp->Json(data); // throw data; } catch (Json data) { data["error"] = "ThrowErr: Exception Caught"; resp->Json(data); }
    });

    svr.POST("/hw2-hanoi", [](const HttpReq *req, HttpResp *resp)
    { // try {
        Json data; std::map<std::string, std::string> &form_kv = req->form_kv();
        
        for (auto &kv: form_kv) data[kv.first] = std::stoi(kv.second);
        int n = data["n"]; hanoiResult = "";
        hanoi(n, "A", "C", "B"); data["hanoiResult"] = hanoiResult;

        resp->Json(data); // throw data; } catch (Json data) { data["error"] = "ThrowErr: Exception Caught"; resp->Json(data); }
    });

    svr.POST("/hw3", [](const HttpReq *req, HttpResp *resp)
    { // try {
        Json data; std::map<std::string, std::string> &form_kv = req->form_kv();
        
        for (auto &kv: form_kv) data[kv.first] = std::stoi(kv.second);
        int n = data["n"]; magicResult = "";
        if (n % 2) {
            int magicSquare[n][n];
            memset(magicSquare, 0, sizeof(magicSquare));
            int i = n / 2, j = n - 1;
            for (int num = 1; num <= n * n;)
            {
                if (i == -1 && j == n) {
                    j = n - 2; i = 0;
                } else {
                    if (j == n) j = 0;
                    if (i < 0) i = n - 1;
                } if (magicSquare[i][j]) {
                    j -= 2; i++;
                    continue;
                } else magicSquare[i][j] = num++; j++; i--;
            }
            
            magicResult += "Magic Square of size " + to_string(n) + ":\n\n";
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++) magicResult += to_string(magicSquare[i][j]) + " ";
                magicResult += "\n";
            }
        } else magicResult = "n must be odd number!";
        data["magicResult"] = magicResult;
        
        resp->Json(data); // throw data; } catch (Json data) { data["error"] = "ThrowErr: Exception Caught"; resp->Json(data); }
    });

    svr.POST("/hw2-permutation", [](const HttpReq *req, HttpResp *resp)
    { // try {
        Json data; std::map<std::string, std::string> &form_kv = req->form_kv();
        
        for (auto &kv: form_kv) data[kv.first] = kv.second;
        string str = data["str"]; int n = str.length(); perResult = "";
        permute(str, 0, n-1); data["permutation"] = perResult;

        resp->Json(data); // throw data; } catch (Json data) { data["error"] = "ThrowErr: Exception Caught"; resp->Json(data); }
    });

    if (svr.track().start(8888) == 0)
    {
        svr.list_routes();
        wait_group.wait();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
