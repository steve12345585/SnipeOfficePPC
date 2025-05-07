/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 *   Copyright (C) 2012 Novell, Inc.
 *   Michael Meeks <michael.meeks@suse.com> (initial developer)
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */
#include <android/androidinst.hxx>
#include <headless/svpdummies.hxx>
#include <generic/gendata.hxx>
#include <jni.h>
#include <android/log.h>
#include <android/looper.h>
#include <osl/detail/android-bootstrap.h>
#include <osl/detail/android_native_app_glue.h>
#include <rtl/strbuf.hxx>
#include <basebmp/scanlineformats.hxx>

class AndroidSalData : public SalGenericData
{
public:
    AndroidSalData( SalInstance *pInstance ) : SalGenericData( SAL_DATA_ANDROID, pInstance ) {}
    virtual void ErrorTrapPush() {}
    virtual bool ErrorTrapPop( bool ) { return false; }
};

static rtl::OString MotionEdgeFlagsToString(int32_t nFlags)
{
    rtl::OStringBuffer aStr;
    if (nFlags == AMOTION_EVENT_EDGE_FLAG_NONE)
        aStr.append ("no-edge");
    if (nFlags & AMOTION_EVENT_EDGE_FLAG_TOP)
        aStr.append (":top");
    if (nFlags & AMOTION_EVENT_EDGE_FLAG_BOTTOM)
        aStr.append (":bottom");
    if (nFlags & AMOTION_EVENT_EDGE_FLAG_LEFT)
        aStr.append (":left");
    if (nFlags & AMOTION_EVENT_EDGE_FLAG_RIGHT)
        aStr.append (":right");
    return aStr.makeStringAndClear();
}

static rtl::OString KeyMetaStateToString(int32_t nFlags)
{
    rtl::OStringBuffer aStr;
    if (nFlags == AMETA_NONE)
        aStr.append ("no-meta");
    if (nFlags & AMETA_ALT_ON)
        aStr.append (":alt");
    if (nFlags & AMETA_SHIFT_ON)
        aStr.append (":shift");
    if (nFlags & AMETA_SYM_ON)
        aStr.append (":sym");
    return aStr.makeStringAndClear();
}

static sal_uInt16 KeyMetaStateToCode(AInputEvent *event)
{
    sal_uInt16 nCode = 0;
    int32_t nFlags = AKeyEvent_getMetaState(event);
    if (nFlags & AMETA_SHIFT_ON)
        nCode |= KEY_SHIFT;
    if (nFlags & AMETA_SYM_ON)
        nCode |= KEY_MOD1;
    if (nFlags & AMETA_ALT_ON)
        nCode |= KEY_MOD2;
    return nCode;
}

