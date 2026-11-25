// SPDX-License-Identifier: GPL-3.0-or-later
// HttpFetch - the HTTPS behind Settings > Mods > Find mods: the native curl shim (android/native/stubs/curl_stub.cpp)
// calls get() through JNI from the mod list's worker thread. Android's own HttpURLConnection uses the system's
// certificates, so no TLS library or CA bundle has to be shipped.
package com.deivid22srk.dk64recomp;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

final class HttpFetch {
    private HttpFetch() { }

    /** GET url (redirects followed, also http -> https). Returns { Integer status, byte[] body, String error }:
     *  error != null means no answer at all (status 0). Any thread. */
    static Object[] get(String url, String[] headers, String userAgent, int timeoutSeconds) {
        String next = url;
        try {
            for (int hops = 0; hops < 8; hops++) {
                HttpURLConnection c = (HttpURLConnection) new URL(next).openConnection();
                c.setInstanceFollowRedirects(false);
                int ms = Math.max(1, timeoutSeconds) * 1000;
                c.setConnectTimeout(ms);
                c.setReadTimeout(ms);
                if (userAgent != null) c.setRequestProperty("User-Agent", userAgent);
                if (headers != null) {
                    for (String h : headers) {
                        int i = h.indexOf(':');
                        if (i > 0) c.setRequestProperty(h.substring(0, i).trim(), h.substring(i + 1).trim());
                    }
                }
                int status = c.getResponseCode();
                if (status >= 300 && status < 400 && c.getHeaderField("Location") != null) {
                    next = new URL(new URL(next), c.getHeaderField("Location")).toString();
                    c.disconnect();
                    continue;
                }
                InputStream in = status >= 400 ? c.getErrorStream() : c.getInputStream();
                ByteArrayOutputStream out = new ByteArrayOutputStream();
                if (in != null) {
                    byte[] buf = new byte[64 * 1024];
                    for (int n; (n = in.read(buf)) > 0; ) out.write(buf, 0, n);
                    in.close();
                }
                c.disconnect();
                return new Object[] { status, out.toByteArray(), null };
            }
            return new Object[] { 0, new byte[0], "too many redirects" };
        } catch (Exception e) {
            return new Object[] { 0, new byte[0], String.valueOf(e) };
        }
    }
}
