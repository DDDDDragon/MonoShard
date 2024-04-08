using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

namespace MonoShardModLib.ModUtils
{
    public class ModFileChunk
    {
        public string? Name { get; set; }

        public int Offset { get; set; }

        public int Size { get; set; }
    }

    public class ModFiles
    {
        public ModFiles()
        {
            Files = new();
            Icon = Array.Empty<byte>();
        }

        public Assembly? Assembly { get; set; }

        public int FileOffset { get; set; }

        public List<ModFileChunk>? Files { get; set; }

        public byte[] Icon { get; set; }

        public byte[] GetFile(string fileName)
        {
            return new byte[] { };
        }
    }
}