static sal_uInt16 KeyToCode(AInputEvent *event)
{
    sal_uInt16 nCode = 0;
    switch (AKeyEvent_getKeyCode(event)) {
#define MAP(a,b)                                 \
    case AKEYCODE_##a: nCode = KEY_##b; break
#define MAP_SAME(a) MAP(a,a)

    MAP_SAME(HOME);
    MAP_SAME(0); MAP_SAME(1); MAP_SAME(2); MAP_SAME(3); MAP_SAME(4);
    MAP_SAME(5); MAP_SAME(6); MAP_SAME(7); MAP_SAME(8); MAP_SAME(9);

    MAP_SAME(A); MAP_SAME(B); MAP_SAME(C); MAP_SAME(D);
    MAP_SAME(E); MAP_SAME(F); MAP_SAME(G); MAP_SAME(H);
    MAP_SAME(I); MAP_SAME(J); MAP_SAME(K); MAP_SAME(L);
    MAP_SAME(M); MAP_SAME(N); MAP_SAME(O); MAP_SAME(P);
    MAP_SAME(Q); MAP_SAME(R); MAP_SAME(S); MAP_SAME(T);
    MAP_SAME(U); MAP_SAME(V); MAP_SAME(W); MAP_SAME(X);
    MAP_SAME(Y); MAP_SAME(Z);

    MAP_SAME(TAB); MAP_SAME(SPACE); MAP_SAME(COMMA);

    MAP(ENTER,RETURN);
    MAP(PAGE_UP, PAGEUP);
    MAP(PAGE_DOWN, PAGEDOWN);
    MAP(DEL, DELETE);
    MAP(PERIOD, POINT);

    MAP(DPAD_UP, UP); MAP(DPAD_DOWN, DOWN);
    MAP(DPAD_LEFT, LEFT); MAP(DPAD_RIGHT, RIGHT);

    case AKEYCODE_BACK: // escape ?
    case AKEYCODE_UNKNOWN:
    case AKEYCODE_SOFT_LEFT:
    case AKEYCODE_SOFT_RIGHT:
    case AKEYCODE_CALL:
    case AKEYCODE_ENDCALL:
    case AKEYCODE_STAR:
    case AKEYCODE_POUND:
    case AKEYCODE_VOLUME_UP:
    case AKEYCODE_VOLUME_DOWN:
    case AKEYCODE_DPAD_CENTER:
    case AKEYCODE_POWER:
    case AKEYCODE_CAMERA:
    case AKEYCODE_CLEAR:
    case AKEYCODE_ALT_LEFT:
    case AKEYCODE_ALT_RIGHT:
    case AKEYCODE_SHIFT_LEFT:
    case AKEYCODE_SHIFT_RIGHT:
    case AKEYCODE_SYM:
    case AKEYCODE_EXPLORER:
    case AKEYCODE_ENVELOPE:
    case AKEYCODE_GRAVE:
    case AKEYCODE_MINUS:
    case AKEYCODE_EQUALS:
    case AKEYCODE_LEFT_BRACKET:
    case AKEYCODE_RIGHT_BRACKET:
    case AKEYCODE_BACKSLASH:
    case AKEYCODE_SEMICOLON:
    case AKEYCODE_APOSTROPHE:
    case AKEYCODE_SLASH:
    case AKEYCODE_AT:
    case AKEYCODE_NUM:
    case AKEYCODE_HEADSETHOOK:
    case AKEYCODE_FOCUS: // not widget, but camera focus
    case AKEYCODE_PLUS:
    case AKEYCODE_MENU:
    case AKEYCODE_NOTIFICATION:
    case AKEYCODE_SEARCH:
    case AKEYCODE_MEDIA_PLAY_PAUSE:
    case AKEYCODE_MEDIA_STOP:
    case AKEYCODE_MEDIA_NEXT:
    case AKEYCODE_MEDIA_PREVIOUS:
    case AKEYCODE_MEDIA_REWIND:
    case AKEYCODE_MEDIA_FAST_FORWARD:
    case AKEYCODE_MUTE:
    case AKEYCODE_PICTSYMBOLS:
    case AKEYCODE_SWITCH_CHARSET:
    case AKEYCODE_BUTTON_A:
    case AKEYCODE_BUTTON_B:
    case AKEYCODE_BUTTON_C:
    case AKEYCODE_BUTTON_X:
    case AKEYCODE_BUTTON_Y:
    case AKEYCODE_BUTTON_Z:
    case AKEYCODE_BUTTON_L1:
    case AKEYCODE_BUTTON_R1:
    case AKEYCODE_BUTTON_L2:
    case AKEYCODE_BUTTON_R2:
    case AKEYCODE_BUTTON_THUMBL:
    case AKEYCODE_BUTTON_THUMBR:
    case AKEYCODE_BUTTON_START:
    case AKEYCODE_BUTTON_SELECT:
    case AKEYCODE_BUTTON_MODE:
        fprintf (stderr, "un-mapped keycode %d\n", nCode);
        nCode = 0;
        break;
#undef MAP_SAME
#undef MAP
    }
    fprintf (stderr, "mapped %d -> %d\n", AKeyEvent_getKeyCode(event), nCode);
    return nCode;
}

