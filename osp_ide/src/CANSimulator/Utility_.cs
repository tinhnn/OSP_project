using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Windows;
using static WpfApplication3.SendCmd_;
using RestSharp;
using System.Diagnostics;
using Configuration;

namespace WpfApplication3
{
    class Utility_
    {
        public string LoadJson_()
        {
            using (StreamReader r = new StreamReader("command_data.json"))
            {
                string json = r.ReadToEnd();
                return json;
            }
        }

        public ViewData[] ParsingJsonDataToView_(CommandJson_[] arrCmdJ)
        {
            ViewData[] arrview = new ViewData[] { };

            List<ViewData> listView = new List<ViewData>();
            List<string> listTriger = new List<string>();
            listTriger.Add("Manual");
            listTriger.Add("Preodic: __ms");
            List<string> listType = new List<string>();
            listType.Add("CAN Data");
            listType.Add("CAN FD");
            List<string> listChanel = new List<string>();
            string[] chanelArray = ConfigurationManager.AppSettings["canChanel"].Split(',');
            for(int i = 0; i < chanelArray.Length; i++)
            {
                listChanel.Add(chanelArray[i]);
            }
            

            for (var i = 0; i < arrCmdJ.Length; i++)
            {
                ViewData view = new ViewData
                {
                    row = i,
                    isSellectedCommand = false,      // init value
                    sendSts = false,
                    properties = new ViewCommandProperties
                    {
                        dlc = arrCmdJ[i].dlc,
                        id = arrCmdJ[i].id,
                        name = arrCmdJ[i].name,
                        ide = false,
                        brs = false,
                        btr = false,
                        trigerList = listTriger,
                        trigerSelectedIdx = 0,
                        chanelList = listChanel,
                        chanelSelectedIdx = 0,
                        typeList = listType,
                        typeSelectedIdx = 0,
                        isCycle = false,
                        cycle = 0,
                        sentStatus = "SEND",
                        isCreating = false
                    }
                };
                listView.Add(view);
            }

            arrview = listView.ToArray();

            // init binding signal
            for (var j = 0; j < arrCmdJ.Length; j++)
            {
                List<ViewSignalProperties> listSig = new List<ViewSignalProperties>();
                int cnt = arrCmdJ[j].signals.Length;
                for (int s = 0; s < cnt; s++)
                {
                    // mapping data for all command
                    List<int> rawValueList = new List<int>();
                    List<string> physValueList = new List<string>();
                    int sIndex = 0;

                    if(String.Equals("enum", arrCmdJ[j].signals[s].value_type, StringComparison.CurrentCulture))
                    {
                        for (int num = 0; num < arrCmdJ[j].signals[s].description.Length; num++)
                        {
                            rawValueList.Add(arrCmdJ[j].signals[s].description[num].value);
                            physValueList.Add(arrCmdJ[j].signals[s].description[num].name);
                        }
                    }
                    else
                    {
                        int rawVal = arrCmdJ[j].signals[s].minimum;

                        if (arrCmdJ[j].signals[s].resolution == 0)
                        {
                            Console.WriteLine("CMDID: {0} NAME: {1} SIG: {2} have resolution invalid! ", arrCmdJ[j].id, arrCmdJ[j].name, arrCmdJ[j].signals[s].name);
                            MessageBox.Show("Resolution invalid!");
                            return null;
                        }
                        while (
                            (rawVal >= arrCmdJ[j].signals[s].minimum)
                            && (rawVal <= arrCmdJ[j].signals[s].maximum)
                            )
                        {
                            rawValueList.Add(rawVal);
                            rawVal = rawVal + arrCmdJ[j].signals[s].resolution;
                        }
                    }

                    sIndex = rawValueList.IndexOf(arrCmdJ[j].signals[s].init_value);

                    ViewSignalProperties signal = new ViewSignalProperties
                    {
                        name = arrCmdJ[j].signals[s].name,
                        length = arrCmdJ[j].signals[s].length,
                        startBit = arrCmdJ[j].signals[s].start_bit,
                        rawStep = arrCmdJ[j].signals[s].resolution,
                        physStep = arrCmdJ[j].signals[s].resolution,
                        rawVal = arrCmdJ[j].signals[s].init_value,
                        listRawVal = rawValueList,
                        listPhysVal = physValueList,
                        valSelectedIndex = sIndex,
                        isCreating = false,
                        minRawVal = arrCmdJ[j].signals[s].minimum,
                        maxRawVal = arrCmdJ[j].signals[s].maximum,
                        isSellectedSig = false
                    };
                    listSig.Add(signal);
                }
                arrview[j].properties.sigList = listSig;
            }

            return arrview;
        }

