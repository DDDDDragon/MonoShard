﻿using System;
using System.Runtime.CompilerServices;

namespace MonoShardModLib
{
    internal class Core
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static string GetGenPath();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static string TestFunc(string val);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static float GetInstancePropertyAsInteger(string name, int self, int other);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetInstancePropertyAsInteger(string name, float value, int self, int other);
    }
}