static void BlitFrameRegionToWindow(ANativeWindow_Buffer *pOutBuffer,
                                    const basebmp::BitmapDeviceSharedPtr& aDev,
                                    const ARect &rSrcRect,
                                    int nDestX, int nDestY)
{
    fprintf (stderr, "Blit frame src %d,%d->%d,%d to position %d, %d\n",
             rSrcRect.left, rSrcRect.top, rSrcRect.right, rSrcRect.bottom,
             nDestX, nDestY);

    basebmp::RawMemorySharedArray aSrcData = aDev->getBuffer();
    basegfx::B2IVector aDevSize = aDev->getSize();
    sal_Int32 nStride = aDev->getScanlineStride();
    unsigned char *pSrc = aSrcData.get();

    // FIXME: do some cropping goodness on aSrcRect to ensure no overflows etc.
    ARect aSrcRect = rSrcRect;

    // FIXME: by default we have WINDOW_FORMAT_RGB_565 = 4 ...
    for (unsigned int y = 0; y < (unsigned int)(aSrcRect.bottom - aSrcRect.top); y++)
    {
        unsigned char *sp = ( pSrc + nStride * (aSrcRect.top + y) +
                              aSrcRect.left * 3 /* src pixel size */ );

        switch (pOutBuffer->format) {
        case WINDOW_FORMAT_RGBA_8888:
        case WINDOW_FORMAT_RGBX_8888:
        {
            unsigned char *dp = ( (unsigned char *)pOutBuffer->bits +
                                  pOutBuffer->stride * 4 * (y + nDestY) +
                                  nDestX * 4 /* dest pixel size */ );
            for (unsigned int x = 0; x < (unsigned int)(aSrcRect.right - aSrcRect.left); x++)
            {
                dp[x*4 + 0] = sp[x*3 + 2]; // R
                dp[x*4 + 1] = sp[x*3 + 1]; // G
                dp[x*4 + 2] = sp[x*3 + 0]; // B
                dp[x*4 + 3] = 255; // A
            }
            break;
        }
        case WINDOW_FORMAT_RGB_565:
        {
            unsigned char *dp = ( (unsigned char *)pOutBuffer->bits +
                                  pOutBuffer->stride * 2 * (y + nDestY) +
                                  nDestX * 2 /* dest pixel size */ );
            for (unsigned int x = 0; x < (unsigned int)(aSrcRect.right - aSrcRect.left); x++)
            {
                unsigned char b = sp[x*3 + 0]; // B
                unsigned char g = sp[x*3 + 1]; // G
                unsigned char r = sp[x*3 + 2]; // R
                dp[x*2 + 0] = (r & 0xf8) | (g >> 5);
                dp[x*2 + 1] = ((g & 0x1c) << 5) | ((b & 0xf8) >> 3);
            }
            break;
        }
        default:
            fprintf (stderr, "unknown pixel format %d !\n", pOutBuffer->format);
            break;
        }
    }
}

void AndroidSalInstance::BlitFrameToWindow(ANativeWindow_Buffer *pOutBuffer,
                                           const basebmp::BitmapDeviceSharedPtr& aDev)
{
    basegfx::B2IVector aDevSize = aDev->getSize();
    ARect aWhole = { 0, 0, aDevSize.getX(), aDevSize.getY() };
    BlitFrameRegionToWindow(pOutBuffer, aDev, aWhole, 0, 0);
}

void AndroidSalInstance::RedrawWindows(ANativeWindow *pWindow)
{
    if (!pWindow)
        return;

    ANativeWindow_Buffer aOutBuffer;
    memset ((void *)&aOutBuffer, 0, sizeof (aOutBuffer));

//    ARect aRect;
    fprintf (stderr, "pre lock #3\n");
    int32_t nRet = ANativeWindow_lock(pWindow, &aOutBuffer, NULL);
    fprintf (stderr, "locked window %d returned " // rect:  %d,%d->%d,%d "
             "buffer: %dx%d stride %d, format %d, bits %p\n",
             nRet, // aRect.left, aRect.top, aRect.right, aRect.bottom,
             aOutBuffer.width, aOutBuffer.height, aOutBuffer.stride,
             aOutBuffer.format, aOutBuffer.bits);
    if (aOutBuffer.bits != NULL)
    {

#if 0   // pre-'clean' the buffer with cruft:
        // hard-code / guess at a format ...
        int32_t *p = (int32_t *)aOutBuffer.bits;
        for (int32_t y = 0; y < aOutBuffer.height; y++)
        {
            for (int32_t x = 0; x < aOutBuffer.stride; x++)
                *p++ = (y << 24) + (x << 10) + 0xff ;
        }

#endif
        int i = 0;
        std::list< SalFrame* >::const_iterator it;
        for ( it = getFrames().begin(); it != getFrames().end(); i++, it++ )
        {
            SvpSalFrame *pFrame = static_cast<SvpSalFrame *>(*it);

            if (pFrame->IsVisible())
            {
                fprintf( stderr, "render visible frame %d\n", i );
#ifndef REGION_RE_RENDER
                BlitFrameToWindow (&aOutBuffer, pFrame->getDevice());
#else
                // Sadly it seems that due to double buffering, we don't
                // get back in our buffer what we had there last time - so we cannot
                // do incremental rendering. Presumably this will require us to
                // render to a bitmap, and keep that updated instead in future.

                // Intersect re-rendering region with this frame
                Region aClipped( maRedrawRegion );
                basegfx::B2IVector aDevSize = pFrame->getDevice()->getSize();
                aClipped.Intersect( Rectangle( 0, 0, aDevSize.getX(), aDevSize.getY() ) );

                Rectangle aSubRect;
                RegionHandle aHdl = aClipped.BeginEnumRects();
                while( aClipped.GetNextEnumRect( aHdl, aSubRect ) )
                {
                    ARect aASubRect = { aSubRect.Left(), aSubRect.Top(),
                                        aSubRect.Right(), aSubRect.Bottom() };
                    BlitFrameRegionToWindow(&aOutBuffer, pFrame->getDevice(),
                                            aASubRect,
                                            aSubRect.Left(), aSubRect.Top());
                }
                aClipped.EndEnumRects( aHdl );
#endif
            }
        }
    }
    else
        fprintf (stderr, "no buffer for locked window\n");
    ANativeWindow_unlockAndPost(pWindow);

    fprintf (stderr, "done render!\n");
    maRedrawRegion.SetEmpty();
    mbQueueReDraw = false;
}

