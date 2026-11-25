/*
 * curl_stub.cpp - the part of libcurl RecompFrontend's mod list (Settings > Mods > Find mods) uses, on top of
 * Android's own HTTPS: curl_easy_perform calls HttpFetch.get (Java, HttpURLConnection, the system's certificates)
 * through JNI and hands the answer to the CURLOPT_WRITEFUNCTION in pieces, as libcurl does. Supported options: URL,
 * WRITEFUNCTION, WRITEDATA, TIMEOUT, HTTPHEADER, USERAGENT (FOLLOWLOCATION / SSL_VERIFY* are always on); info:
 * RESPONSE_CODE. (Was an inert stub - "mod store offline" - before.)
 */
#include <curl/curl.h>

#include <SDL_system.h>
#include <jni.h>
#include <stdarg.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace {

typedef size_t (*write_fn)(char *, size_t, size_t, void *);

struct Handle {
    std::string url;
    std::string user_agent;
    long timeout = 30;
    curl_slist *headers = nullptr;
    write_fn write = nullptr;
    void *write_data = nullptr;
    long response_code = 0;
};

jclass fetch_class(JNIEnv *env) {
    static jclass cls = nullptr;
    if (cls != nullptr || env == nullptr) return cls;
    jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
    if (activity == nullptr) return nullptr;
    jclass act_cls = env->GetObjectClass(activity);
    jmethodID get_loader = env->GetMethodID(act_cls, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject loader = env->CallObjectMethod(activity, get_loader);
    jclass loader_cls = env->FindClass("java/lang/ClassLoader");
    jmethodID load = env->GetMethodID(loader_cls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    jstring name = env->NewStringUTF("com.deivid22srk.dk64recomp.HttpFetch");
    jobject found = env->CallObjectMethod(loader, load, name);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        found = nullptr;
    }
    if (found != nullptr) {
        cls = static_cast<jclass>(env->NewGlobalRef(found));
        env->DeleteLocalRef(found);
    }
    env->DeleteLocalRef(name);
    env->DeleteLocalRef(loader_cls);
    env->DeleteLocalRef(loader);
    env->DeleteLocalRef(act_cls);
    env->DeleteLocalRef(activity);
    return cls;
}

} // namespace

extern "C" {

CURLcode curl_global_init(long) {
    return CURLE_OK;
}

void curl_global_cleanup(void) {
}

CURL *curl_easy_init(void) {
    return reinterpret_cast<CURL *>(new Handle());
}

CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...) {
    Handle *h = reinterpret_cast<Handle *>(curl);
    if (h == nullptr) return CURLE_BAD_FUNCTION_ARGUMENT;
    va_list args;
    va_start(args, option);
    switch (option) {
    case CURLOPT_URL: { const char *s = va_arg(args, const char *); h->url = s ? s : ""; break; }
    case CURLOPT_USERAGENT: { const char *s = va_arg(args, const char *); h->user_agent = s ? s : ""; break; }
    case CURLOPT_TIMEOUT: h->timeout = va_arg(args, long); break;
    case CURLOPT_HTTPHEADER: h->headers = va_arg(args, curl_slist *); break;
    case CURLOPT_WRITEFUNCTION: h->write = reinterpret_cast<write_fn>(va_arg(args, void *)); break;
    case CURLOPT_WRITEDATA: h->write_data = va_arg(args, void *); break;
    default: break;   // FOLLOWLOCATION, SSL_VERIFYPEER / SSL_VERIFYHOST: always on
    }
    va_end(args);
    return CURLE_OK;
}

