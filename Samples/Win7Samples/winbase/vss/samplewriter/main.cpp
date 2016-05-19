/*
    Copyright (c) 2009 Microsoft Corporation

    Module Name:
        main.cpp

This is an implementation of a sample Writer. It should be used for reference
when building your own Writers. Please note that this Writer should not be used
in a production environment. It aims to demonstrates certain concepts that are
already documented on MSDN, making it even easier to understand them.

It is important to understand that Writers should only cover files that are
exclusively handled by the application Writer is being developed for. Since
this sample has no corresponding application, it manages common files that
can be found on the boxes: text documents, spreadsheets, images.These files
are being generated by many different applications and that may or may not be
handled by some real Writer in the future. Since Writers should only work
with files they handle exclusively, scenarios demonstrated are purely for
testing purposes.

For data safety this Writer does not alter any files on the disk. However
it is strongly encouraged to run it in a sandbox, on a designated test machine.

Files that are being backed up by this writer are *.doc, *.docx, *.xls, *.xlsx,
*.jpe, *.jpg, *.jpeg, *png, *.gif and *.bmp. Writer will search for those files
on the machine in profile directories for every existing user.

This Writer can be exercised using BeTest tool included in the Windows SDK.

*/


#include "stdafx.h"
#include "swriter.h"


HANDLE g_quitEvent = NULL;


BOOL WINAPI ConsoleControlHandler(DWORD dwParam)
{
    UNREFERENCED_PARAMETER(dwParam);

    // We want to quit regardless of what the control event was
    ::SetEvent(g_quitEvent);

    return TRUE;
}


int __cdecl wmain(int argc, WCHAR *argv[])
{
    HRESULT hr      = S_OK;
    int     iResult = 1;

    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        wprintf(L"(!) CoInitializeEx failed");
        return 1;
    }

    hr = ::CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_IMP_LEVEL_IDENTIFY,
        NULL,
        EOAC_NONE,
        NULL);

    if (FAILED(hr))
    {
        wprintf(L"(!) CoInitializeSecurity failed");
        goto _exit;
    }

    g_quitEvent = ::CreateEventW(NULL, TRUE, FALSE, NULL);
    if (g_quitEvent == NULL)
    {
        wprintf(L"(!) CreateEventW failed");
        goto _exit;
    }

    // Set a control handler that allows the writer to be shut down with Ctrl+C
    if (!::SetConsoleCtrlHandler(ConsoleControlHandler, TRUE))
    {
        wprintf(L"(!) SetConsoleSecurityHandler failed");
        goto _exit;
    }

    // We want the writer to go out of scope before the return statement
    {
        SampleWriter writer;
        hr = writer.Initialize();

        if (FAILED(hr))
        {
            wprintf(L"(!) SampleWriter::Initialize failed");
            goto _exit;
        }


        if (::WaitForSingleObject(g_quitEvent, INFINITE) != WAIT_OBJECT_0)
        {
            wprintf(L"(!) WaitForSingleObject failed");
            goto _exit;
        }
        writer.Uninitialize();
    }

    iResult = 0;

_exit:
    return iResult;
}