void AndroidSalInstance::damaged(AndroidSalFrame */* frame */, const Rectangle &rRect)
{
    // FIXME: translate rRect to the frame's offset ...
    maRedrawRegion.Union( rRect );
    mbQueueReDraw = true;
}

static const char *app_cmd_name(int cmd)
{
    switch (cmd) {
    case APP_CMD_INPUT_CHANGED: return "INPUT_CHANGED";
    case APP_CMD_INIT_WINDOW: return "INIT_WINDOW";
    case APP_CMD_TERM_WINDOW: return "TERM_WINDOW";
    case APP_CMD_WINDOW_RESIZED: return "WINDOW_RESIZED";
    case APP_CMD_WINDOW_REDRAW_NEEDED: return "WINDOW_REDRAW_NEEDED";
    case APP_CMD_CONTENT_RECT_CHANGED: return "CONTENT_RECT_CHANGED";
    case APP_CMD_GAINED_FOCUS: return "GAINED_FOCUS";
    case APP_CMD_LOST_FOCUS: return "LOST_FOCUS";
    case APP_CMD_CONFIG_CHANGED: return "CONFIG_CHANGED";
    case APP_CMD_LOW_MEMORY: return "LOW_MEMORY";
    case APP_CMD_START: return "START";
    case APP_CMD_RESUME: return "RESUME";
    case APP_CMD_SAVE_STATE: return "SAVE_STATE";
    case APP_CMD_PAUSE: return "PAUSE";
    case APP_CMD_STOP: return "STOP";
    case APP_CMD_DESTROY: return "DESTROY";
    default:
        static char buf[10];
        sprintf(buf, "%d", cmd);
        return buf;
    }
}

void AndroidSalInstance::GetWorkArea( Rectangle& rRect )
{
    if (!mpApp || !mpApp->window)
        rRect = Rectangle( Point( 0, 0 ),
                           Size( 800, 600 ) );
    else
        rRect = Rectangle( Point( 0, 0 ),
                           Size( ANativeWindow_getWidth( mpApp->window ),
                                 ANativeWindow_getHeight( mpApp->window ) ) );
}

void AndroidSalInstance::onAppCmd (struct android_app* app, int32_t cmd)
{
        fprintf (stderr, "app cmd for app %p: %s\n", app, app_cmd_name(cmd));
        ANativeWindow *pWindow = mpApp->window;
        switch (cmd) {
        case APP_CMD_INIT_WINDOW:
        {
            ARect aRect = { 0, 0, 0, 0 };
            aRect.right = ANativeWindow_getWidth(pWindow);
            aRect.bottom = ANativeWindow_getHeight(pWindow);
            int nRet = ANativeWindow_setBuffersGeometry(
                                pWindow, ANativeWindow_getWidth(pWindow),
                                ANativeWindow_getHeight(pWindow),
                                WINDOW_FORMAT_RGBA_8888);
            fprintf (stderr, "we have an app window ! %p %dx%x (%d) set %d\n",
                     pWindow, aRect.right, aRect.bottom,
                     ANativeWindow_getFormat(pWindow), nRet);
            maRedrawRegion = Region( Rectangle( 0, 0, ANativeWindow_getWidth(pWindow),
                                                ANativeWindow_getHeight(pWindow) ) );
            mbQueueReDraw = true;
            break;
        }
        case APP_CMD_WINDOW_RESIZED:
        {
            ARect aRect = { 0, 0, 0, 0 };
            aRect.right = ANativeWindow_getWidth(pWindow);
            aRect.bottom = ANativeWindow_getHeight(pWindow);
            fprintf (stderr, "app window resized to ! %p %dx%x (%d)\n",
                     pWindow, aRect.right, aRect.bottom,
                     ANativeWindow_getFormat(pWindow));
            break;
        }

        case APP_CMD_WINDOW_REDRAW_NEEDED:
        {
            fprintf (stderr, "redraw needed\n");
            maRedrawRegion = Region( Rectangle( 0, 0, ANativeWindow_getWidth(pWindow),
                                                ANativeWindow_getHeight(pWindow) ) );
            mbQueueReDraw = true;
            break;
        }

        case APP_CMD_CONTENT_RECT_CHANGED:
        {
            ARect aRect = mpApp->contentRect;
            fprintf (stderr, "content rect changed [ k/b popped up etc. ] %d,%d->%d,%d\n",
                     aRect.left, aRect.top, aRect.right, aRect.bottom);
            break;
        }
        default:
            fprintf (stderr, "unhandled app cmd %d\n", cmd);
            break;
        }
}

