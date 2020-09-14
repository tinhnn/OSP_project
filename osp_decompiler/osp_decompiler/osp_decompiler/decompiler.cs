using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using RestSharp;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Net;
using System.Text;

namespace osp_decompiler
{
    public class decompiler
    {
        private osp_utility _util;

        public decompiler()
        {
            _util = new osp_utility();
        }

        public string decompile(string inputfile, string IpPort)
        {
            string tempFolder = Directory.GetCurrentDirectory() + "\\temp";
            string output = "[";
            int index = 0;
            List<ODB_format> List_OdbMap = new List<ODB_format> { };

            if (Directory.Exists(tempFolder)) Directory.Delete(tempFolder, true);
            Directory.CreateDirectory(tempFolder);
            ZipFile.ExtractToDirectory("addon.zip", tempFolder);

            IPAddress address;
            bool isIp = IPAddress.TryParse(inputfile, out address);
            if (isIp) {
                _util.ipAdd = inputfile;
                _util.ipPort = IpPort;
                IRestResponse modules = _util.getModules();
                if (modules.Content == "") return "[]";
                var json_modules = JToken.Parse(modules.Content);
                foreach (dynamic json_module in json_modules)
                {
                    index++;
                    ODB_format odb_map = new ODB_format();
                    IRestResponse code_trunk = _util.getCodeTrunk(json_module.id.ToString());
                    string data = _util.ExecutiveCodeTrunk(
                        json_module.module.ToString(),
                        code_trunk.RawBytes);
                    if (data != "")
                    {
                        output += data + ",";
                    }
                }
                output = output.Remove(output.Length - 1);
            }
            else {  /* *.dbc */
                if(!File.Exists(inputfile))
                {
                    Console.WriteLine("Not existed input file.");
                    return null;
                }
                string dbcPath = Directory.GetCurrentDirectory() + "\\dbc";
                if (Directory.Exists(dbcPath)) Directory.Delete(dbcPath, true);
                Directory.CreateDirectory(dbcPath);
                ZipFile.ExtractToDirectory(inputfile, dbcPath);
                string[] list_avmc = Directory.GetFiles(dbcPath);
                foreach(var avmcfile in list_avmc)
                {
                    index++;
                    string data = _util.ExecutiveAvmcFile(avmcfile);
                    if (data != "")
                    {
                        output += data + ",";
                    }
                }
                output = output.Remove(output.Length - 1);
            }
            output += "\n]";
            return output;
        }
    }
}
