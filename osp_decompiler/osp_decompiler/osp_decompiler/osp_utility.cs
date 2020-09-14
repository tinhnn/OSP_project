using Newtonsoft.Json.Linq;
using RestSharp;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Compression;

namespace osp_decompiler
{
    public class osp_utility
    {
        public string ipAdd = "";  // local IP: 127.0.0.1
        public string ipPort = "";  // local Port: 9222
        public IRestResponse getModules()
        {
            RestClient client = new RestClient("http://" + ipAdd + ":" + ipPort + "/modules");
            RestRequest request = new RestRequest(Method.GET);
            return (client.Execute(request));
        }

        public IRestResponse getCodeTrunk(string id)
        {
            RestClient client = new RestClient("http://" + ipAdd + ":" + ipPort + "/modules?id=" + id);
            RestRequest request = new RestRequest(Method.GET);
            return (client.Execute(request));
        }

        private static string _strOutput = "";
        private static bool _isupdate = false;

        public string ExecutiveAvmcFile(string module_Path)
        {
            _strOutput = "";
            _isupdate = false;

            string tempFolder = Directory.GetCurrentDirectory() + "\\temp\\";

            using (StreamReader reader = new StreamReader(tempFolder + "pulldata.aml"))
            {
                string input = reader.ReadToEnd();
                reader.Close();
                using (StreamWriter writer = new StreamWriter(tempFolder + "pull_data.aml", false))
                {
                    string output = input.Replace("Module_Name", Path.GetFileNameWithoutExtension(module_Path));
                    writer.Write(output);
                    writer.Close();
                }
            }
            /* compiler get_data */
            ExecuteCommand(tempFolder + "amlc.exe -c " + tempFolder + "pull_data.aml");
            ExecuteCommand("move pull_data.avmc temp");

            Process process = new Process();
            process.EnableRaisingEvents = true;
            process.OutputDataReceived += new DataReceivedEventHandler(OnOutputDataReceived);
            process.ErrorDataReceived += new DataReceivedEventHandler(ErrorDataReceived);

            process.StartInfo.FileName = tempFolder + "amlc.exe";
            process.StartInfo.Arguments =
                                "-e pull_data:main/0 " +
                                tempFolder + "pull_data.avmc " +
                                module_Path;
            process.StartInfo.UseShellExecute = false;
            process.StartInfo.RedirectStandardOutput = true;
            process.StartInfo.RedirectStandardError = true;

            process.Start();
            process.BeginErrorReadLine();
            process.BeginOutputReadLine();

            process.WaitForExit();
            process.Close();
            return _strOutput;
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name="module_name"></param>
        /// <param name="code_trunk"></param>
        /// <param name="odb_map"></param>
        /// <returns></returns>
        public string ExecutiveCodeTrunk(string module_name, byte[] code_trunk)
        {
            _strOutput = "";
            _isupdate = false;

            string tempFolder = Directory.GetCurrentDirectory() + "\\temp\\";

            using (StreamReader reader = new StreamReader(tempFolder + "pulldata.aml"))
            {
                string input = reader.ReadToEnd();
                reader.Close();
                using (StreamWriter writer = new StreamWriter(tempFolder + "pull_data.aml", false))
                {
                    string output = input.Replace("Module_Name", module_name);
                    writer.Write(output);
                    writer.Close();
                }
            }
            /* compiler get_data */
            ExecuteCommand(tempFolder + "amlc.exe -c " + tempFolder + "pull_data.aml");
            ExecuteCommand("move pull_data.avmc temp");

            BinaryWriter _writer = new BinaryWriter(File.Open(tempFolder + module_name + ".avmc", FileMode.Create));
            _writer.Write(code_trunk);
            _writer.Close();

            Process process = new Process();
            process.EnableRaisingEvents = true;
            process.OutputDataReceived += new DataReceivedEventHandler(OnOutputDataReceived);
            process.ErrorDataReceived += new DataReceivedEventHandler(ErrorDataReceived);

            process.StartInfo.FileName = tempFolder + "amlc.exe";
            process.StartInfo.Arguments =
                                "-e pull_data:main/0 " +
                                tempFolder + "pull_data.avmc " +
                                tempFolder + module_name + ".avmc";
            process.StartInfo.UseShellExecute = false;
            process.StartInfo.RedirectStandardOutput = true;
            process.StartInfo.RedirectStandardError = true;

            process.Start();
            process.BeginErrorReadLine();
            process.BeginOutputReadLine();

            process.WaitForExit();
            process.Close();
            return _strOutput;
        }
        static void OnOutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            if (e.Data != null)
            {
                if((e.Data[0] == '{')&&(_isupdate == false))
                {
                    _isupdate = true;
                    _strOutput += "\n" + e.Data;
                }
                else if(_isupdate == true)
                {
                    _strOutput += "\n" + e.Data;
                }
            }

        }

        static void ErrorDataReceived(object sender, DataReceivedEventArgs e)
        {
            Console.WriteLine(e.Data + "\n");
        }
        public void ExecuteCommand(string command)
        {
            int ExitCode;
            ProcessStartInfo ProcessInfo;
            Process Process;

            ProcessInfo = new ProcessStartInfo("cmd.exe", "/c " + command);
            ProcessInfo.CreateNoWindow = true;
            ProcessInfo.UseShellExecute = false;

            Process = Process.Start(ProcessInfo);
            Process.WaitForExit();

            ExitCode = Process.ExitCode;
            Process.Close();
        }
    }
}