/*
 * Try too hard to get a frame, in the absence of anything better to do
 */
SalFrame *AndroidSalInstance::getFocusFrame() const
{
    SalFrame *pFocus = SvpSalFrame::GetFocusFrame();
    if (!pFocus) {
        fprintf (stderr, "no focus frame, re-focusing first visible frame\n");
        const std::list< SalFrame* >& rFrames( getFrames() );
        for( std::list< SalFrame* >::const_iterator it = rFrames.begin(); it != rFrames.end(); ++it )
        {
            SvpSalFrame *pFrame = const_cast<SvpSalFrame*>(static_cast<const SvpSalFrame*>(*it));
            if( pFrame->IsVisible() )
            {
                pFrame->GetFocus();
                pFocus = pFrame;
                break;
            }
        }
    }
    return pFocus;
}

int32_t AndroidSalInstance::onInputEvent (struct android_app* app, AInputEvent* event)
{
    bool bHandled = false;
    fprintf (stderr, "input event for app %p, event %p type %d source %d device id %d\n",
             app, event,
             AInputEvent_getType(event),
             AInputEvent_getSource(event),
             AInputEvent_getDeviceId(event));

    switch (AInputEvent_getType(event))
    {
    case AINPUT_EVENT_TYPE_KEY:
    {
        int32_t nAction = AKeyEvent_getAction(event);
        fprintf (stderr, "key event keycode %d '%s' %s flags (0x%x) 0x%x\n",
                 AKeyEvent_getKeyCode(event),
                 nAction == AKEY_EVENT_ACTION_DOWN ? "down" :
                 nAction == AKEY_EVENT_ACTION_UP ? "up" : "multiple",
                 KeyMetaStateToString(AKeyEvent_getMetaState(event)).getStr(),
                 AKeyEvent_getMetaState (event),
                 AKeyEvent_getFlags (event));

        // FIXME: the whole SALEVENT_KEYMODCHANGE stuff is going to be interesting
        // can we really emit that ? no input method madness required though.
        sal_uInt16 nEvent;
        SalKeyEvent aEvent;
        int64_t nNsTime = AKeyEvent_getEventTime(event);

        // FIXME: really we need a Java wrapper app as Mozilla has that does
        // key event translation for us, and provides -much- cleaner events.
        nEvent = (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_UP ?
                  SALEVENT_KEYUP : SALEVENT_KEYINPUT);
        sal_uInt16 nCode = KeyToCode(event);
        sal_uInt16 nMetaState = KeyMetaStateToCode(event);
        if (nCode >= KEY_0 && nCode <= KEY_9)
            aEvent.mnCharCode = '0' + nCode - KEY_0;
        else if (nCode >= KEY_A && nCode <= KEY_Z)
            aEvent.mnCharCode = (nMetaState & KEY_SHIFT ? 'A' : 'a') + nCode - KEY_A;
        else if (nCode == KEY_SPACE)
            aEvent.mnCharCode = ' ';
        else if (nCode == KEY_COMMA)
            aEvent.mnCharCode = ',';
        else if (nCode == KEY_POINT)
            aEvent.mnCharCode = '.';
        else
            aEvent.mnCharCode = 0;
        aEvent.mnTime = nNsTime / (1000 * 1000);
        aEvent.mnCode = nMetaState | nCode;
        aEvent.mnRepeat = AKeyEvent_getRepeatCount(event);

        SalFrame *pFocus = getFocusFrame();
        if (pFocus)
            bHandled = pFocus->CallCallback( nEvent, &aEvent );
        else
            fprintf (stderr, "no focused frame to emit event on\n");

        fprintf( stderr, "bHandled == %s\n", bHandled? "true": "false" );
        break;
    }
    case AINPUT_EVENT_TYPE_MOTION:
    {
        size_t nPoints = AMotionEvent_getPointerCount(event);
        fprintf (stderr, "motion event %d %g,%g %d points: %s\n",
                 AMotionEvent_getAction(event),
                 AMotionEvent_getXOffset(event),
                 AMotionEvent_getYOffset(event),
                 (int)nPoints,
                 MotionEdgeFlagsToString(AMotionEvent_getEdgeFlags(event)).getStr());
        for (size_t i = 0; i < nPoints; i++)
            fprintf(stderr, "\t%d: %g,%g - pressure %g\n",
                    (int)i,
                    AMotionEvent_getX(event, i),
                    AMotionEvent_getY(event, i),
                    AMotionEvent_getPressure(event, i));

        SalMouseEvent aMouseEvent;
        sal_uInt16 nEvent = 0;

        // FIXME: all this filing the nEvent and aMouseEvent has to be cleaned up
        nEvent = AMotionEvent_getAction(event)? SALEVENT_MOUSEBUTTONUP: SALEVENT_MOUSEBUTTONDOWN;

        if (nPoints > 0)
        {
            aMouseEvent.mnX = AMotionEvent_getX(event, 0);
            aMouseEvent.mnY = AMotionEvent_getY(event, 0);
        } else {
            aMouseEvent.mnX = AMotionEvent_getXOffset(event);
            aMouseEvent.mnY = AMotionEvent_getYOffset(event);
        }

        int64_t nNsTime = AMotionEvent_getEventTime(event);
        aMouseEvent.mnTime = nNsTime / (1000 * 1000);
        aMouseEvent.mnCode = 0; // FIXME
        aMouseEvent.mnButton = MOUSE_LEFT; // FIXME

        SalFrame *pFocus = getFocusFrame();
        if (pFocus)
            bHandled = pFocus->CallCallback( nEvent, &aMouseEvent );
        else
            fprintf (stderr, "no focused frame to emit event on\n");

        fprintf( stderr, "bHandled == %s\n", bHandled? "true": "false" );

        break;
    }
    default:
        fprintf (stderr, "unknown input event type %p %d\n",
                 event, AInputEvent_getType(event));
        break;
    }
    return bHandled ? 1 : 0;
}

