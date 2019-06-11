﻿// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using TestLibrary;

namespace FixupCallsHostWhenLoaded
{
    class FixupCallsHostWhenLoaded
    {
        static int Main(string[] args)
        {
            if(Environment.OSVersion.Platform != PlatformID.Win32NT)
            {
                return 100;
            }

            try
            {
                IntPtr ijwHost = NativeLibrary.Load(Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "ijwhost.dll"));

                WasModuleVTableQueriedDelegate wasModuleVTableQueried = Marshal.GetDelegateForFunctionPointer<WasModuleVTableQueriedDelegate>(NativeLibrary.GetExport(ijwHost, "WasModuleVTableQueried"));

                // Load IJW via reflection
                Assembly.Load("IjwNativeDll");

                IntPtr ijwModuleHandle = GetModuleHandle("IjwNativeDll.dll");
                
                Assert.AreNotEqual(IntPtr.Zero, ijwModuleHandle);
                Assert.IsTrue(wasModuleVTableQueried(ijwModuleHandle));
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return 101;
            }

            return 100;
        }


        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        delegate bool WasModuleVTableQueriedDelegate(IntPtr handle);

        [DllImport("kernel32.dll")]
        static extern IntPtr GetModuleHandle(string lpModuleName);
    }
}
