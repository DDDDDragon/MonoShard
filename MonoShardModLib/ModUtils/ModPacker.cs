using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.CodeAnalysis.Emit;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace MonoShardModLib.ModUtils
{
    public class ModPacker
    {
        public static void Pack(string path)
        {
            DirectoryInfo dir = new(path);
            FileInfo[] textures = dir.GetFiles("*.png", SearchOption.AllDirectories);

            int offset = 0;

            FileStream fs = new(Path.Combine(ModLoader.ModPath, dir.Name + ".sml"), FileMode.Create);

            Write(fs, "MSLM");

            Write(fs, textures.Length);
            foreach (FileInfo tex in textures)
            {
                string name = dir.Name + tex.FullName.Replace(path, "");
                Write(fs, name.Length);
                Write(fs, name);
                Write(fs, offset);
            }
            foreach (FileInfo tex in textures)
                Write(fs, tex);

            bool success = CompileMod(dir.Name, path, out byte[] code, out _);

            if(!success)
            {
                fs.Close();
                File.Delete(fs.Name);
                return;
            }

            Write(fs, code.Length);
            Write(fs, code);
            fs.Close();

            Console.WriteLine($"Successfully pack {dir.Name}.");
        }

        public static void Write(FileStream fs, object obj)
        {
            byte[] bytes;
            switch (obj)
            {
                case int num:
                    bytes = BitConverter.GetBytes(num);
                    fs.Write(bytes, 0, bytes.Length);
                    break;
                case string str:
                    bytes = Encoding.UTF8.GetBytes(str);
                    fs.Write(bytes, 0, bytes.Length);
                    break;
                case FileInfo info:
                    FileStream stream = new(info.FullName, FileMode.Open);
                    bytes = new byte[stream.Length];
                    stream.Read(bytes, 0, bytes.Length);
                    fs.Write(bytes, 0, bytes.Length);
                    stream.Close();
                    break;
                case byte[] data:
                    fs.Write(data, 0, data.Length);
                    break;
                default:
                    break;
            }
        }

        public static Diagnostic[] RoslynCompile(string name, IEnumerable<string> files, IEnumerable<string> preprocessorSymbols, out byte[] code, out byte[] pdb)
        {
            IEnumerable<string> DefaultNamespaces = new[] { "System.Collections.Generic" };

            CSharpCompilationOptions options = new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary, checkOverflow: true, optimizationLevel: OptimizationLevel.Release, allowUnsafe: false)
                .WithUsings(DefaultNamespaces);

            CSharpParseOptions parseOptions = new(LanguageVersion.Preview, preprocessorSymbols: preprocessorSymbols);

            EmitOptions emitOptions = new(debugInformationFormat: DebugInformationFormat.PortablePdb);

            string[] dlls = Directory.GetFiles(Path.Combine(ModLoader.GenPath, "mono\\lib\\4.5"), "*.dll");

            List<MetadataReference> defaultReferences = dlls.ToList()
                .ConvertAll(
                    new Converter<string, MetadataReference>(delegate(string str) { return MetadataReference.CreateFromFile(str); })
                );

            defaultReferences.AddRange(new List<MetadataReference>() { MetadataReference.CreateFromFile(typeof(object).Assembly.Location) });

            IEnumerable<SyntaxTree> src = files.Select(f => SyntaxFactory.ParseSyntaxTree(File.ReadAllText(f), parseOptions, f, Encoding.UTF8));

            CSharpCompilation comp = CSharpCompilation.Create(name, src, defaultReferences, options);

            using MemoryStream peStream = new();
            using MemoryStream pdbStream = new();

            EmitResult results = comp.Emit(peStream, pdbStream, options: emitOptions);

            code = peStream.ToArray();
            pdb = pdbStream.ToArray();

            return results.Diagnostics.ToArray();
        }

        public static bool CompileMod(string name, string path, out byte[] code, out byte[] pdb)
        {
            IEnumerable<string> files = Directory
                .GetFiles(path, "*.cs", SearchOption.AllDirectories)
                .Where(file => !IgnoreCompletely(name, file));

            Diagnostic[] result = RoslynCompile(name, files, [], out code, out pdb);

            if (Array.Exists(result, e => e.Severity == DiagnosticSeverity.Error))
                return false;
            return true;
        }

        public static bool IgnoreCompletely(string name, string file)
        {
            string relPath = file.Substring(file.IndexOf("ModSources")).Replace("ModSources\\" + name + "\\", "");
            return relPath[0] == '.' ||
                relPath.StartsWith("bin" + Path.DirectorySeparatorChar) ||
                relPath.StartsWith("obj" + Path.DirectorySeparatorChar);
        }
    }
}