AndroidSalInstance *AndroidSalInstance::getInstance()
{
    if (!ImplGetSVData())
        return NULL;
    AndroidSalData *pData = static_cast<AndroidSalData *>(ImplGetSVData()->mpSalData);
    if (!pData)
        return NULL;
    return static_cast<AndroidSalInstance *>(pData->m_pInstance);
}

extern "C" {
    void onAppCmd_cb (struct android_app *app, int32_t cmd)
    {
        AndroidSalInstance::getInstance()->onAppCmd(app, cmd);
    }

    int32_t onInputEvent_cb (struct android_app *app, AInputEvent *event)
    {
        return AndroidSalInstance::getInstance()->onInputEvent(app, event);
    }
}

AndroidSalInstance::AndroidSalInstance( SalYieldMutex *pMutex )
    : SvpSalInstance( pMutex )
    , mpApp( NULL )
    , mbQueueReDraw( false )
{
    mpApp = lo_get_app();
    fprintf (stderr, "created Android Sal Instance for app %p window %p thread: %d\n",
             mpApp,
             mpApp ? mpApp->window : NULL,
             (int)pthread_self());
    if (mpApp)
    {
        pthread_mutex_lock (&mpApp->mutex);
        mpApp->onAppCmd = onAppCmd_cb;
        mpApp->onInputEvent = onInputEvent_cb;
        pthread_mutex_unlock (&mpApp->mutex);
    }
}

AndroidSalInstance::~AndroidSalInstance()
{
    fprintf (stderr, "destroyed Android Sal Instance\n");
}

void AndroidSalInstance::Wakeup()
{
    fprintf (stderr, "Wakeup alooper\n");
    if (mpApp && mpApp->looper)
        ALooper_wake (mpApp->looper);
    else
        fprintf (stderr, "busted - no global looper\n");
}

