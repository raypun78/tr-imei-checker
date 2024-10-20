#include <iostream>
#include <string>
#include <Windows.h>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include "Header.h"
#define CURL_STATICLIB

#define USERAGENT "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/105.0.0.0 Safari/537.36"

#include "curl.h"
using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

bool fexist(const char* fileName)
{
	ifstream infile(fileName);
	return infile.good();
}

int prx_ = 0;
int nmb_ = 0;
bool sifreliproxy;

void proxcount() {
	if (fexist("proxy.txt")) {
		string re1;
		ifstream prxy;
		prxy.open("proxy.txt");
		while (getline(prxy, re1)) {
			prx_++;
		}
		prxy.close();
	}
	else {
		cout << "Could not find file proxy.txt";
		exit(0);
	}
}

void numbcount() {
	if (fexist("list.txt")) {
		string nm_;
		ifstream numb;
		numb.open("list.txt");
		while (getline(numb, nm_)) {
			nmb_++;
		}
		numb.close();
	}
	else {
		cout << "Could not find file list.txt";
		exit(0);
	}
}


void curlse() {
	proxcount();
	numbcount();
	int prc = 0;
	//
	string** proxy = new string*[prx_];
	for (int i = 0; i < prx_; i++) {  //alloc proxy
		proxy[i] = new string;
	}
	//
	string** number = new string * [nmb_];  //alloc numbers
	for (int i = 0; i < nmb_; i++) {
		number[i] = new string;
	}//


	ifstream pr;
	pr.open("proxy.txt");
	int pri = 0;
	while (getline(pr, *proxy[pri])) { //assign proxies
		pri++;
	}
	pr.close();

	ifstream numero;
	numero.open("list.txt");
	for (int i = 0; i < nmb_; i++) {  //assign numbers
		getline(numero, *number[i]);
	}
	numero.close();

	string** proxyset = new string * [prx_];

	if (sifreliproxy) {
		for (int i = 0; i < prx_; i++) {  //alloc proxy
			proxyset[i] = new string;
		}

		string buff;
		int sayac = 0;
		for (int i = 0; i < prx_; i++) {
			sayac = 0;
			buff = *proxy[i];
			
			for (int d = 0; d < buff.length(); d++) {
				if (buff[d] == ':')
					sayac++;
				if (sayac == 2) {
					int dd = d + 1;
					buff.erase(0, dd);
					*proxyset[i] = buff;
					(*proxy[i]).erase(d, (*proxy[i]).length() - d);
					break;
				}
			}
		}
	}

	CURL* curl;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	CURLcode res;

	for (int i = 0 ; i < nmb_; i++) {
		string readBuffer;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_REFERER, referer.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, false);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, USERAGENT);
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookie.txt");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
		
		curl_easy_setopt(curl, CURLOPT_PROXY, (*proxy[prc]).c_str());
		if (sifreliproxy)
			curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, (*proxyset[prc]).c_str());

		res = curl_easy_perform(curl);

		istringstream ddd(readBuffer);

		string line;
		__int64 offset;

		while (getline(ddd, line)) {
			if ((offset = line.find("<input type=\"hidden\" name=\"token\" value=")) != string::npos) {
				break;
			}
		}
		int hold = 0;
		for (int i = 0; i < line.length(); i++) {
			if (line[i] == '{') {
				hold = i;
				break;
			}
		}
		line.erase(0, hold);
		line.erase(line.length() - 4, 4);
		
		size_t size_ = line.length();
		int size = static_cast<int>(size_);
		const char* linen = line.c_str();
		char* output2 = curl_easy_escape(curl, linen, size);

		string p2 = output2;
		curl_free(output2);
		string post = "txtImei=" + *number[i] + "&token=" + p2;

		string read;
		curl_easy_setopt(curl, CURLOPT_URL, url2.c_str());
		curl_easy_setopt(curl, CURLOPT_REFERER, referer.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, USERAGENT);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
		curl_easy_setopt(curl, CURLOPT_PROXY, (*proxy[prc]).c_str());

		if (sifreliproxy)
			curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, (*proxyset[prc]).c_str());

		curl_easy_setopt(curl, CURLOPT_POST, true);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());  //c_str olmasý önemli...

		res = curl_easy_perform(curl);

		if (prc < prx_)
			prc++;
		if(prc==prx_)
			prc = 0;

		istringstream last(read);

		int head = 0;
		int count = 0;
		__int64 offset2;

		string kaynaklar[4];

		while (getline(last, line)) {
			if ((offset2 = line.find("<dt>Durum</dt>")) != string::npos) {
				head = count;
			}

			if (count == head + 1)
				kaynaklar[0] = line;
			else if (count == head + 3)
				kaynaklar[1] = line;
			else if (count == head + 5)
				kaynaklar[2] = line;
			else if (count == head + 7)
				kaynaklar[3] = line;
			count++;
		}

		for (int i = 0; i < 4; i++) {
			kaynaklar[i].erase(0, 20);
		}
		for (int i = 0; i < 4; i++) {

			for (int t = 0; t < kaynaklar[i].length(); t++) {
				if (kaynaklar[i][t] == '<') {
					kaynaklar[i].erase(t, 5);
				}
			}

		}

		std::cout << "IMEI: " << *number[i] << std::endl;
		std::cout << "Durum: " << kaynaklar[0] << std::endl;
		std::cout << "Kaynak: " << kaynaklar[1] << std::endl;
		std::cout << "Sorgu Tarihi: " << kaynaklar[2] << std::endl;
		std::cout << "Marka/Model: " << kaynaklar[3] << std::endl;

	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();


	for (int i = 0; i < nmb_; i++) {  //dealloc number
		delete number[i];
	}
	delete[] number;

	//
	for (int i = 0; i < prx_; i++) { //dealloc proxy
		delete proxy[i];
		
	}
	delete[] proxy;
	
	if (sifreliproxy) {
		for (int i = 0; i < prx_; i++) { //dealloc proxy
			delete proxyset[i];
		}
	}
	delete[] proxyset;

	exit(0);
}