CURLcode curl_easy_perform(CURL *curl) {
    Handle *h = reinterpret_cast<Handle *>(curl);
    if (h == nullptr || h->url.empty()) return CURLE_URL_MALFORMAT;
    JNIEnv *env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
    jclass cls = fetch_class(env);
    if (cls == nullptr) return CURLE_FAILED_INIT;
    static jmethodID get = env->GetStaticMethodID(cls, "get",
        "(Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;I)[Ljava/lang/Object;");
    env->PushLocalFrame(32);
    int count = 0;
    for (curl_slist *s = h->headers; s != nullptr; s = s->next) count++;
    jobjectArray headers = env->NewObjectArray(count, env->FindClass("java/lang/String"), nullptr);
    int i = 0;
    for (curl_slist *s = h->headers; s != nullptr; s = s->next) {
        env->SetObjectArrayElement(headers, i++, env->NewStringUTF(s->data ? s->data : ""));
    }
    jstring url = env->NewStringUTF(h->url.c_str());
    jstring ua = h->user_agent.empty() ? nullptr : env->NewStringUTF(h->user_agent.c_str());
    jobjectArray result = static_cast<jobjectArray>(
        env->CallStaticObjectMethod(cls, get, url, headers, ua, static_cast<jint>(h->timeout)));
    CURLcode code = CURLE_OK;
    if (env->ExceptionCheck() || result == nullptr) {
        env->ExceptionClear();
        code = CURLE_COULDNT_CONNECT;
    } else {
        jobject status = env->GetObjectArrayElement(result, 0);
        jmethodID int_value = env->GetMethodID(env->FindClass("java/lang/Integer"), "intValue", "()I");
        h->response_code = env->CallIntMethod(status, int_value);
        jobject error = env->GetObjectArrayElement(result, 2);
        if (error != nullptr) {
            code = CURLE_COULDNT_CONNECT;
        } else if (h->write != nullptr) {
            jbyteArray body = static_cast<jbyteArray>(env->GetObjectArrayElement(result, 1));
            const jsize n = env->GetArrayLength(body);
            jbyte *bytes = env->GetByteArrayElements(body, nullptr);
            // in pieces, as libcurl hands them over (a callback that takes less stops the transfer)
            for (jsize off = 0; off < n; ) {
                const size_t piece = static_cast<size_t>(std::min<jsize>(n - off, 64 * 1024));
                if (h->write(reinterpret_cast<char *>(bytes) + off, 1, piece, h->write_data) != piece) {
                    code = CURLE_WRITE_ERROR;
                    break;
                }
                off += static_cast<jsize>(piece);
            }
            env->ReleaseByteArrayElements(body, bytes, JNI_ABORT);
        }
    }
    env->PopLocalFrame(nullptr);
    return code;
}

CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...) {
    Handle *h = reinterpret_cast<Handle *>(curl);
    va_list args;
    va_start(args, info);
    CURLcode code = CURLE_BAD_FUNCTION_ARGUMENT;
    if (info == CURLINFO_RESPONSE_CODE) {
        long *out = va_arg(args, long *);
        if (out != nullptr) *out = h != nullptr ? h->response_code : 0;
        code = CURLE_OK;
    }
    va_end(args);
    return code;
}

void curl_easy_cleanup(CURL *curl) {
    delete reinterpret_cast<Handle *>(curl);
}

const char *curl_easy_strerror(CURLcode code) {
    switch (code) {
    case CURLE_OK: return "No error";
    case CURLE_COULDNT_CONNECT: return "Couldn't connect to server (no internet?)";
    case CURLE_WRITE_ERROR: return "Failed writing received data";
    case CURLE_URL_MALFORMAT: return "URL using bad/illegal format or missing URL";
    case CURLE_FAILED_INIT: return "Failed initialization";
    default: return "Network error";
    }
}

struct curl_slist *curl_slist_append(struct curl_slist *list, const char *data) {
    auto *item = static_cast<curl_slist *>(malloc(sizeof(curl_slist)));
    if (!item) return list;
    item->data = strdup(data ? data : "");
    item->next = nullptr;
    if (!list) return item;
    struct curl_slist *end = list;
    while (end->next) end = end->next;
    end->next = item;
    return list;
}

void curl_slist_free_all(struct curl_slist *list) {
    while (list) {
        curl_slist *next = list->next;
        free(list->data);
        free(list);
        list = next;
    }
}

} /* extern "C" */