void AndroidSalInstance::DoReleaseYield (int nTimeoutMS)
{
    // Presumably this should never be called at all except in
    // NativeActivity-based apps with a GUI, like android/qa/desktop, where
    // the message pump is run here in vcl?
    if (!mpApp) {
        static bool beenhere = false;
        if (!beenhere)
        {
            fprintf (stderr, "**** Huh, %s called in non-NativeActivity app\n", __FUNCTION__);
            beenhere = true;
        }
        return;
    }

    // release yield mutex
    sal_uLong nAcquireCount = ReleaseYieldMutex();

    fprintf (stderr, "DoReleaseYield #3 %d thread: %d ms\n",
             nTimeoutMS, (int)pthread_self());

    struct android_poll_source *pSource = NULL;
    int outFd = 0, outEvents = 0;

    if (mbQueueReDraw)
        nTimeoutMS = 0;

    int nRet;
    nRet = ALooper_pollAll (nTimeoutMS, &outFd, &outEvents, (void**)&pSource);
    fprintf (stderr, "ret #6 %d %d %d %p\n", nRet, outFd, outEvents, pSource);

    // acquire yield mutex again
    AcquireYieldMutex(nAcquireCount);

    if (nRet >= 0)
    {
        // Process this event.
        if (pSource != NULL)
            pSource->process(mpApp, pSource);
    }

    if (mbQueueReDraw && mpApp && mpApp->window)
        AndroidSalInstance::getInstance()->RedrawWindows (mpApp->window);
}

bool AndroidSalInstance::AnyInput( sal_uInt16 nType )
{
    if( (nType & VCL_INPUT_TIMER) != 0 )
        return CheckTimeout( false );
    // FIXME: ideally we should check our input queue here ...
    fprintf (stderr, "FIXME: AnyInput returns false\n");
    return false;
}

class AndroidSalSystem : public SvpSalSystem {
public:
    AndroidSalSystem() : SvpSalSystem() {}
    virtual ~AndroidSalSystem() {}
    virtual int ShowNativeDialog( const rtl::OUString& rTitle,
                                  const rtl::OUString& rMessage,
                                  const std::list< rtl::OUString >& rButtons,
                                  int nDefButton );
};

SalSystem *AndroidSalInstance::CreateSalSystem()
{
    return new AndroidSalSystem();
}

class AndroidSalFrame : public SvpSalFrame
{
public:
    AndroidSalFrame( AndroidSalInstance *pInstance,
                     SalFrame           *pParent,
                     sal_uLong           nSalFrameStyle,
                     SystemParentData   *pSysParent )
        : SvpSalFrame( pInstance, pParent, nSalFrameStyle,
                       true, basebmp::Format::TWENTYFOUR_BIT_TC_MASK,
                       pSysParent )
    {
        enableDamageTracker();
    }

    virtual void GetWorkArea( Rectangle& rRect )
    {
        AndroidSalInstance::getInstance()->GetWorkArea( rRect );
    }

    virtual void damaged( const basegfx::B2IBox& rDamageRect)
    {
        long long area = rDamageRect.getWidth() * rDamageRect.getHeight();
//        if( area > 32 * 1024 )
        fprintf( stderr, "bitmap damaged  %d %d (%dx%d) area %lld\n",
                 (int) rDamageRect.getMinX(),
                 (int) rDamageRect.getMinY(),
                 (int) rDamageRect.getWidth(),
                 (int) rDamageRect.getHeight(),
                 area );
        if (rDamageRect.getWidth() <= 0 ||
            rDamageRect.getHeight() <= 0)
        {
            fprintf (stderr, "ERROR: damage region has tiny / negative size\n");
            return;
        }
        Rectangle aRect( std::max((long) 0, (long) rDamageRect.getMinX() ),
                         std::max((long) 0, (long) rDamageRect.getMinY() ),
                         std::max((long) 0, (long) ( rDamageRect.getMinX() +
                                                     rDamageRect.getWidth() ) ),
                         std::max((long) 0, (long) ( rDamageRect.getMinY() +
                                                     rDamageRect.getHeight() ) ) );
        AndroidSalInstance::getInstance()->damaged( this, aRect );
    }

