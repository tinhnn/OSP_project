using CommandLine;
using System;
using System.IO;

namespace osp.dbc
{
    class Options
    {
        [Option(
            'c', "compile", Required = true,
            HelpText = "Compile specification file.")]
        public string CompileFile { get; set; }

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
                using (var input = File.OpenText(options.CompileFile))
                {
                    {
                        Compiler.Compile(input, null);
                    }
                    using (var output = File.CreateText(Directory.GetCurrentDirectory() + "\\can_db.aml"))
                    {
                        var source = new can_db();
                        output.Write(source.TransformText());
                        Console.WriteLine(" -> {0}", ".\\can_db.aml");
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
