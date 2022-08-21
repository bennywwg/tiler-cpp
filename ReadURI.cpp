#include "ReadURI.hpp"

#include <curl/curl.h>

#include <iostream>

namespace Tiler {
    struct CurlMemoryStruct {
		char* memory;
		size_t size;
	};

	static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
	{
		size_t realsize = size * nmemb;
		CurlMemoryStruct* mem = reinterpret_cast<CurlMemoryStruct*>(userp);

		char* ptr = reinterpret_cast<char*>(realloc(mem->memory, mem->size + realsize + 1));

		//tkAssert(ptr);
		if (!ptr) return 0;

		mem->memory = ptr;
		memcpy(&(mem->memory[mem->size]), contents, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;

		return realsize;
	}

	std::vector<uint8_t> LoadEntireURIBinary(std::string const& path) {
		CURL* curl_handle;
		CURLcode res;

		CurlMemoryStruct chunk;

		chunk.memory = reinterpret_cast<char*>(malloc(1));
		chunk.size = 0;

		curl_global_init(CURL_GLOBAL_ALL);

		curl_handle = curl_easy_init();

		curl_easy_setopt(curl_handle, CURLOPT_URL, path.c_str());

		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);

		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		res = curl_easy_perform(curl_handle);

		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			return { };
		}

		std::vector<uint8_t> mres;
		mres.resize(chunk.size);
		memcpy(mres.data(), chunk.memory, chunk.size);

		curl_easy_cleanup(curl_handle);

		free(chunk.memory);

		return mres;
	}
}