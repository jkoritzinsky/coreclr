// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using TestLibrary;

namespace NativeCallingManaged
{
    class NativeCallingManaged
    {
        static int Main(string[] args)
        {
            // Disable running on Windows 7 until IJW activation work is complete.
            if(Environment.OSVersion.Platform != PlatformID.Win32NT || TestLibrary.Utilities.IsWindows7)
            {
                return 100;
            }

            bool success = true;
            TestFramework.BeginScenario("Calling from managed to native IJW code");

            // Building with a reference to the IJW dll is difficult, so load via reflection instead
            TestFramework.BeginTestCase("Load IJW dll via reflection");
            Assembly ijwNativeDll = Assembly.Load("IjwNativeCallingManagedDll");
            TestFramework.EndTestCase();

            TestFramework.BeginTestCase("Call native method returning int");
            Type testType = ijwNativeDll.GetType("TestClass");
            object testInstance = Activator.CreateInstance(testType);
            MethodInfo testMethod = testType.GetMethod("ManagedEntryPoint");
            int result = (int)testMethod.Invoke(testInstance, null);
            if(result != 100)
            {
                TestFramework.LogError("IJW", "Incorrect result returned: " + result);
                success = false;
            }
            TestFramework.EndTestCase();

            return success ? 100 : 99;
        }
    }
}