        public string CreateFileJson_(Object inputCMD, string nameFile)
        {
            string json = JsonConvert.SerializeObject(inputCMD, Formatting.Indented);
            //Console.WriteLine(json);
            System.IO.File.WriteAllText(nameFile, json);
            return json;
        }

        public static void TranferData_<T>(T input, ref T output)
        {
            Type type = input.GetType();
            PropertyInfo[] propInfos = type.GetProperties();
            foreach (var propInfo in propInfos)
            {
                object value = type.GetProperty(propInfo.Name).GetValue(input);
                type.GetProperty(propInfo.Name).SetValue(output, value);
            }
        }

        public List<SendCmd_> creatSendList_(ViewData[] arrview)
        {
            List<SendCmd_> listOutput = new List<SendCmd_>();

            for (int cmdIndex = 0; cmdIndex < arrview.Length; cmdIndex++)
            {
                // add all 
                if (String.Equals("SENDING..", arrview[cmdIndex].properties.sentStatus, StringComparison.CurrentCulture))
                {
                    listOutput.Add(creatSendCmd_(arrview[cmdIndex]));
                }
            }
            return listOutput;
        }

        public SendCmd_ creatSendCmd_(ViewData viewData)
        {
            List<SendCmdItemPSignal_> listSendSig = new List<SendCmdItemPSignal_>();
            foreach (var sigCmd in viewData.properties.sigList)
            {
                SendCmdItemPSignal_ sig = new SendCmdItemPSignal_
                {
                    name = sigCmd.name,
                    value = sigCmd.rawVal
                };
                listSendSig.Add(sig);
            }
            SendCmd_ cmdSend = new SendCmd_
            {
                id = viewData.properties.id,
                cycletime = viewData.properties.cycle,
                transmitter = viewData.properties.chanelList.ToArray()[viewData.properties.chanelSelectedIdx],
                // signals
                signals = listSendSig.ToArray(),
                title = "send command",
                description = "send command"
            };
            return cmdSend;
        }

        public void creatCommandJSON_(ViewData[] arrview, CommandJson_[] arrCmdCur, string namefile, Description[][] arrDes)
        {
            List<CommandJson_> listcmd = new List<CommandJson_>();
            CommandJson_ newCmd = new CommandJson_();
            
            if (arrCmdCur != null)
            {
                for (int j = 0; j < arrCmdCur.Length; j++)
                {
                    listcmd.Add(arrCmdCur[j]);
                }
            }
            for (int i = 0; i < arrview.Length; i++)
            {
                // creat array signal
                int cnt = arrview[i].properties.sigList.Count;

                Signal_[] signals = new Signal_[cnt];
                
                // TOD0:
                List<Description> des = new List<Description>();
                Description a = new Description { name = "", value = 1, comment = "" };
                des.Add(a);

                for (int j = 0; j < cnt; j++)
                {
                    signals[j] = new Signal_
                    {
                        id = 0,                                                                 // TODO: "required"
                        name = arrview[i].properties.sigList.ToArray()[j].name,
                        start_bit = arrview[i].properties.sigList.ToArray()[j].startBit,
                        length = arrview[i].properties.sigList.ToArray()[j].length,
                        byte_order = "little_endian",                                           // TODO: "required"
                        value_type = "signed",                                                  // TODO: "required"
                        description = des.ToArray(),                                                     // TODO: "required"
                        init_value = arrview[i].properties.sigList.ToArray()[j].rawVal,         // TODO: "required"
                        resolution = arrview[i].properties.sigList.ToArray()[j].rawStep,
                        minimum = arrview[i].properties.sigList.ToArray()[j].minRawVal,
                        maximum = arrview[i].properties.sigList.ToArray()[j].maxRawVal,
                    };
                }

                CommandJson_ cmd = new CommandJson_
                {
                    id = arrview[i].properties.id,
                    name = arrview[i].properties.name,
                    id_format = "standard",
                    dlc = arrview[i].properties.dlc,
                    tx_method = "cyclic",
                    cycle_time = arrview[i].properties.cycle,   // TODO: required arrview[i].properties.cycle ? int
                    transmitter = "M7G",                         // TODO: required 
                    comment = "NOT required",
                    signals = signals
                };

                // add command to list
                listcmd.Add(cmd);
                newCmd = cmd;
            }

            CommandJson_[] arrcmd = listcmd.ToArray();

            //if (arrCmdCur == null)
            //{
            //    // Call creat file json
            //    CreateFileJson_(newCmd, namefile);

            //}
            //else
            //{
            //    // Call creat file json
            //    CreateFileJson_(arrcmd, namefile);
            //}

            // Call creat file json
            CreateFileJson_(arrcmd, namefile);

        }

