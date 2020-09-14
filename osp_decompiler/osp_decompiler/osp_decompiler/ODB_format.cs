using Newtonsoft.Json;
using System.Collections.Generic;

namespace osp_decompiler
{
    public class ODB_format
    {
        [JsonProperty(PropertyName = "module")]
        public string module { get; set; }
        [JsonProperty(PropertyName = "command_id")]
        public string command_id { get; set; }
        [JsonProperty(PropertyName = "signals")]
        public List<Signal_format> signals { get; set; }
    }

    public class Signal_format
    {
        [JsonProperty(PropertyName = "name")]
        public string name { get; set; }
        [JsonProperty(PropertyName = "getter")]
        public string getter { get; set; }
        [JsonProperty(PropertyName = "setter")]
        public string setter { get; set; }
    }
}
