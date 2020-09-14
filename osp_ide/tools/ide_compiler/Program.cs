using CommandLine;
using System;
using System.IO;

namespace ide_compiler
{
    class Options
    {
        [Option(
            'c', "compile", Required = true,
            HelpText = "Compile specification file.")]
        public string CompileFile { get; set; }

        [Option(
            'm', "mode", Required = true,
            HelpText = "Mode compile: 0 - oneshoot; 1 - periodic.")]
        public string ModeCompile { get; set; }

        [Option(
            'p', "map_odb", Required = true,
            HelpText = "Map odb.")]
        public string MapOdb { get; set; }

        [Option(
            'o', "output",
            HelpText = "Place the output into file.")]
        public string OutputFile { get; set; }
    }

    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                var options = new Options();
                var isValid = Parser.Default.ParseArgumentsStrict(args, options);
                if (isValid == false) throw new Exception("bad options");
                if (options.OutputFile == null)
                {
                    options.OutputFile = Path.ChangeExtension(
                        options.CompileFile, "aml");
                }
                using (var input = File.OpenText(options.CompileFile))
                {
                    using (var mapOdb = File.OpenText(options.MapOdb))
                    {
                        using (var output = File.CreateText(options.OutputFile))
                        {
                            switch(options.ModeCompile)
                            {
                                case "0":
                                    Compiler.Compile(Compiler.TypeCompile.OneShoot, input, output, mapOdb);
                                    break;
                                case "1":
                                    Compiler.Compile(Compiler.TypeCompile.Periodic, input, output, mapOdb);
                                    break;
                                default:
                                    break;
                            }
                        }
                        Console.WriteLine(" -> {0}", options.OutputFile);
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }
}
