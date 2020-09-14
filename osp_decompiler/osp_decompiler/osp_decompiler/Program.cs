using System;
using System.IO;
using CommandLine;

namespace osp_decompiler
{
    class Options
    {
        [Option(
            'd', "decompile", Required = true,
            HelpText = "Decompile input file.")]
        public string InputFile { get; set; }
        [Option(
            'p', "decompile",
            HelpText = "Ip port.")]
        public string IpPort { get; set; }
        [Option(
            'o', "Output", Required = true,
            HelpText = "Json output file.")]
        public string OutputFile { get; set; }

    }

    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                var options = new Options();
                string data_output = "";
                var isValid = Parser.Default.ParseArgumentsStrict(args, options);
                if (isValid == false) throw new Exception("bad options");

                decompiler decompiler = new decompiler();
                StreamWriter writer;
                if (options.OutputFile == "") {
                    writer = new StreamWriter("output.json");
                }
                else
                {
                    writer = new StreamWriter(options.OutputFile);
                }
                data_output = decompiler.decompile(options.InputFile, options.IpPort);
                writer.WriteLine(data_output);
                writer.Close();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
    }
}
