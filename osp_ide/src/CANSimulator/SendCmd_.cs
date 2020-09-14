using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WpfApplication3
{
    public class SendCmd_
    {
        public int id { get; set; }
        public int cycletime { get; set; }
        public string transmitter { get; set; }
        public class SendCmdItemPSignal_
        {
            public string name { get; set; }
            public int value { get; set; }

        }
        public SendCmdItemPSignal_[] signals { get; set; }
        public string title { get; set; }
        public string description { get; set; }
    }
}


//public class SendCmd_
//{
//    public int id { get; set; }
//    public int cycletime { get; set; }
//    public string transmitter { get; set; }
//    public Signal[] signals { get; set; }
//    public string title { get; set; }
//    public string description { get; set; }
//}

//public class Signal
//{
//    public string name { get; set; }
//    public int value { get; set; }
//}
