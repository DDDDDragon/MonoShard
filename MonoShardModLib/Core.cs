using System;
using System.Runtime.CompilerServices;

namespace MonoShardModLib
{
    internal class Core
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string GetGenPath();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string TestFunc(string val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float GetInstancePropertyAsInteger(string name, int self, int other);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void SetInstancePropertyAsInteger(string name, float value, int self, int other);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string GetInstancePropertyAsString(string name, int self, int other);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void SetInstancePropertyAsString(string name, string value, int self, int other);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float GetValueFromMapAsInteger(string name, int map, int self, int other);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void SetValueInMapAsInteger(string name, int map, float value, int self, int other);
    }
}
