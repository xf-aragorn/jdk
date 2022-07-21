/*
 * Copyright (c) 2022, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#ifndef TEST_LIB_NATIVE_THREAD_H
#define TEST_LIB_NATIVE_THREAD_H

// Header only library for using threads in tests

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32

#include <windows.h>
typedef HANDLE THREAD_ID;

#else // !_WIN32

#include <unistd.h>
#include <pthread.h>
typedef pthread_t THREAD_ID;
#endif // !_WIN32

extern "C" {

typedef void(*PROCEDURE)(void*);

struct Helper {
    PROCEDURE proc;
    void* context;
};

#ifdef _WIN32
static DWORD __stdcall procedure(void* ctxt) {
#else // !windows
void* procedure(void* ctxt) {
#endif
    Helper* helper = (Helper*)ctxt;
    helper->proc(helper->context);
    return 0;
}

void run_async(PROCEDURE proc, void* context) {
    struct Helper helper;
    helper.proc = proc;
    helper.context = context;
#ifdef _WIN32
    HANDLE thread = CreateThread(NULL, 0, procedure, &helper, 0, NULL);
    if (thread == NULL) {
        perror("failed to create thread");
    }
    if (WaitForSingleObject(thread, INFINITE) != WAIT_OBJECT_0) {
        perror("failed to join thread");
    }
#else
    pthread_t thread;
    int result = pthread_create(&thread, NULL, procedure, &helper);
    if (result != 0) {
        perror("failed to create thread");
    }
    if (pthread_join(thread, NULL) != 0) {
        perror("failed to join thread");
    }
#endif
}

}

#endif // TEST_LIB_NATIVE_THREAD_H