        public ViewData creatCommandView(ViewData[] arrviewCurrent, string nameCmd, int idCmd, List<ViewSignalProperties> listSig)
        {
            List<string> listTriger = new List<string>();
            listTriger.Add("Manual");
            listTriger.Add("Preodic: __ms");
            List<string> listType = new List<string>();
            listType.Add("CAN Data");
            listType.Add("CAN FD");
            List<string> listChanel = new List<string>();
            string[] chanelArray = ConfigurationManager.AppSettings["canChanel"].Split(',');
            for (int i = 0; i < chanelArray.Length; i++)
            {
                listChanel.Add(chanelArray[i]);
            }

            int dlcCmd = 0;
            foreach (var sig in listSig)
            {
                dlcCmd = dlcCmd + sig.length;
            }
            if (dlcCmd == 0)
            {
                Console.WriteLine("len is invalid!");
                MessageBox.Show("len is invalid!");
                return null;
            }
            int row = 0;
            if(arrviewCurrent != null)
            {
                row = arrviewCurrent.Length;

            }
            ViewData cmd = new ViewData
            {
                row = row,
                isSellectedCommand = true,
                sendSts = false,
                properties = new ViewCommandProperties
                {
                    dlc = dlcCmd,
                    id = idCmd,
                    name = nameCmd,
                    ide = false,
                    brs = false,
                    btr = false,
                    trigerList = listTriger,
                    trigerSelectedIdx = 0,
                    chanelList = listChanel,
                    chanelSelectedIdx = 0,
                    typeList = listType,
                    typeSelectedIdx = 0,
                    isCycle = false,
                    cycle = 0,
                    sentStatus = "SEND",
                    isCreating = true
                }
            };

            cmd.properties.sigList = listSig;
            return cmd;
        }

        // TO TEST: we don't use it.
        public List<ViewSignalProperties> creatListSig(int numSig, int[] minVal, int[] maxVal, int[] resolutionSig, int[] initVal, string[] nameSig, int[] lenSig)
        {
            List<ViewSignalProperties> listSig = new List<ViewSignalProperties>();

            int startBitSig = 0;

            for (int s = 0; s < numSig; s++)
            {
                // mapping data for all command
                List<int> rawValueList = new List<int>();
                List<string> physValueList = new List<string>();

                //// REMOVE: data base is mapping value
                //int rawVal = minVal[s];
                //rawValueList.Add(rawVal);
                //if (minVal[s] > maxVal[s])
                //{
                //    Console.WriteLine("data is invalid!");
                //    return null;
                //}
                //while (
                //    (rawVal > minVal[s])
                //    && (rawVal <= maxVal[s])
                //    )
                //{
                //    rawValueList.Add(rawVal);
                //    rawVal = rawVal + resolutionSig[s];
                //}

                ViewSignalProperties signal = new ViewSignalProperties
                {
                    name = nameSig[s],
                    length = lenSig[s],
                    startBit = startBitSig,
                    rawStep = resolutionSig[s],
                    physStep = resolutionSig[s],
                    rawVal = initVal[s],
                    listRawVal = rawValueList,
                    listPhysVal = physValueList,
                    valSelectedIndex = -1,          // init value
                    isCreating = true,
                    minRawVal = minVal[s],
                    maxRawVal = maxVal[s]
                };
                listSig.Add(signal);
                startBitSig = startBitSig + lenSig[s];
            }

            return listSig;
        }

        public int[] calcStartBit(int[] arrlensig)
        {
            if(arrlensig == null)
            {
                return null;
            }
            else
            {
                int[] arrOut = new int[arrlensig.Length];
               
                for(int i = 0; i < arrlensig.Length; i++)
                {
                    if(i == 0)
                    {
                        arrOut[i] = 0;
                    }
                    else
                    {
                        for(int j = 0; j < i; j++)
                        {
                            arrOut[i] = arrOut[i] + arrlensig[j];
                        }
                    }
                }
                return arrOut;
            }
        }