int main(int argc, const char* args[]) {
	int secim;
	setlocale(LC_ALL, "Turkish");
	cout << "Proxy listenizdeki proxyler þifreli mi? \n1. Evet\n2. Hayýr\n>> ";
	cin >> secim;
	if (secim == 1)
		sifreliproxy = true;
	else if (secim == 2)
		sifreliproxy = false;
	else {
		cout << "Undefined input value";
		exit(0);
	}

	curlse();

	CURL* curl;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	CURLcode res;
	string readBuffer;

	string numara = "356254125468590";

	//proxy kullanýrken proxy responde code için: CURLINFO_HTTP_CONNECTCODE

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		curl_easy_setopt(curl, CURLOPT_REFERER, "https://www.turkiye.gov.tr/");

		curl_easy_setopt(curl, CURLOPT_POST, false);

		//curl_easy_setopt(curl, CURLOPT_PROXY, PROXY.c_str());


		curl_easy_setopt(curl, CURLOPT_USERAGENT, USERAGENT);

		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt");

		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt");

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

		curl_easy_setopt(curl, CURLOPT_HTTPGET, CURLINFO_HTTP_CODE);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		

		
		res = curl_easy_perform(curl);

		if (res == CURLE_OK) {
			std::cout << endl << endl << "STATUS: OK" << endl << endl;
		}

		if (curl_easy_strerror(res)) {
			std::cout << curl_easy_strerror(res) << endl << endl;
			
		}
		cout << readBuffer << endl << endl;
		long lo;
		curl_easy_getinfo(curl,  CURLINFO_HTTP_CODE, &lo);

		curl_easy_cleanup(curl);

		curl_global_cleanup();

		std::cout << "HTTP CODE: " << lo << endl << endl << endl;

		istringstream ddd(readBuffer);

		string line;
		__int64 offset;

		while (getline(ddd, line)) {
			if ((offset = line.find("<input type=\"hidden\" name=\"token\" value="))!=string::npos) {
				break;
			}
		}
		int hold = 0;
		for (int i = 0; i < line.length(); i++) {
			if (line[i] == '{') {
				hold = i;
				break;
			}
		}
		line.erase(0, hold);
		line.erase(line.length()-4, 4);

		size_t size_ = line.length();
		int size = static_cast<int>(size_);
		const char* linen = line.c_str();
		char* output2 = curl_easy_escape(curl, linen, size);

		string p2 = output2;
		curl_free(output2);
		string post = "txtImei="+numara+"&token=" + p2;

		CURL* curl1 = curl_easy_init();
		curl_global_init(CURL_GLOBAL_ALL);

		url = "https://www.turkiye.gov.tr/imei-sorgulama?submit";

		string read;

		curl_easy_setopt(curl1, CURLOPT_URL, url.c_str());

		curl_easy_setopt(curl1, CURLOPT_REFERER, "https://www.turkiye.gov.tr/imei-sorgulama");

		curl_easy_setopt(curl1, CURLOPT_USERAGENT, USERAGENT);

		curl_easy_setopt(curl1, CURLOPT_FOLLOWLOCATION, true);

		//curl_easy_setopt(curl1, CURLOPT_PROXY, PROXY.c_str());

		curl_easy_setopt(curl1, CURLOPT_POST, true);

		curl_easy_setopt(curl1, CURLOPT_SSL_VERIFYPEER, 0);

		curl_easy_setopt(curl1, CURLOPT_SSL_VERIFYHOST, 0);

		curl_easy_setopt(curl1, CURLOPT_COOKIEJAR, "cookies.txt");

		curl_easy_setopt(curl1, CURLOPT_COOKIEFILE, "cookies.txt");

		curl_easy_setopt(curl1, CURLOPT_WRITEFUNCTION, WriteCallback);

		curl_easy_setopt(curl1, CURLOPT_WRITEDATA, &read);

		curl_easy_setopt(curl1, CURLOPT_POSTFIELDS, post.c_str());  //c_str olmasý önemli...

		res = curl_easy_perform(curl1);

		curl_easy_cleanup(curl1);

		curl_global_cleanup();

		istringstream last(read);

		int head = 0;
		int count = 0;
		__int64 offset2;

		string kaynaklar[4];

		while (getline(last, line)) {
			if ((offset2 = line.find("<dt>Durum</dt>")) != string::npos) {
				head = count;
			}

			if (count == head + 1)
				kaynaklar[0] = line;
			else if (count == head + 3)
				kaynaklar[1] = line;
			else if (count == head + 5)
				kaynaklar[2] = line;
			else if (count == head + 7) 
				kaynaklar[3] = line;
			count++;
		}

		for (int i = 0; i < 4; i++) {
			kaynaklar[i].erase(0, 20);			
		}
		for (int i = 0; i < 4; i++) {

			for (int t = 0; t < kaynaklar[i].length(); t++) {
				if (kaynaklar[i][t] == '<') {
					kaynaklar[i].erase(t, 5);
				}
			}

		}

		std::cout << "IMEI: " << numara << std::endl;
		std::cout << "Durum: " << kaynaklar[0] << std::endl;
		std::cout << "Kaynak: " << kaynaklar[1] << std::endl;
		std::cout << "Sorgu Tarihi: " << kaynaklar[2] << std::endl;
		std::cout << "Marka/Model: " << kaynaklar[3] << std::endl<<endl<<endl;   //proxy lazým

	}
	
	else {
		std::cout << "curl initialization failure." << std::endl << std::endl;
		return 128;
	}
}
