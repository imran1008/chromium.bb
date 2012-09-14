// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/app/content_main.h"

#include "content/shell/shell_main_delegate.h"
#include "sandbox/src/sandbox_types.h"
#include "sstream"

namespace WebCore {
    struct LayoutTimeStamp;
    std::vector<LayoutTimeStamp*> *g_layoutTimeStamp = NULL;
    void (*g_startLayoutDebugFunc)(void) = NULL;
    void (*g_endLayoutDebugFunc)(void) = NULL;
    extern void printLayoutTimeStamp(std::wostream&, LayoutTimeStamp*);
    extern void deleteLayoutTimeStamp(LayoutTimeStamp*);
}

void endLayoutDebug() {
    if (!WebCore::g_layoutTimeStamp || WebCore::g_layoutTimeStamp->empty()) {
        return;
    }

    // print out the result
    std::wstringstream ss;

    for(int i = 0, len = WebCore::g_layoutTimeStamp->size() ; i < len; i++) {
        WebCore::LayoutTimeStamp *item = WebCore::g_layoutTimeStamp->at(i);
        ss.str(L"");
        WebCore::printLayoutTimeStamp(ss, item);
        WebCore::deleteLayoutTimeStamp(item);
        OutputDebugStringW(ss.str().c_str());
        OutputDebugStringW(L"\n");
    }

    // WebCore::g_layoutTimeStamp->clear();
    delete WebCore::g_layoutTimeStamp;
    WebCore::g_layoutTimeStamp = NULL;
}

void startLayoutDebug() {
    if (WebCore::g_layoutTimeStamp) {
        endLayoutDebug();
    }
    WebCore::g_layoutTimeStamp = new std::vector<WebCore::LayoutTimeStamp*>();
}

#if defined(OS_WIN)
#include "content/public/app/startup_helper_win.h"
#endif

#if defined(OS_MACOSX)
#include "content/shell/shell_content_main.h"
#endif

#if defined(OS_WIN)

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, wchar_t*, int) {
  sandbox::SandboxInterfaceInfo sandbox_info = {0};
  content::InitializeSandboxInfo(&sandbox_info);
  ShellMainDelegate delegate;
  return content::ContentMain(instance, &sandbox_info, &delegate);
}

#else

int main(int argc, const char** argv) {
#if defined(OS_MACOSX)
  // Do the delegate work in shell_content_main to avoid having to export the
  // delegate types.
  return ::ContentMain(argc, argv);
#else
  ShellMainDelegate delegate;
  return content::ContentMain(argc, argv, &delegate);
#endif  // OS_MACOSX
}

#endif  // OS_POSIX
