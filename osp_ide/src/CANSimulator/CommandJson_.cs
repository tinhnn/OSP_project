namespace WpfApplication3
{
    public class Description
    {
        public string name { get; set; }
        public int value { get; set; }
        public string comment { get; set; }
    }

    public class Signal_
    {
        public int id { get; set; }
        public string name { get; set; }
        public int start_bit { get; set; }
        public int length { get; set; }
        public string byte_order { get; set; }
        public string value_type { get; set; }
        public Description[] description { get; set; }
        public int init_value { get; set; }
        public int resolution { get; set; }
        public int minimum { get; set; }
        public int maximum { get; set; }
    }

    public class CommandJson_
    {
        public int id { get; set; }
        public string name { get; set; }
        public string id_format { get; set; }
        public int dlc { get; set; }
        public string tx_method { get; set; }
        public int cycle_time { get; set; }
        public string transmitter { get; set; }
        public string comment { get; set; }
        public Signal_[] signals { get; set; }
    }
}

/*-----------
---AUTO GEN--
-----------*/

//public class Rootobject
//{
//    public int id { get; set; }
//    public string name { get; set; }
//    public string id_format { get; set; }
//    public int dlc { get; set; }
//    public string tx_method { get; set; }
//    public int cycle_time { get; set; }
//    public string transmitter { get; set; }
//    public string comment { get; set; }
//    public Signal[] signals { get; set; }
//}

//public class Signal
//{
//    public int id { get; set; }
//    public string name { get; set; }
//    public int start_bit { get; set; }
//    public int length { get; set; }
//    public string byte_order { get; set; }
//    public string value_type { get; set; }
//    public string description { get; set; }
//    public int init_value { get; set; }
//    public int resolution { get; set; }
//    public int minimum { get; set; }
//    public int maximum { get; set; }
//}