    virtual void UpdateSettings( AllSettings &rSettings )
    {
        // Clobber the UI fonts
#if 0
        psp::FastPrintFontInfo aInfo;
        aInfo.m_aFamilyName = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Roboto" ) );
        aInfo.m_eItalic = ITALIC_NORMAL;
        aInfo.m_eWeight = WEIGHT_NORMAL;
        aInfo.m_eWidth = WIDTH_NORMAL;
        psp::PrintFontManager::get().matchFont( aInfo, rSettings.GetUILocale() );
#endif

        // FIXME: is 12 point enough ?
        Font aFont( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Roboto" ) ),
                    Size( 0, 14 ) );

        StyleSettings aStyleSet = rSettings.GetStyleSettings();
        aStyleSet.SetAppFont( aFont );
        aStyleSet.SetHelpFont( aFont );
        aStyleSet.SetMenuFont( aFont );
        aStyleSet.SetToolFont( aFont );
        aStyleSet.SetLabelFont( aFont );
        aStyleSet.SetInfoFont( aFont );
        aStyleSet.SetRadioCheckFont( aFont );
        aStyleSet.SetPushButtonFont( aFont );
        aStyleSet.SetFieldFont( aFont );
        aStyleSet.SetIconFont( aFont );
        aStyleSet.SetGroupFont( aFont );

        rSettings.SetStyleSettings( aStyleSet );
    }
};

SalFrame *AndroidSalInstance::CreateChildFrame( SystemParentData* pParent, sal_uLong nStyle )
{
    return new AndroidSalFrame( this, NULL, nStyle, pParent );
}

SalFrame *AndroidSalInstance::CreateFrame( SalFrame* pParent, sal_uLong nStyle )
{
    return new AndroidSalFrame( this, pParent, nStyle, NULL );
}


// All the interesting stuff is slaved from the AndroidSalInstance
void InitSalData()   {}
void DeInitSalData() {}
void InitSalMain()   {}
void DeInitSalMain() {}

void SalAbort( const rtl::OUString& rErrorText, bool bDumpCore )
{
    rtl::OUString aError( rErrorText );
    if( aError.isEmpty() )
        aError = rtl::OUString::createFromAscii("Unknown application error");
    ::fprintf( stderr, "%s\n", rtl::OUStringToOString(rErrorText, osl_getThreadTextEncoding()).getStr() );

    __android_log_print(ANDROID_LOG_INFO, "LibreOffice", "SalAbort: '%s'",
                        rtl::OUStringToOString(aError, RTL_TEXTENCODING_ASCII_US).getStr());
    if( bDumpCore )
        abort();
    else
        _exit(1);
}

const OUString& SalGetDesktopEnvironment()
{
    static rtl::OUString aEnv( RTL_CONSTASCII_USTRINGPARAM( "android" ) );
    return aEnv;
}

SalData::SalData() :
    m_pInstance( 0 ),
    m_pPlugin( 0 ),
    m_pPIManager(0 )
{
}

SalData::~SalData()
{
}

// This is our main entry point:
SalInstance *CreateSalInstance()
{
    fprintf (stderr, "Android: CreateSalInstance!\n");
    AndroidSalInstance* pInstance = new AndroidSalInstance( new SalYieldMutex() );
    new AndroidSalData( pInstance );
    return pInstance;
}

void DestroySalInstance( SalInstance *pInst )
{
    pInst->ReleaseYieldMutex();
    delete pInst;
}

#include <vcl/msgbox.hxx>

int AndroidSalSystem::ShowNativeDialog( const rtl::OUString& rTitle,
                                        const rtl::OUString& rMessage,
                                        const std::list< rtl::OUString >& rButtons,
                                        int nDefButton )
{
    (void)rButtons; (void)nDefButton;
    fprintf (stderr, "LibreOffice native dialog '%s': '%s'\n",
             rtl::OUStringToOString(rTitle, RTL_TEXTENCODING_ASCII_US).getStr(),
             rtl::OUStringToOString(rMessage, RTL_TEXTENCODING_ASCII_US).getStr());
    __android_log_print(ANDROID_LOG_INFO, "LibreOffice", "Dialog '%s': '%s'",
                        rtl::OUStringToOString(rTitle, RTL_TEXTENCODING_ASCII_US).getStr(),
                        rtl::OUStringToOString(rMessage, RTL_TEXTENCODING_ASCII_US).getStr());

    if (AndroidSalInstance::getInstance() != NULL)
    {
        // Does Android have a native dialog ? if not,. we have to do this ...

        // Of course it has. android.app.AlertDialog seems like a good
        // choice, it even has one, two or three buttons. Naturally,
        // it intended to be used from Java, so some verbose JNI
        // horror would be needed to use it directly here. Probably we
        // want some easier to use magic wrapper, hmm.

        ErrorBox aVclErrBox( NULL, WB_OK, rTitle );
        aVclErrBox.SetText( rMessage );
        aVclErrBox.Execute();
    }
    else
        fprintf (stderr, "VCL not initialized\n");
    return 0;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
