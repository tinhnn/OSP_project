using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Threading;

namespace WpfApplication3
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public ViewData[] arrview { get; set; }
        public ViewCommandProperties cmdPro { get; set; }
        public List<ViewSignalProperties> listSigPro { get; set; }
        public List<SendCmd_> listSend;
        public int selectedCmdIdx;
        public int selectedSigIdx;
        private static Utility_ util;
        private ReadAndCheckData check;
        private int curNumCmd;
        public CommandJson_[] arrCmdJson { get; set; }
        public int newCmd_ID { get; set; }
        public int newCmd_numSig { get; set; }
        public static void TranferData<T>(T input, ref T output)
        {
            Type type = input.GetType();
            PropertyInfo[] propInfos = type.GetProperties();
            foreach (var propInfo in propInfos)
            {
                object value = type.GetProperty(propInfo.Name).GetValue(input);
                type.GetProperty(propInfo.Name).SetValue(output, value);
            }
        }

        Thread thread = new Thread(new ThreadStart(WorkThreadFunction));

        public static void WorkThreadFunction()
        {
            try
            {
                ////  gen file osp-dbc from json
                //util.ExecuteCommand(@"osp-dbc.exe -c command_data.json");
                //util.ExecuteCommand("move *.aml src_avm");
                //// build to trunk code
                //util.buildAllFileAml();
                string a = util.listAllFileAvmc();
                //Link all file avmc
                Process.Start("test.bat", util.listAllFileAvmc());
            }
            catch(Exception e)
            {
                // TODO: 
                Console.WriteLine(e.ToString());
            }
        }

        public MainWindow()
        {
            // init component
            InitializeComponent();

            // Log Console Message
            lLog.Items.Insert(0, "Starting Application...");

            cmdPro = new ViewCommandProperties();
            listSigPro = new List<ViewSignalProperties>();

            listSend = new List<SendCmd_>();
            util = new Utility_();
            check = new ReadAndCheckData();

            bool result = check.CheckCommand(util.LoadJson_());
            lLog.Items.Insert(0, "Read and check all command from data. result =" + result);

            arrCmdJson = JsonConvert.DeserializeObject<CommandJson_[]>(util.LoadJson_());

            // Binding data
            DataContext = this;
            lLog.Items.Insert(0, "Bindind data ..");

            // init all data
            thread.Start();
        }

        private void listCmd_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (lCmd.SelectedIndex == -1)
            {
                return;
            }
            // select index
            selectedCmdIdx = lCmd.SelectedIndex;
            if (arrview == null)
            {
                Console.WriteLine("not view !");
                lLog.Items.Insert(0, "ERR: not view");
                lSig.ItemsSource = null;
                // update data
                spProperties.IsEnabled = false;
                return;
            }
            spProperties.IsEnabled = true;

            for (var i = 0; i < arrview.Length; i++)
            {
                if (i == lCmd.SelectedIndex)
                {
                    //lLog.Items.Insert(0, "Sellect command name = " + arrview[i].properties.name); 
                    arrview[i].isSellectedCommand = true;
                    // update data
                    ViewCommandProperties temp = cmdPro;
                    TranferData(arrview[i].properties, ref temp);
                }
                else
                {
                    arrview[i].isSellectedCommand = false;
                }
            }
            listSigPro = arrview[selectedCmdIdx].properties.sigList;
            // update data
            lSig.ItemsSource = listSigPro;
         
        }

        private void listSig_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (lSig.SelectedIndex == -1)
            {
                return;
            }
            // select index
            selectedSigIdx = lSig.SelectedIndex;
            if(arrview == null)
            {
                return;
            }

            for (var i = 0; i < arrview[selectedCmdIdx].properties.sigList.Count; i++)
            {
                if (i == lSig.SelectedIndex)
                {
                    arrview[selectedCmdIdx].properties.sigList.ToArray()[i].isSellectedSig = true;
                    listSigPro.ToArray()[selectedSigIdx].isSellectedSig = true;
                    //lLog.Items.Insert(0, "Sellect signal name = " + listSigPro.ToArray()[selectedSigIdx].name);
                }
                else
                {
                    arrview[selectedCmdIdx].properties.sigList.ToArray()[i].isSellectedSig = false;
                    listSigPro.ToArray()[i].isSellectedSig = false;
                }
            }
            // update data
            //lSig.ItemsSource = listSigPro;

        }

        private void cbTriger_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cb = sender as ComboBox;
            if (cb.SelectedIndex == -1)
            {
                return;
            }
            lLog.Items.Insert(0, "cbTriger_SelectionChanged item = " + cb.SelectedValue);
            //// update triger and send status view
            //arrview[selectedCmdIdx].properties.trigerSelectedIdx = cb.SelectedIndex;
            //arrview[selectedCmdIdx].properties.isCycle = arrview[selectedCmdIdx].properties.trigerSelectedIdx == 1 ? true : false; /* 0: manual 1: cycle*/
            if (!arrview[selectedCmdIdx].properties.isCycle)
            {
                // SENDING -> END
                if (String.Equals("SENDING..", arrview[selectedCmdIdx].properties.sentStatus, StringComparison.CurrentCulture))
                {
                    // Stop it
                    lLog.Items.Insert(0, "ADD: PAUSE send preodic cmdID = " + arrview[selectedCmdIdx].properties.id);
                    arrview[selectedCmdIdx].properties.sentStatus = "SEND";
                    listSend = util.creatSendList_(arrview);
                    // Creat json.
                    string json = util.CreateFileJson_(listSend.ToArray(), "send_periodic.json");
                    check.CheckSendMessage(json);
                    lLog.Items.Insert(0, "Call ide compiler to gen file send_periodic.json");
                    util.ide_compiler_send_preodic();
                    lLog.Items.Insert(0, "build to trunk code and PAUSE send command with preodic");
                }
                arrview[selectedCmdIdx].properties.sentStatus = "SEND";
            }
            else
            {
                if(String.Equals("SEND", arrview[selectedCmdIdx].properties.sentStatus, StringComparison.CurrentCulture))
                {
                    arrview[selectedCmdIdx].properties.sentStatus = "START";
                }
            }
            ViewCommandProperties temp = cmdPro;
            TranferData(arrview[selectedCmdIdx].properties, ref temp);
        }

        private void cbChanel_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // TODO:
            ComboBox cb = sender as ComboBox;
            if (cb.SelectedIndex == -1)
            {
                return;
            }
            Console.WriteLine("cbChanel_SelectionChanged item = {0}", cb.SelectedValue);
            // update chanel 
            arrview[selectedCmdIdx].properties.chanelSelectedIdx = cb.SelectedIndex;

            ViewCommandProperties temp = cmdPro;
            TranferData(arrview[selectedCmdIdx].properties, ref temp);
        }

        private void cbRawVal_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cb = sender as ComboBox;
            if(cb.SelectedIndex == -1)
            {
                return;
            }
            if (arrview[selectedCmdIdx].properties.sigList.ToArray() != null)
            {
                arrview[selectedCmdIdx].properties.sigList.ToArray()[selectedSigIdx].rawVal = int.Parse(cb.SelectedValue.ToString());
            }
            ViewCommandProperties temp = cmdPro;
            TranferData(arrview[selectedCmdIdx].properties, ref temp);

            // TODO: 
            // incase send preodic data. 
            // if you change data value. We need rebuild and update data.
            //if (String.Equals("SENDING..", arrview[selectedCmdIdx].properties.sentStatus, StringComparison.CurrentCulture))
            //{
            //    listSend.Clear();
            //    listSend = util.creatSendList_(arrview);
            //    // Rebuild send cycle data
            //    // And push + update data
            //    string json = util.CreateFileJson_(listSend.ToArray(), "send_periodic.json");
            //    check.CheckSendMessage(json);
            //    lLog.Items.Insert(0, "Call ide compiler to gen file send_periodic.json");
            //    //Process.Start("ide_compiler_send_preodic.bat").WaitForExit();
            //    util.ide_compiler_send_preodic();
            //    lLog.Items.Insert(0, "build to trunk code and PAUSE send command with preodic");
            //}
        }

        private void cbType_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // TODO:
            ComboBox cb = sender as ComboBox;
            if (cb.SelectedIndex == -1)
            {
                return;
            }
            Console.WriteLine("cbType_SelectionChanged item = {0}", cb.SelectedValue);
            string type = "CAN FD";
            bool brs = String.Equals(type, cmdPro.typeList[cb.SelectedIndex], StringComparison.CurrentCulture);
            // update type 
            arrview[selectedCmdIdx].properties.typeSelectedIdx = cb.SelectedIndex;
            arrview[selectedCmdIdx].properties.brs = brs;

            ViewCommandProperties temp = cmdPro;
            TranferData(arrview[selectedCmdIdx].properties, ref temp);
        }

        private void btSend_Click(object sender, RoutedEventArgs e)
        {
            Button btn = sender as Button;
            Console.WriteLine("btSend_Click content = {0}", btn.Content);
            lLog.Items.Insert(0, "SEND.............");

            if (String.Equals("SEND", btn.Content.ToString(), StringComparison.CurrentCulture))
            {
                //Send manual command
                lLog.Items.Insert(0, "ADD: send oneshot cmdID = " + arrview[selectedCmdIdx].properties.id);

                // listSend = util.creatSendList_(arrview);
                List<SendCmd_> listSendOneShot = new List<SendCmd_>();
                listSendOneShot.Add( util.creatSendCmd_(arrview[selectedCmdIdx]));
                // Creat json.
                string json = util.CreateFileJson_(listSendOneShot.ToArray(), "send_one_shoot.json");
                check.CheckSendMessage(json);
                lLog.Items.Insert(0, "Call ide compiler to gen file send_one_shoot.json");
                //Process.Start("ide_compiler_send_oneshort.bat").WaitForExit();
                util.ide_compiler_send_oneshort();
                lLog.Items.Insert(0, "build to trunk code");
                string funcname = "send_msg_" + arrview[selectedCmdIdx].properties.id + "/0";
                lLog.Items.Insert(0, "creatNewActor send_oneshoot:" + funcname);
                util.creatNewActor("send_one_shoot.avmc", "send_oneshoot", funcname);
                lLog.Items.Insert(0, "FINISH..............");
            }
            else if(String.Equals("START", btn.Content.ToString(), StringComparison.CurrentCulture))
            {
                listSend.Clear();
                lLog.Items.Insert(0, "ADD: send preodic cmdID = " + arrview[selectedCmdIdx].properties.id + ". preodic = " + arrview[selectedCmdIdx].properties.cycle);
                arrview[selectedCmdIdx].properties.sentStatus = "SENDING..";
                listSend = util.creatSendList_(arrview);
                // Creat json.
                string json = util.CreateFileJson_(listSend.ToArray(), "send_periodic.json");
                check.CheckSendMessage(json);
                lLog.Items.Insert(0, "Call ide compiler to gen file send_periodic.json");
                util.ide_compiler_send_preodic();
                lLog.Items.Insert(0, "build to trunk code and START send command with preodic");
            }
            else if (String.Equals("SENDING..", btn.Content.ToString(), StringComparison.CurrentCulture))
            {
                listSend.Clear();
                lLog.Items.Insert(0, "ADD: PAUSE send preodic cmdID = " + arrview[selectedCmdIdx].properties.id);
                arrview[selectedCmdIdx].properties.sentStatus = "START";
                listSend = util.creatSendList_(arrview);
                // Creat json.
                string json = util.CreateFileJson_(listSend.ToArray(), "send_periodic.json");
                check.CheckSendMessage(json);
                lLog.Items.Insert(0, "Call ide compiler to gen file send_periodic.json");
                util.ide_compiler_send_preodic();
                lLog.Items.Insert(0, "build to trunk code");
                lLog.Items.Insert(0, "build to trunk code and PAUSE send command with preodic");
            }
            else
            {
                lLog.Items.Insert(0, "ERR: send item = " + btn.Content);
            }
            // TODO: notify SEND.
        }

        private void btStartCreat_Click(object sender, RoutedEventArgs e)
        {
            // start creat command. Can't sellect command and view properties
            //isViewCommand = false;
            lLog.Items.Insert(0, "creat new command with ID " + newCmd_ID + "number signal = " + newCmd_numSig);

            Console.WriteLine("creat new cmdID = {0} , numSig = {1}, ...", newCmd_ID, newCmd_numSig);
            // Check.
            if (newCmd_numSig < 1 || newCmd_numSig > 8)
            {
                lLog.Items.Insert(0, "ERR: number signal is invalid!");
                MessageBox.Show("ERR: number signal is invalid!");
                return;
            }

            if(arrCmdJson != null)
            {
                for (int i = 0; i < arrCmdJson.Length; i++)
                {
                    if (arrCmdJson[i].id == newCmd_ID)
                    {
                        lLog.Items.Insert(0, "command ID is exist!");
                        MessageBox.Show("command ID is exist!");
                        return;
                    }
                }
            }
            if(arrview != null)
            {
                for (int i = 0; i < arrview.Length; i++)
                {
                    if (arrview[i].properties.id == newCmd_ID)
                    {
                        lLog.Items.Insert(0, "command ID is exist!");
                        MessageBox.Show("command ID is exist!");
                        return;
                    }
                }
            }
            // remove all old view command to add new command
            arrview = null;
            lCmd.ItemsSource = null;

            // update arrview 
            List<ViewData> ltCmd = new List<ViewData>();
            // TEST DATA
            int[] minVal = new int[newCmd_numSig]; 
            int[] maxVal = new int[newCmd_numSig];
            int[] resolutionSig = new int[newCmd_numSig];
            int[] initVal = new int[newCmd_numSig];
            int[] lenSig = new int[newCmd_numSig];
            string[] sigName = new string[newCmd_numSig];


            for (int i = 0; i < newCmd_numSig; i++)
            {
                // init default val
                resolutionSig[i] = 1;
                sigName[i] = "sig_name_" + i.ToString();
                // init
                lenSig[i] = 1;
            }

            ViewData newCmd = util.creatCommandView(arrview, "CMDName___new" + newCmd_ID.ToString(), newCmd_ID, util.creatListSig(newCmd_numSig, minVal, maxVal, resolutionSig, initVal, sigName, lenSig));

            if(newCmd == null)
            {
                Console.WriteLine("can not creat new cmd !");
                MessageBox.Show("can not creat new cmd !");
                return;
            }
            // update view
            spChoiceListCommand.IsEnabled = false;
            spProperties.IsEnabled = false;
            //if(arrview != null)
            //{
            //    for (int i = 0; i < arrview.Length; i++)
            //    {
            //        ltCmd.Add(arrview[i]);
            //    }
            //}

            ltCmd.Add(newCmd);
            arrview = ltCmd.ToArray();

            // update
            lCmd.ItemsSource = arrview;

            // only support add one command
            btStartCreat.IsEnabled = false;

            btSave.IsEnabled = true;
        }

        private void btSave_Click(object sender, RoutedEventArgs e)
        {
            if (isValidData() == false)
            {
                lLog.Items.Insert(0, "ERR: can't creat command! Please re-input data!");
                MessageBox.Show("ERR: can't creat command! Please re-input data!");
                return;
            }

            // finish creat command. You can sellect command and view properties
            //isViewCommand = true;
            spChoiceListCommand.IsEnabled = true;
            spProperties.IsEnabled = true;
            // only support add one command
            btStartCreat.IsEnabled = true;

            if(arrview.Length == 0)
            {
                lLog.Items.Insert(0, "new command did not created.");
                return;
            }

            string namenewmodule = "";
            string pathnewFileAml = ".aml";
            // creat file json for new command
            if (curNumCmd < arrview.Length)
            {
                List<ViewData> newCmd = new List<ViewData>();
                for (int i = curNumCmd; i < arrview.Length; i++)
                {
                    newCmd.Add(arrview[i]);
                }
                // creat json file
                util.creatCommandJSON_(newCmd.ToArray(), null, "new_command.json", null);
                namenewmodule = newCmd.ToArray()[0].properties.name;
                pathnewFileAml = "src_avm/" + namenewmodule.ToLower() + ".aml";
            }

            //TODO: currently we only creat file json for new command
            // and command data base module will handle it and update file command json
            // we will reload json.
            util.creatCommandJSON_(arrview, arrCmdJson, "command_data.json", null);
            lLog.Items.Insert(0, "save new command to file new_command.json");

            // reset arrview
            arrview = null;
            lCmd.ItemsSource = null;


            // reload command
            check.CheckCommand(util.LoadJson_());
            arrCmdJson = JsonConvert.DeserializeObject<CommandJson_[]>(util.LoadJson_());

            lbChoiceListCmd.ItemsSource = arrCmdJson;

            // call gen file aml
            lLog.Items.Insert(0, "TEST: call complie new command -> new_command.avmc");
            //Process.Start("gencmd.bat");
            util.ExecuteCommand(@"osp-dbc.exe -c new_command.json");
            util.ExecuteCommand(@"move *.aml src_avm");
            // build and link
            util.ExecuteCommand("amlc -c " + pathnewFileAml);
            util.ExecuteCommand("move *.avmc avmc");
            // TODO: re-update file map-odb.json

            // put and link
            util.putModule(namenewmodule.ToLower() + ".avmc");
            util.linkModules();

            // gen file aml
            lLog.Items.Insert(0, "New command is created!");
            MessageBox.Show("New command is created!");
            btSave.IsEnabled = false;
        }

        private void txLenSigChange_TextChanged(object sender, TextChangedEventArgs e)
        {
            // update len
            TextBox tx = sender as TextBox;
            if (tx.Text.Length == 0)
            {
                return;
            }
            Console.WriteLine("CmdID = {0} signame = {1}  len(bit) = {2}", arrview[selectedCmdIdx].properties.id, arrview[selectedCmdIdx].properties.sigList.ToArray()[selectedSigIdx].name, tx.Text);
            int dlc = 0;
            List<int> lenSig = new List<int>();
            foreach(var sig in arrview[selectedCmdIdx].properties.sigList)
            {
                if(sig == null)
                {
                    lLog.Items.Insert(0, "len input is invalid!");
                    MessageBox.Show("len input is invalid!");
                    return;
                }
                if (sig.isSellectedSig)
                {
                    sig.length = int.Parse(tx.Text.ToString());
                }
                dlc = dlc + sig.length;
                lenSig.Add(sig.length);
            }
            //if(dlc < 1 || dlc > 8)
            //{
            //    lLog.Items.Insert(0, "len is invalid!");
            //    MessageBox.Show("len is invalid!");
            //    return;
            //}
            //arrview[selectedCmdIdx].properties.dlc = dlc;

            //lLog.Items.Insert(0, "newCmdNo = " + arrview[selectedCmdIdx].properties.id + ". Updated dlc = " + dlc);

            //int[] arrStartBit = util.calcStartBit(lenSig.ToArray());
            //for(int i = 0; i < arrStartBit.Length; i++)
            //{
            //    arrview[selectedCmdIdx].properties.sigList.ToArray()[i].startBit = arrStartBit[i];
            //}

            // update view
            //lCmd.ItemsSource = arrview;
        }

        private void txStartBitChange_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox tx = sender as TextBox;
            if (tx.Text.Length == 0)
            {
                return;
            }
            Console.WriteLine("CmdID = {0} sigName = {1} startbit = {2}", arrview[selectedCmdIdx].properties.id, arrview[selectedCmdIdx].properties.sigList.ToArray()[selectedSigIdx].name, tx.Text);
        }

        private void txDLCChange_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox tx = sender as TextBox;
            if(tx.Text.Length == 0)
            {
                return;
            }
            Console.WriteLine("CmdID = {0} dlc = {1}", arrview[selectedCmdIdx].properties.id, tx.Text);
            arrview[selectedCmdIdx].properties.dlc = int.Parse(tx.Text.ToString());
            if ( int.Parse(tx.Text.ToString()) > 8)
            {
                Console.WriteLine("ERR: dlc is invalid!");
                MessageBox.Show("DLC is invalid!");
            }
        }

        private void txCycleChange_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox tx = sender as TextBox;
            if(tx.Text.Length== 0||int.Parse(tx.Text.ToString()) < 0)
            {
                return;
            }
            //if (arrview[selectedCmdIdx].properties.sentStatus.Equals("SENDING.."))
            //{
            //    listSend.Clear();
            //    arrview[selectedCmdIdx].properties.cycle = int.Parse(tx.Text.ToString());
            //    listSend = util.creatSendList_(arrview);
            //    // Creat json.
            //    string json = util.CreateFileJson_(listSend.ToArray(), "send_periodic.json");
            //    check.CheckSendMessage(json);
            //    lLog.Items.Insert(0, "Call ide compiler to gen file send_periodic.json");
            //    //Process.Start("ide_compiler_send_preodic.bat").WaitForExit();
            //    util.ide_compiler_send_preodic();
            //    lLog.Items.Insert(0, "build to trunk code and UPDATE send command with new preodic = " + tx.Text.ToString());
            //}
        }


        private static bool isFirst = false;
        private void btSaveSellectCmd_Click(object sender, RoutedEventArgs e)
        {
            // TODO:
            if (isFirst)
            {
                // stop all preodic
                // Creat json.
                listSend.Clear();
                string json = util.CreateFileJson_(listSend.ToArray(), "send_periodic.json");
                lLog.Items.Insert(0, "stop all preodic command");
                util.ide_compiler_send_preodic();
            }

            if (lbChoiceListCmd.SelectedItems.Count > 0)
            {
                isFirst = true;
                List<CommandJson_> listCmd = new List<CommandJson_>();
                foreach (CommandJson_ item in lbChoiceListCmd.SelectedItems)
                {
                    lLog.Items.Insert(0, "user sellect: id = " + item.id + ". name = " + item.name + ". dlc = " + item.dlc);
                    listCmd.Add(item);
                }
                // Parsing to view
                arrview = util.ParsingJsonDataToView_(listCmd.ToArray());
            }
            else
            {
                arrview = null;
            }
            // save number view command 
            if(arrview == null)
            {
                curNumCmd = 0;
            }
            else
            {
                curNumCmd = arrview.Length;
            }
            lCmd.ItemsSource = arrview;
        }

        private void btSellectAllCmd_Click(object sender, RoutedEventArgs e)
        {
            lbChoiceListCmd.SelectAll();
            lLog.Items.Insert(0, "Sellected all command.");
        }

        private void btUnSellectAllCmd_Click(object sender, RoutedEventArgs e)
        {
            lbChoiceListCmd.UnselectAll();
            lLog.Items.Insert(0, "Un-Sellected all command.");
        }

        private void testFunc()
        {
            lLog.Items.Insert(0, "TEST: call test RF.");
            // call get
            util.getModules();

            // creat new actor 
            util.creatNewActor("new.avmc", "test_resful", "main/0");

        }

        //private void btAssignSignalValue_Click(object sender, RoutedEventArgs e)
        //{
        //    // open new windown
        //    List<int> input = util.calListRawValue(0,2,1);
        //    win2 = new NewCmd(input);
        //    win2.Show();
        //}

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            // Remove all amvc file
            util.clear_file();
            // Close console
            Process.Start("close.bat");
        }

        private bool isValidData()
        {
            int dlc = arrview[selectedCmdIdx].properties.dlc;
            List<int> lStartBit = new List<int>();
            List<int> lSigLengh = new List<int>();

            foreach(var sig in arrview[selectedCmdIdx].properties.sigList)
            {
                lStartBit.Add(sig.startBit);
                lSigLengh.Add(sig.length);
            }

            // check start bit is valid ?
            for(int i = 1; i < lStartBit.Count; i++)
            {
                if(lStartBit.ToArray()[i] < lStartBit.ToArray()[i - 1] + lSigLengh.ToArray()[i - 1])
                {
                    lLog.Items.Insert(0, "ERR: start bit in signame = " + arrview[selectedCmdIdx].properties.sigList.ToArray()[i].name + " is invalid");
                    return false;
                }
            }

            // check len is valid ?
            int numSig = lSigLengh.Count;
            int totalLen = lSigLengh.ToArray()[numSig - 1] + lStartBit.ToArray()[numSig - 1];
            if (totalLen > dlc*8)
            {
                lLog.Items.Insert(0, "ERR: data is invalid. dlc < total sig len ");
                return false;
            }

            return true;
        }

        private void btUpdate_Click(object sender, RoutedEventArgs e)
        {
            // only handle when press update data
            if (arrview[selectedCmdIdx].properties.sentStatus.Equals("SENDING.."))
            {
                if (arrview[selectedCmdIdx].properties.cycle <= 0)
                {
                    //show log ERROR
                    lLog.Items.Insert(0, "cycle time is invalid! Please re-input value!");
                    MessageBox.Show("cycle time is invalid! Please re-input value!");
                    return;
                }

                listSend.Clear();
                listSend = util.creatSendList_(arrview);
                // Creat json.
                string json = util.CreateFileJson_(listSend.ToArray(), "send_periodic.json");
                check.CheckSendMessage(json);
                lLog.Items.Insert(0, "Call ide compiler to gen file send_periodic.json");
                util.ide_compiler_send_preodic();
                lLog.Items.Insert(0, "build to trunk code and UPDATE send command with new preodic = " + arrview[selectedCmdIdx].properties.cycle);
            }
        }

        private void btSendAll_Click(object sender, RoutedEventArgs e)
        {
            if (arrview == null)
            {
                return;
            }
            // TODO: issue send all non-panic func. WHY ? WHY ?
            // creat list
            List<SendCmd_> listSendOneShort = new List<SendCmd_>();
            for (int i = 0; i < arrview.Length; i++)
            {
                if (String.Equals("SEND", arrview[i].properties.sentStatus, StringComparison.CurrentCulture))
                {
                    listSendOneShort.Add(util.creatSendCmd_(arrview[i]));
                }
                else if (String.Equals("START", arrview[i].properties.sentStatus, StringComparison.CurrentCulture))
                {
                    arrview[i].properties.sentStatus = "SENDING..";
                    listSend.Add(util.creatSendCmd_(arrview[i]));
                }
            }

            // send
            // Creat json.
            if (listSendOneShort.Count > 0)
            {
                string json = util.CreateFileJson_(listSendOneShort.ToArray(), "send_one_shoot.json");
                check.CheckSendMessage(json);
                lLog.Items.Insert(0, "Call ide compiler to gen file send_one_shoot.json");
                util.ide_compiler_send_oneshort();
                lLog.Items.Insert(0, "build to trunk code");
                string funcname = "send_msg_" + arrview[selectedCmdIdx].properties.id + "/0";
                lLog.Items.Insert(0, "creatNewActor send_oneshoot:" + funcname);
                util.creatNewActor("send_one_shoot.avmc", "send_oneshoot", funcname);
                lLog.Items.Insert(0, "FINISH..............");
            }

            if (listSend.Count > 0)
            {
                // Creat json.
                string json = util.CreateFileJson_(listSend.ToArray(), "send_periodic.json");
                check.CheckSendMessage(json);
                lLog.Items.Insert(0, "Call ide compiler to gen file send_periodic.json");
                util.ide_compiler_send_preodic();
                lLog.Items.Insert(0, "build to trunk code and START send command with preodic");
            }

        }

        private void btStopAll_Click(object sender, RoutedEventArgs e)
        {
            if (arrview == null)
            {
                return;
            }
            // creat list
            listSend.Clear();
            for (int i = 0; i < arrview.Length; i++)
            {
                if (String.Equals("SENDING..", arrview[i].properties.sentStatus, StringComparison.CurrentCulture))
                {
                    arrview[i].properties.sentStatus = "START";
                }
            }

            // Creat json.
            string json = util.CreateFileJson_(listSend.ToArray(), "send_periodic.json");
            check.CheckSendMessage(json);
            lLog.Items.Insert(0, "Call ide compiler to gen file send_periodic.json");
            util.ide_compiler_send_preodic();
            lLog.Items.Insert(0, "build to trunk code and END send command with preodic");

        }
    }

}