        public void putModule(string nameFile)
        {
            var client = new RestClient("http://127.0.0.1:9222/modules?=");
            var request = new RestRequest(Method.POST);
            var content_type = "application/octet-stream";
            request.AddHeader("Content-Type", content_type);
            var fileName = string.Concat(@"avmc\", nameFile);
            try
            {
                request.AddParameter(content_type, File.ReadAllBytes(fileName), ParameterType.RequestBody);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }

            IRestResponse response = client.Execute(request);
            Console.WriteLine("put response = {0}", response.StatusCode);
        }

        public void linkModules()
        {
            var client = new RestClient("http://127.0.0.1:9222/modules/link");
            var request = new RestRequest(Method.POST);
            IRestResponse response = client.Execute(request);
            Console.WriteLine("link response = {0}", response.StatusCode);
        }

        public void getModules()
        {
            RestClient client = new RestClient("http://127.0.0.1:9222/modules");
            RestRequest request = new RestRequest(Method.GET);
            IRestResponse response = client.Execute(request);
        }

        private void spawnModule(string nameModule, string nameMethod)
        {
            string path = String.Concat("http://127.0.0.1:9222/actors?module=", nameModule, "&name=", nameMethod);
            var client = new RestClient(path);
            var request = new RestRequest(Method.POST);
            IRestResponse response = client.Execute(request);
            Console.WriteLine("spawn response = {0}", response.StatusCode);
        }

        public int creatNewActor(string nameFile, string nameModule, string nameMethod)
        {
            putModule(nameFile);
            linkModules();
            spawnModule(nameModule, nameMethod);
            return 1;
        }

        public string listAllFileAvmc()
        {
            string str = "";
            DirectoryInfo d = new DirectoryInfo(@"avmc\");//Assuming Test is your Folder
            FileInfo[] Files = d.GetFiles("*.avmc"); //Getting Text files
            
            foreach (FileInfo file in Files)
            {
                str = str + " " +"avmc/" + file.Name;
            }
            return str;
        }

        public void buildAllFileAml()
        {
            
            DirectoryInfo d = new DirectoryInfo(@"src_avm\");//Assuming Test is your Folder
            FileInfo[] Files = d.GetFiles("*.aml"); //Getting Text files
            foreach (FileInfo file in Files)
            {
                string str = "src_avm/" + file.Name;
                //Process.Start("build_aml.bat", str).WaitForExit();
                ExecuteCommand("amlc -c " + str);
                ExecuteCommand("move *.avmc avmc");
            }
        }

        public List<int> calListRawValue(int minVal, int maxVal, int resolution)
        {
            List<int> rawValueList = new List<int>();

            int rawVal = minVal;
            if (resolution == 0)
            {
                MessageBox.Show("Resolution invalid!");
                return null;
            }
            while (
                (rawVal >= minVal)
                && (rawVal <= maxVal)
                )
            {
                rawValueList.Add(rawVal);
                rawVal += resolution;
            }
            return rawValueList;
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

        public void ide_compiler_send_oneshort()
        {
            ExecuteCommand(@"ide_compiler.exe -c send_one_shoot.json -m 0 -p map_odb.json");
            ExecuteCommand("move *.aml src_avm");
            ExecuteCommand(@"amlc -c src_avm\send_one_shoot.aml");
            ExecuteCommand("move *.avmc avmc");
            // link
            ExecuteCommand(@"amlc avmc\send_one_shoot.avmc");
        }

        private static bool isFirstSendPreodic = true;
        public void ide_compiler_send_preodic()
        {
            ExecuteCommand(@"ide_compiler.exe -c send_periodic.json -m 1 -p map_odb.json");
            ExecuteCommand(@"move send_periodic.aml src_avm\send_periodic.aml");
            ExecuteCommand(@"amlc -c src_avm\send_periodic.aml");
            ExecuteCommand("move *.avmc avmc");
            // link
            ExecuteCommand(@"amlc avmc\send_periodic.avmc ");
            // put and link in sever
            putModule("send_periodic.avmc");
            linkModules();

            ExecuteCommand(@"amlc -c src_avm\send_preodic_instance\send_periodic_instance.aml");
            ExecuteCommand("move *.avmc avmc");

            if (isFirstSendPreodic)
            {
                ExecuteCommand(@"amlc -c src_avm\send_preodic_instance\send_periodic_instance.aml");
                ExecuteCommand("move *.avmc avmc");
                //link
                ExecuteCommand(@"amlc avmc\send_periodic_instance.avmc");
                // START send command with preodic
                creatNewActor("send_periodic_instance.avmc", "send_periodic", "main/0");
                isFirstSendPreodic = false;
            }
        }

        public void clear_file()
        {
            // delete file avmc
            ExecuteCommand(@"del src_avm\send_one_shoot.aml");
            ExecuteCommand(@"del src_avm\send_periodic.aml");
            ExecuteCommand(@"del avmc\send_one_shoot.avmc");
            ExecuteCommand(@"del avmc\send_periodic.avmc");
            ExecuteCommand(@"del avmc\send_periodic_instance.avmc");
        }
    }

}
