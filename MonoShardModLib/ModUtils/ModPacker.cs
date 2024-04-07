using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.CodeAnalysis.Emit;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Net.Mime.MediaTypeNames;

namespace MonoShardModLib.ModUtils
{
    public class ModPacker
    {
        public static void Pack(string path)
        {
            DirectoryInfo dir = new(path);
            FileInfo[] textures = dir.GetFiles("*.png", SearchOption.AllDirectories);

            Console.WriteLine(path);

            int offset = 0;

            FileStream fs = new(Path.Join(ModLoader.ModPath, dir.Name + ".sml"), FileMode.Create);

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
        }

        public static void Write(FileStream fs, object obj)
        {
            Type type = obj.GetType();
            if (type == typeof(int))
            {
                byte[]? bytes = BitConverter.GetBytes((int)obj);
                fs.Write(bytes, 0, bytes.Length);
            }
            else if (type == typeof(string))
            {
                byte[]? bytes = Encoding.UTF8.GetBytes((string)obj);
                fs.Write(bytes, 0, bytes.Length);
            }
            else if (type == typeof(FileInfo))
            {
                FileStream stream = new(((FileInfo)obj).FullName, FileMode.Open);
                byte[]? bytes = new byte[stream.Length];
                stream.Read(bytes, 0, bytes.Length);
                fs.Write(bytes, 0, bytes.Length);
                stream.Close();
            }
            else if (type == typeof(byte[]))
            {
                fs.Write((byte[])obj);
            }
        }

        public static Diagnostic[] RoslynCompile(string name, IEnumerable<string> files, IEnumerable<string> preprocessorSymbols, out byte[] code, out byte[] pdb)
        {
            IEnumerable<string> DefaultNamespaces = new[] { "System.Collections.Generic" };

            CSharpCompilationOptions options = new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary, checkOverflow: true, optimizationLevel: OptimizationLevel.Release, allowUnsafe: false)
                .WithUsings(DefaultNamespaces);

            CSharpParseOptions parseOptions = new(LanguageVersion.Preview, preprocessorSymbols: preprocessorSymbols);

            EmitOptions emitOptions = new(debugInformationFormat: DebugInformationFormat.PortablePdb);

            string[] dlls = Directory.GetFiles(Environment.CurrentDirectory, "*.dll");

            List<MetadataReference> defaultReferences = dlls.ToList()
                .ConvertAll(
                    new Converter<string, MetadataReference>(delegate(string str) { return MetadataReference.CreateFromFile(str); })
                );

            defaultReferences.AddRange(new List<MetadataReference>() { MetadataReference.CreateFromFile(typeof(object).Assembly.Location) });

            IEnumerable<SyntaxTree> src = files.Select(f => SyntaxFactory.ParseSyntaxTree(File.ReadAllText(f), parseOptions, f, Encoding.UTF8));

            CSharpCompilation comp = CSharpCompilation.Create(name, src, defaultReferences, options);

            foreach (MetadataReference usedAssemblyReferences in comp.GetUsedAssemblyReferences())
            {
                if (usedAssemblyReferences.Display != null)
                {
                    FileVersionInfo fileVersionInfo = FileVersionInfo.GetVersionInfo(usedAssemblyReferences.Display);
                }
                else
                {

                }

            }

            foreach (SyntaxTree tree in comp.SyntaxTrees)
            {
                // get the semantic model for this tree
                SemanticModel model = comp.GetSemanticModel(tree);

                // find everywhere in the AST that refers to a type
                SyntaxNode root = tree.GetRoot();
                IEnumerable<TypeSyntax> allTypeNames = root.DescendantNodesAndSelf().OfType<TypeSyntax>();

                foreach (TypeSyntax typeName in allTypeNames)
                {
                    // what does roslyn think the type _name_ actually refers to?
                    Microsoft.CodeAnalysis.TypeInfo effectiveType = model.GetTypeInfo(typeName);
                    if (effectiveType.Type != null && effectiveType.Type.TypeKind == TypeKind.Error)
                    {
                        // if it's an error type (ie. couldn't be resolved), cast and proceed
                    }
                }
            }

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

            Diagnostic[] result = RoslynCompile(name, files, new[] { "FNA" }, out code, out pdb);

            foreach (Diagnostic err in result.Where(e => e.Severity == DiagnosticSeverity.Error))
            {

            }
            foreach (Diagnostic warning in result.Where(e => e.Severity == DiagnosticSeverity.Warning))
            {

            }
            foreach (Diagnostic info in result.Where(e => e.Severity == DiagnosticSeverity.Info))
            {

            }

            if (Array.Exists(result, e => e.Severity == DiagnosticSeverity.Error))
            {
                return false;
            }
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

