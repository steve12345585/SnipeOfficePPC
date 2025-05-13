// -*- Mode: Java; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-

// Version: MPL 1.1 / GPLv3+ / LGPLv3+
//
// The contents of this file are subject to the Mozilla Public License Version
// 1.1 (the "License"); you may not use this file except in compliance with
// the License or as specified alternatively below. You may obtain a copy of
// the License at http://www.mozilla.org/MPL/
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the
// License.
//
// Major Contributor(s):
// Copyright (C) 2011 Tor Lillqvist <tml@iki.fi> (initial developer)
// Copyright (C) 2011 SUSE Linux http://suse.com (initial developer's employer)
//
// All Rights Reserved.
//
// For minor contributions see the git repository.
//
// Alternatively, the contents of this file may be used under the terms of
// either the GNU General Public License Version 3 or later (the "GPLv3+"), or
// the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
// in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
// instead of those above.

package org.libreoffice.android;

import android.app.Activity;
import android.content.pm.ApplicationInfo;
import android.util.Log;

import java.io.File;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;

public class Bootstrap
{
    private static String TAG = "lo-bootstrap";

    // Native methods in this class are all implemented in
    // sal/android/lo-bootstrap.c as the lo-bootstrap library is loaded with
    // System.loadLibrary() and Android's JNI works only to such libraries, it
    // seems.

    private static native boolean setup(String dataDir,
                                        String cacheDir,
                                        String apkFile);

    // Extracts files in the .apk that need to be extraced into the app's tree
    static native void extract_files();

    // Wrapper for getpid()
    public static native int getpid();

    // Wrapper for system()
    public static native void system(String cmdline);

    // Wrapper for putenv()
    public static native void putenv(String string);

    // A wrapper for InitVCL() in libvcl (svmain.cxx), called indirectly
    // through the lo-bootstrap library
    public static native void initVCL();

    // A wrapper for osl_setCommandArgs(). Before calling
    // osl_setCommandArgs(), argv[0] is prefixed with the parent directory of
    // where the lo-bootstrap library is.
    public static native void setCommandArgs(String[] argv);

    // A wrapper for createWindowFoo() in the vcl library
    public static native int createWindowFoo();

    // A method that starts a thread to redirect stdout and stderr writes to
    // the Android logging mechanism, or stops the redirection.
    public static native boolean redirect_stdio(boolean state);

    // The DIB returned by css.awt.XBitmap.getDIB is in BGR_888 form, at least
    // for Writer documents. We need it in Android's Bitmap.Config.ARGB_888
    // format, which actually is RGBA_888, whee... At least in Android 4.0.3,
    // at least on my device. No idea if it is always like that or not, the
    // documentation sucks.
    public static native void twiddle_BGR_to_RGBA(byte[] source, int offset, int width, int height, ByteBuffer destination);

    public static native void force_full_alpha_array(byte[] array, int offset, int length);

    public static native void force_full_alpha_bb(ByteBuffer buffer, int offset, int length);

    public static native long new_byte_buffer_wrapper(ByteBuffer bbuffer);

    public static native void delete_byte_buffer_wrapper(long bbw);

    static boolean setup_done = false;

    // This setup() method should be called from the upper Java level of
    // LO-based apps.
    public static synchronized void setup(Activity activity)
    {
        if (setup_done)
            return;

        setup_done = true;

        String dataDir = null;

        ApplicationInfo ai = activity.getApplicationInfo();
        dataDir = ai.dataDir;
        Log.i(TAG, String.format("dataDir=%s\n", dataDir));

        redirect_stdio(true);

        if (!setup(dataDir,
                   activity.getApplication().getCacheDir().getAbsolutePath(),
                   activity.getApplication().getPackageResourcePath()))
            return;

        // Extract files from the .apk that can't be used mmapped directly from it
        extract_files();

        // If we notice that a fonts.conf file was extracted, automatically
        // set the FONTCONFIG_FILE env var.
        InputStream i;
        try {
            i = activity.getAssets().open("unpack/etc/fonts/fonts.conf");
        }
        catch (java.io.IOException e) {
            i = null;
        }
        putenv("OOO_DISABLE_RECOVERY=1");
        if (i != null)
            putenv("FONTCONFIG_FILE=" + dataDir + "/etc/fonts/fonts.conf");

        // TMPDIR is used by osl_getTempDirURL()
        putenv("TMPDIR=" + activity.getCacheDir().getAbsolutePath());
    }

    // Now with static loading we always have all native code in one native
    // library which we always call liblo-native-code.so, regardless of the
    // app. The library has already been unpacked into /data/data/<app
    // name>/lib at installation time by the package manager.
    static {
        System.loadLibrary("lo-native-code");
    }
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
