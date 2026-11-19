// TouchNative - the JNI side of the touch buttons (android/native/compat/virtual_pad.cpp in libmain.so).
// SPDX-License-Identifier: GPL-3.0-or-later
package com.deivid22srk.dk64recomp;

final class TouchNative {
    private TouchNative() { }

    /** TouchOverlay's button bits and its stick (-1..1, y down = +). */
    static native void nativeTouchPad(int buttons, float x, float y);
    /** The game runs (not the launcher screen). */
    static native boolean nativeGameRunning();
    /** fps, average and longest frame time (ms) of the last half second (counted per frame drawn). */
    static native float[] nativeFrameStats();
    /** One of the game's menus takes input (options menu, launcher dialogs). */
    static native boolean nativeMenuOpen();
}
