using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using static System.Net.WebRequestMethods;

namespace MonoShardModLib.ModUtils
{
    public class ModReader
    {
        public static FileStream? fs {  get; set; }

        public static Mod? Read(string path)
        {
            fs = new(path, FileMode.Open);

            string modName = fs.Name.Split('\\').Last().Replace(".sml", "");

            if (Encoding.UTF8.GetString(Read(fs, 4)) != "MSLM") Error();
            
            ModFiles files = new();

            //read textures
            int count = BitConverter.ToInt32(Read(fs, 4), 0);
            for (int i = 0; i < count; i++)
            {
                int fileSize = BitConverter.ToInt32(Read(fs, 4), 0);

                ModFileChunk chunk = new()
                {
                    Name = Encoding.UTF8.GetString(Read(fs, fileSize)),
                    Offset = BitConverter.ToInt32(Read(fs, 4), 0),
                    Size = BitConverter.ToInt32(Read(fs, 4), 0)
                };

                files.Files?.Add(chunk);
            }

            files.FileOffset = (int)fs.Position;

            if (files.Files?.Count > 0)
            {
                ModFileChunk? f = files.Files.Last();

                Read(fs, (int)(f?.Offset + f?.Size));
            }

            int size = BitConverter.ToInt32(Read(fs, 4), 0);
            files.Assembly = Assembly.Load(Read(fs, size));

            try {
                files.Icon = files.GetFile($"{modName}\\icon.png");
            } catch (Exception) { }

            fs.Close();

            try
            {
                Type? modType = Array.Find(files.Assembly.GetTypes(), t => t.IsSubclassOf(typeof(Mod)));

                if (modType == null)
                {
                    Console.WriteLine($"{files.Assembly.GetName().Name} need a subclass of Mod.");
                    return null;
                }
                else
                {
                    if (Activator.CreateInstance(modType) is not Mod mod) return null;
                    mod.ModFiles = files;
                    return mod;
                }
            }
            catch
            {
                throw; 
            }
        }

        public static void Error()
        {
            fs?.Close();
            throw new Exception();
        }            

        public static byte[] Read(FileStream fs, int length)
        {
            byte[] bytes = new byte[length];
            if (fs.Length - fs.Position < length)
            {
                fs.Close();
                throw new Exception(string.Format("In FileReader.Read cannot read {0} bytes in the mod {1} ", length, fs.Name.Split('\\').Last()));
            }
            fs.Read(bytes, 0, length);
            return bytes;
        }
    }
}
